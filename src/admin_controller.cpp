// admin_controller.cpp
#include "admin_controller.h"
#include <sqlite3.h>
#include <iostream>
#include <iomanip>
#include <vector>
#include <stdexcept>

using namespace std;

AdminController::AdminController(sqlite3* db) : Database(db) {}

void AdminController::insert() {
    vector<string> tables = {"horses", "jockeys", "races"};
    cout << "Выберите таблицу для добавления:\n";
    for(size_t i = 0; i < tables.size(); ++i) {
        cout << i+1 << ". " << tables[i] << "\n";
    }
    
    int choice = get_int_input("Ваш выбор: ", 1, tables.size());
    switch(choice) {
        case 1: insert_horse(); break;
        case 2: insert_jockey(); break;
        case 3: insert_race(); break;
    }
}

void AdminController::update() {
    vector<string> tables = {"horses", "jockeys", "races"};
    cout << "Выберите таблицу для обновления:\n";
    for(size_t i = 0; i < tables.size(); ++i) {
        cout << i+1 << ". " << tables[i] << "\n";
    }
    
    int choice = get_int_input("Ваш выбор: ", 1, tables.size());
    int id = get_int_input("Введите ID записи: ");
    
    switch(choice) {
        case 1: update_horse(id); break;
        case 2: update_jockey(id); break;
        case 3: update_race(id); break;
    }
}

void AdminController::remove() {
    vector<string> tables = {"horses", "jockeys", "races"};
    cout << "Выберите таблицу для удаления:\n";
    for(size_t i = 0; i < tables.size(); ++i) {
        cout << i+1 << ". " << tables[i] << "\n";
    }
    
    int choice = get_int_input("Ваш выбор: ", 1, tables.size());
    int id = get_int_input("Введите ID записи: ");
    
    string sql = "DELETE FROM " + tables[choice-1] + " WHERE id = ?;";
    execute_parameterized(sql, {to_string(id)});
    cout << "Запись успешно удалена!\n";
}

void AdminController::select() {
    vector<string> tables = {"horses", "jockeys", "races"};
    cout << "Выберите таблицу для просмотра:\n";
    for(size_t i = 0; i < tables.size(); ++i) {
        cout << i+1 << ". " << tables[i] << "\n";
    }
    
    int choice = get_int_input("Ваш выбор: ", 1, tables.size());
    print_table(tables[choice-1]);
}

void AdminController::generate_reports() {
    vector<string> reports = {
        "Лошадь с максимальными победами",
        "Самый активный жокей",
        "Забеги за период",
        "Статистика владельца",
        "Полная история забегов"
    };
    
    cout << "Доступные отчеты:\n";
    for(size_t i = 0; i < reports.size(); ++i) {
        cout << i+1 << ". " << reports[i] << "\n";
    }
    
    int choice = get_int_input("Выберите отчет: ", 1, reports.size());
    switch(choice) {
        case 1: report_most_winning_horse(); break;
        case 2: report_most_active_jockey(); break;
        case 3: report_races_period(); break;
        case 4: report_owner_stats(); break;
        case 5: report_full_history(); break;
    }
}

// Приватные методы
void AdminController::insert_horse() {
    vector<string> params(5);
    params[0] = get_string_input("Кличка лошади: ");
    params[1] = to_string(get_int_input("Возраст: ", 1, 30));
    params[2] = to_string(get_int_input("Опыт (лет): ", 0, 30));
    params[3] = to_string(get_double_input("Стоимость: "));
    params[4] = get_string_input("Владелец: ");

    string sql = "INSERT INTO horses (name, age, experience, price, owner) "
                 "VALUES (?, ?, ?, ?, ?);";
    execute_parameterized(sql, params);
    cout << "Лошадь успешно добавлена!\n";
}

void AdminController::insert_jockey() {
    vector<string> params(4);
    params[0] = get_string_input("Фамилия жокея: ");
    params[1] = to_string(get_int_input("Стаж (лет): ", 0, 50));
    params[2] = to_string(get_int_input("Год рождения: ", 1950, 2010));
    params[3] = get_string_input("Адрес: ");

    string sql = "INSERT INTO jockeys (surname, experience, birth_year, address) "
                 "VALUES (?, ?, ?, ?);";
    execute_parameterized(sql, params);
    cout << "Жокей успешно добавлен!\n";
}

