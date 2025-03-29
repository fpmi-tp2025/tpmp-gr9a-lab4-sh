// owner_interface.cpp
#include "owner_interface.h"
#include <sqlite3.h>
#include <iostream>
#include <iomanip>
#include <stdexcept>

using namespace std;

OwnerInterface::OwnerInterface(sqlite3* db, const string& owner_id) 
    : Database(db), owner_id(owner_id) {}

void OwnerInterface::insert() {
    throw runtime_error("Владелец не имеет прав на добавление записей");
}

void OwnerInterface::update() {
    throw runtime_error("Владелец не имеет прав на изменение записей");
}

void OwnerInterface::remove() {
    throw runtime_error("Владелец не имеет прав на удаление записей");
}

void OwnerInterface::select() {
    cout << "1. Список моих лошадей\n"
         << "2. Результаты забегов\n"
         << "3. Финансовая отчетность\n";
    
    int choice;
    cin >> choice;
    
    switch(choice) {
        case 1: view_horses(); break;
        case 2: view_race_results(); break;
        case 3: financial_statements(); break;
        default: throw invalid_argument("Неверный выбор");
    }
}

void OwnerInterface::view_horses() {
    string sql = 
        "SELECT name, age, experience, price "
        "FROM horses "
        "WHERE owner = ? "
        "ORDER BY name;";
    
    cout << "\nСписок лошадей владельца " << owner_id << ":\n";
    cout << setw(20) << "Кличка" 
         << setw(8) << "Возраст" 
         << setw(10) << "Опыт" 
         << setw(12) << "Стоимость\n";
    
    execute(sql, [](sqlite3_stmt* stmt) {
        cout << setw(20) << sqlite3_column_text(stmt, 0)
             << setw(8) << sqlite3_column_int(stmt, 1)
             << setw(10) << sqlite3_column_int(stmt, 2)
             << setw(12) << fixed << setprecision(2) << sqlite3_column_double(stmt, 3)
             << endl;
    }, {owner_id});
}

void OwnerInterface::view_race_results() {
    string sql = 
        "SELECT h.name, strftime('%Y-%m-%d', r.date) as date, "
        "       r.position, r.race_number, j.surname "
        "FROM horses h "
        "JOIN races r ON h.id = r.horse_id "
        "JOIN jockeys j ON r.jockey_id = j.id "
        "WHERE h.owner = ? "
        "ORDER BY r.date DESC;";
    
    cout << "\nРезультаты забегов для владельца " << owner_id << ":\n";
    cout << setw(20) << "Лошадь" 
         << setw(12) << "Дата" 
         << setw(8) << "Место" 
         << setw(10) << "Забег №" 
         << setw(20) << "Жокей\n";
    
    execute(sql, [](sqlite3_stmt* stmt) {
        cout << setw(20) << sqlite3_column_text(stmt, 0)
             << setw(12) << sqlite3_column_text(stmt, 1)
             << setw(8) << sqlite3_column_int(stmt, 2)
             << setw(10) << sqlite3_column_int(stmt, 3)
             << setw(20) << sqlite3_column_text(stmt, 4)
             << endl;
    }, {owner_id});
}

void OwnerInterface::financial_statements() {
    string sql = 
        "SELECT pf.race_id, pf.total_amount, "
        "       json_extract(pf.distribution, '$.1st') as first_prize, "
        "       r.date "
        "FROM prize_funds pf "
        "JOIN races r ON pf.race_id = r.id "
        "JOIN horses h ON r.horse_id = h.id "
        "WHERE h.owner = ? "
        "ORDER BY r.date DESC;";
    
    cout << "\nФинансовая отчетность владельца " << owner_id << ":\n";
    cout << setw(10) << "Забег" 
         << setw(12) << "Дата" 
         << setw(15) << "Общий фонд" 
         << setw(15) << "Выигрыш 1-го\n";
    
    execute(sql, [](sqlite3_stmt* stmt) {
        cout << setw(10) << sqlite3_column_int(stmt, 0)
             << setw(12) << sqlite3_column_text(stmt, 3)
             << setw(15) << fixed << setprecision(2) << sqlite3_column_double(stmt, 1)
             << setw(15) << sqlite3_column_double(stmt, 2)
             << endl;
    }, {owner_id});
}

void OwnerInterface::register_new_horse() {
    string name = get_input("Кличка лошади: ");
    int age = get_int_input("Возраст: ", 1, 30);
    int experience = get_int_input("Опыт (лет): ", 0, age);
    double price = get_double_input("Стоимость: ");
    
    string sql = 
        "INSERT INTO horses (name, age, experience, price, owner) "
        "VALUES (?, ?, ?, ?, ?);";
    
    execute_parameterized(sql, {name, to_string(age), 
        to_string(experience), to_string(price), owner_id});
    cout << "Лошадь успешно зарегистрирована!\n";
}

void OwnerInterface::transfer_ownership(const string& new_owner) {
    string sql = 
        "UPDATE horses SET owner = ? "
        "WHERE id = ? AND owner = ?;";
    
    int horse_id = get_int_input("Введите ID лошади: ");
    int affected = execute_parameterized(sql, {new_owner, 
        to_string(horse_id), owner_id});
    
    if(affected == 0) {
        throw runtime_error("Лошадь не найдена или нет прав на передачу");
    }
    cout << "Право собственности успешно передано!\n";
}

// Вспомогательные методы
string OwnerInterface::get_input(const string& prompt) {
    string input;
    cout << prompt;
    cin >> ws;
    getline(cin, input);
    return input;
}

int OwnerInterface::get_int_input(const string& prompt, int min, int max) {
    int value;
    do {
        cout << prompt;
        cin >> value;
    } while(value < min || value > max);
    return value;
}

double OwnerInterface::get_double_input(const string& prompt) {
    double value;
    cout << prompt;
    cin >> value;
    return value;
}