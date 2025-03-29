// jockey_interface.cpp
#include "jockey_interface.h"
#include <sqlite3.h>
#include <iostream>
#include <iomanip>

using namespace std;

JockeyInterface::JockeyInterface(sqlite3* db, const string& jockey_id) 
    : Database(db), jockey_id(jockey_id) {}

void JockeyInterface::insert() {
    throw runtime_error("Жокей не имеет прав на добавление записей");
}

void JockeyInterface::update() {
    throw runtime_error("Жокей не имеет прав на изменение записей");
}

void JockeyInterface::remove() {
    throw runtime_error("Жокей не имеет прав на удаление записей");
}

void JockeyInterface::select() {
    cout << "1. Просмотреть личные данные\n"
         << "2. История забегов\n"
         << "3. Информация о текущей лошади\n";
    
    int choice;
    cin >> choice;
    
    switch(choice) {
        case 1: view_personal_info(); break;
        case 2: view_race_history(); break;
        case 3: view_current_horse_info(); break;
        default: throw invalid_argument("Неверный выбор");
    }
}

void JockeyInterface::view_personal_info() {
    string sql = 
        "SELECT surname, experience, birth_year, address "
        "FROM jockeys WHERE id = ?;";
    
    cout << "\nЛичная информация:\n";
    cout << setw(15) << "Фамилия" 
         << setw(10) << "Стаж" 
         << setw(12) << "Год рожд." 
         << setw(30) << "Адрес\n";
    
    execute(sql, [](sqlite3_stmt* stmt) {
        cout << setw(15) << sqlite3_column_text(stmt, 0)
             << setw(10) << sqlite3_column_int(stmt, 1)
             << setw(12) << sqlite3_column_int(stmt, 2)
             << setw(30) << sqlite3_column_text(stmt, 3)
             << endl;
    }, {jockey_id});
}

void JockeyInterface::view_race_history() {
    string sql = 
        "SELECT strftime('%Y-%m-%d', r.date) as date, "
        "       h.name as horse, "
        "       r.position, "
        "       r.race_number "
        "FROM races r "
        "JOIN horses h ON r.horse_id = h.id "
        "WHERE jockey_id = ? "
        "ORDER BY r.date DESC;";
    
    cout << "\nИстория забегов:\n";
    cout << setw(12) << "Дата" 
         << setw(20) << "Лошадь" 
         << setw(10) << "Место" 
         << setw(10) << "Забег №\n";
    
    execute(sql, [](sqlite3_stmt* stmt) {
        cout << setw(12) << sqlite3_column_text(stmt, 0)
             << setw(20) << sqlite3_column_text(stmt, 1)
             << setw(10) << sqlite3_column_int(stmt, 2)
             << setw(10) << sqlite3_column_int(stmt, 3)
             << endl;
    }, {jockey_id});
}

void JockeyInterface::view_current_horse_info() {
    string sql = 
        "SELECT h.name, h.age, h.experience, h.owner "
        "FROM horses h "
        "JOIN races r ON h.id = r.horse_id "
        "WHERE r.jockey_id = ? "
        "ORDER BY r.date DESC "
        "LIMIT 1;";
    
    cout << "\nТекущая лошадь:\n";
    
    execute(sql, [](sqlite3_stmt* stmt) {
        if(sqlite3_column_text(stmt, 0)) {
            cout << "Кличка: " << sqlite3_column_text(stmt, 0) << "\n"
                 << "Возраст: " << sqlite3_column_int(stmt, 1) << "\n"
                 << "Опыт: " << sqlite3_column_int(stmt, 2) << " лет\n"
                 << "Владелец: " << sqlite3_column_text(stmt, 3) << endl;
        } else {
            cout << "Нет активных лошадей" << endl;
        }
    }, {jockey_id});
}

void JockeyInterface::request_leave() {
    string sql = 
        "UPDATE jockeys SET status = 'on_leave' "
        "WHERE id = ?;";
    
    execute(sql, {}, {jockey_id});
    cout << "Запрос на отпуск отправлен\n";
}

void JockeyInterface::view_schedule() {
    string sql = 
        "SELECT strftime('%Y-%m-%d', date), race_number "
        "FROM races "
        "WHERE jockey_id = ? AND date > date('now') "
        "ORDER BY date;";
    
    cout << "\nПредстоящие забеги:\n";
    cout << setw(12) << "Дата" << setw(10) << "Забег №\n";
    
    execute(sql, [](sqlite3_stmt* stmt) {
        cout << setw(12) << sqlite3_column_text(stmt, 0)
             << setw(10) << sqlite3_column_int(stmt, 1)
             << endl;
    }, {jockey_id});
}