void AdminController::insert_race() {
    vector<string> params(5);
    params[0] = get_date_input("Дата забега (ГГГГ-ММ-ДД): ");
    params[1] = to_string(get_int_input("Номер забега: "));
    params[2] = to_string(get_int_input("ID лошади: "));
    params[3] = to_string(get_int_input("ID жокея: "));
    params[4] = to_string(get_int_input("Занятое место: ", 1, 100));

    validate_entries(params[2], params[3]);
    
    string sql = "INSERT INTO races (date, race_number, horse_id, jockey_id, position) "
                 "VALUES (?, ?, ?, ?, ?);";
    execute_parameterized(sql, params);
    cout << "Забег успешно добавлен!\n";
}

void AdminController::update_horse(int id) {
    vector<string> params(5);
    params[0] = get_string_input("Новая кличка [" + get_current_value("horses", "name", id) + "]: ");
    params[1] = to_string(get_int_input("Новый возраст [" + get_current_value("horses", "age", id) + "]: ", 1, 30));
    params[2] = to_string(get_int_input("Новый опыт [" + get_current_value("horses", "experience", id) + "]: ", 0, 30));
    params[3] = to_string(get_double_input("Новая стоимость [" + get_current_value("horses", "price", id) + "]: "));
    params[4] = get_string_input("Новый владелец [" + get_current_value("horses", "owner", id) + "]: ");

    string sql = "UPDATE horses SET "
                 "name = ?, age = ?, experience = ?, price = ?, owner = ? "
                 "WHERE id = ?;";
    params.push_back(to_string(id));
    execute_parameterized(sql, params);
    cout << "Данные лошади обновлены!\n";
}

void AdminController::report_most_winning_horse() {
    string sql = 
        "SELECT h.name, h.owner, COUNT(r.id) as wins, "
        "       GROUP_CONCAT(DISTINCT j.surname) as jockeys "
        "FROM horses h "
        "JOIN races r ON h.id = r.horse_id "
        "JOIN jockeys j ON r.jockey_id = j.id "
        "WHERE r.position = 1 "
        "GROUP BY h.id "
        "ORDER BY wins DESC "
        "LIMIT 1;";

    execute(sql, [](sqlite3_stmt* stmt) {
        cout << "Кличка: " << sqlite3_column_text(stmt, 0) << "\n"
             << "Владелец: " << sqlite3_column_text(stmt, 1) << "\n"
             << "Побед: " << sqlite3_column_int(stmt, 2) << "\n"
             << "Жокеи: " << sqlite3_column_text(stmt, 3) << "\n\n";
    });
}

void AdminController::report_most_active_jockey() {
    string sql = 
        "SELECT j.surname, COUNT(r.id) as total_races, "
        "       MIN(r.date) as first_race, MAX(r.date) as last_race "
        "FROM jockeys j "
        "JOIN races r ON j.id = r.jockey_id "
        "GROUP BY j.id "
        "ORDER BY total_races DESC "
        "LIMIT 1;";

    execute(sql, [](sqlite3_stmt* stmt) {
        cout << "Фамилия: " << sqlite3_column_text(stmt, 0) << "\n"
             << "Всего забегов: " << sqlite3_column_int(stmt, 1) << "\n"
             << "Первый забег: " << sqlite3_column_text(stmt, 2) << "\n"
             << "Последний забег: " << sqlite3_column_text(stmt, 3) << "\n\n";
    });
}

// Вспомогательные методы
int AdminController::get_int_input(const string& prompt, int min, int max) {
    int value;
    do {
        cout << prompt;
        cin >> value;
    } while(value < min || value > max);
    return value;
}

string AdminController::get_current_value(const string& table, const string& column, int id) {
    string sql = "SELECT " + column + " FROM " + table + " WHERE id = ?;";
    string result;
    
    execute(sql, [&result](sqlite3_stmt* stmt) {
        result = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
    }, {to_string(id)});
    
    return result;
}

void AdminController::validate_entries(const string& horse_id, const string& jockey_id) {
    string sql = "SELECT COUNT(*) FROM horses WHERE id = ?;";
    execute_parameterized(sql, {horse_id});
    
    sql = "SELECT COUNT(*) FROM jockeys WHERE id = ?;";
    execute_parameterized(sql, {jockey_id});
}