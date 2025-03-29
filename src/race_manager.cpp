// race_manager.cpp
#include "race_manager.h"
#include <sqlite3.h>
#include <iostream>
#include <stdexcept>

using namespace std;

RaceManager::RaceManager(sqlite3* db) : Database(db) {}

void RaceManager::distribute_prizes(double prize_fund) {
    // Получаем последний забег
    string last_race_sql = 
        "SELECT id, date FROM races "
        "ORDER BY date DESC LIMIT 1;";
    
    string race_id;
    string race_date;
    
    execute(last_race_sql, [&](sqlite3_stmt* stmt) {
        race_id = to_string(sqlite3_column_int(stmt, 0));
        race_date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
    });
    
    if(race_id.empty()) {
        throw runtime_error("Нет завершенных забегов");
    }

    // Проверяем количество участников
    string count_sql = 
        "SELECT COUNT(*) FROM races "
        "WHERE id = ? AND position BETWEEN 1 AND 3;";
    
    int participants = 0;
    execute(count_sql, [&](sqlite3_stmt* stmt) {
        participants = sqlite3_column_int(stmt, 0);
    }, {race_id});
    
    if(participants < 3) {
        throw runtime_error("Для распределения призов необходимо минимум 3 участника");
    }

    // Удаляем предыдущее распределение
    string delete_sql = "DELETE FROM prize_distribution WHERE race_id = ?;";
    execute_parameterized(delete_sql, {race_id});

    // Распределяем призы
    string insert_sql = 
        "INSERT INTO prize_distribution (race_id, position, amount, distribution_date) "
        "SELECT ?, position, "
        "       CASE position "
        "         WHEN 1 THEN ? * 0.5 "
        "         WHEN 2 THEN ? * 0.3 "
        "         WHEN 3 THEN ? * 0.2 "
        "       END, "
        "       date('now') "
        "FROM races "
        "WHERE id = ? AND position BETWEEN 1 AND 3;";

    transaction([&]() {
        execute_parameterized(insert_sql, {
            race_id, 
            to_string(prize_fund),
            to_string(prize_fund),
            to_string(prize_fund),
            race_id
        });
    });

    // Формируем JSON-отчет
    string json_sql =
        "UPDATE prize_funds SET "
        "total_amount = ?, "
        "distribution = json_object( "
        "   '1st', (SELECT amount FROM prize_distribution WHERE position = 1), "
        "   '2nd', (SELECT amount FROM prize_distribution WHERE position = 2), "
        "   '3rd', (SELECT amount FROM prize_distribution WHERE position = 3) "
        ") WHERE race_id = ?;";

    execute_parameterized(json_sql, {
        to_string(prize_fund),
        race_id
    });

    cout << "Призы успешно распределены для забега " << race_date << endl;
    cout << "1 место: " << prize_fund * 0.5 << endl;
    cout << "2 место: " << prize_fund * 0.3 << endl;
    cout << "3 место: " << prize_fund * 0.2 << endl;
}

void RaceManager::validate_race_entries() {
    string sql = 
        "SELECT r.id, r.date, h.name, j.surname "
        "FROM races r "
        "LEFT JOIN horses h ON r.horse_id = h.id "
        "LEFT JOIN jockeys j ON r.jockey_id = j.id "
        "WHERE h.id IS NULL OR j.id IS NULL;";
    
    int invalid_count = 0;
    
    execute(sql, [&](sqlite3_stmt* stmt) {
        cerr << "Некорректная запись: Забег #" << sqlite3_column_int(stmt, 0)
             << " (" << sqlite3_column_text(stmt, 1) << ")"
             << " Лошадь: " << sqlite3_column_text(stmt, 2)
             << " Жокей: " << sqlite3_column_text(stmt, 3) << endl;
        invalid_count++;
    });
    
    if(invalid_count > 0) {
        throw runtime_error("Обнаружены некорректные записи в таблице забегов");
    }
}

void RaceManager::schedule_race(
    const string& date,
    int race_number,
    const vector<pair<int, int>>& participants
) {
    string sql = 
        "INSERT INTO races (date, race_number, horse_id, jockey_id) "
        "VALUES (?, ?, ?, ?);";
    
    transaction([&]() {
        for(const auto& [horse_id, jockey_id] : participants) {
            execute_parameterized(sql, {
                date,
                to_string(race_number),
                to_string(horse_id),
                to_string(jockey_id)
            });
        }
    });
    
    cout << "Забег #" << race_number << " запланирован на " << date 
         << " с " << participants.size() << " участниками" << endl;
}

void RaceManager::record_results(
    const string& race_id,
    const vector<pair<int, int>>& rankings // pair<horse_id, position>
) {
    string update_sql = 
        "UPDATE races SET position = ? "
        "WHERE id = ? AND horse_id = ?;";
    
    transaction([&]() {
        for(const auto& [horse_id, position] : rankings) {
            execute_parameterized(update_sql, {
                to_string(position),
                race_id,
                to_string(horse_id)
            });
        }
    });
    
    cout << "Результаты для забега " << race_id << " обновлены" << endl;
}