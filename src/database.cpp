// database.cpp
#include "database.h"
#include <sqlite3.h>
#include <stdexcept>
#include <iostream>
#include <vector>

using namespace std;

// Реализация StatementGuard
Database::StatementGuard::StatementGuard(sqlite3* db, const string& sql) {
    if(sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        throw runtime_error(sqlite3_errmsg(db));
    }
}

Database::StatementGuard::~StatementGuard() {
    sqlite3_finalize(stmt);
}

// Реализация методов Database
void Database::execute(const string& sql, 
                      function<int(sqlite3_stmt*)> callback,
                      const vector<string>& params) {
    StatementGuard stmt_guard(db, sql);
    
    // Привязка параметров
    for(size_t i = 0; i < params.size(); ++i) {
        sqlite3_bind_text(stmt_guard, i+1, params[i].c_str(), -1, SQLITE_TRANSIENT);
    }
    
    // Выполнение запроса
    int rc;
    while((rc = sqlite3_step(stmt_guard)) == SQLITE_ROW) {
        if(callback) callback(static_cast<sqlite3_stmt*>(stmt_guard));
    }
    
    if(rc != SQLITE_DONE) {
        throw runtime_error(sqlite3_errmsg(db));
    }
}

void Database::transaction(function<void()> operations) {
    try {
        execute("BEGIN EXCLUSIVE TRANSACTION");
        operations();
        execute("COMMIT");
    } catch(...) {
        execute("ROLLBACK");
        throw;
    }
}

void Database::initialize(sqlite3** db, const string& db_name) {
    // Открытие соединения
    if(sqlite3_open(db_name.c_str(), db) != SQLITE_OK) {
        throw runtime_error(sqlite3_errmsg(*db));
    }
    
    // Создание таблиц
    const char* create_tables = 
        "CREATE TABLE IF NOT EXISTS horses ("
        "   id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "   name TEXT NOT NULL UNIQUE,"
        "   age INTEGER CHECK(age BETWEEN 1 AND 30),"
        "   experience INTEGER CHECK(experience >= 0),"
        "   owner TEXT NOT NULL,"
        "   price REAL CHECK(price >= 0)"
        ");"
        
        "CREATE TABLE IF NOT EXISTS jockeys ("
        "   id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "   surname TEXT NOT NULL UNIQUE,"
        "   experience INTEGER CHECK(experience >= 0),"
        "   birth_year INTEGER CHECK(birth_year BETWEEN 1950 AND 2010),"
        "   address TEXT NOT NULL"
        ");"
        
        "CREATE TABLE IF NOT EXISTS races ("
        "   id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "   date TEXT NOT NULL,"
        "   race_number INTEGER NOT NULL,"
        "   horse_id INTEGER NOT NULL,"
        "   jockey_id INTEGER NOT NULL,"
        "   position INTEGER CHECK(position > 0),"
        "   FOREIGN KEY(horse_id) REFERENCES horses(id) ON DELETE CASCADE,"
        "   FOREIGN KEY(jockey_id) REFERENCES jockeys(id) ON DELETE CASCADE"
        ");"
        
        "CREATE TABLE IF NOT EXISTS prize_funds ("
        "   race_id INTEGER PRIMARY KEY,"
        "   total_amount REAL NOT NULL,"
        "   distribution JSON NOT NULL,"
        "   FOREIGN KEY(race_id) REFERENCES races(id)"
        ");";
    
    char* errMsg = nullptr;
    int rc = sqlite3_exec(*db, create_tables, nullptr, nullptr, &errMsg);
    if(rc != SQLITE_OK) {
        string error(errMsg);
        sqlite3_free(errMsg);
        throw runtime_error(error);
    }
    
    // Создание триггеров
    const char* create_triggers =
        "CREATE TRIGGER IF NOT EXISTS validate_race_insert "
        "BEFORE INSERT ON races "
        "BEGIN "
        "   SELECT RAISE(ABORT, 'Invalid horse_id') "
        "   WHERE NOT EXISTS(SELECT 1 FROM horses WHERE id = NEW.horse_id); "
        "   SELECT RAISE(ABORT, 'Invalid jockey_id') "
        "   WHERE NOT EXISTS(SELECT 1 FROM jockeys WHERE id = NEW.jockey_id); "
        "END;";
    
    rc = sqlite3_exec(*db, create_triggers, nullptr, nullptr, &errMsg);
    if(rc != SQLITE_OK) {
        string error(errMsg);
        sqlite3_free(errMsg);
        throw runtime_error(error);
    }
}

void Database::backup(const string& filename) {
    sqlite3* backup_db;
    if(sqlite3_open(filename.c_str(), &backup_db) != SQLITE_OK) {
        throw runtime_error(sqlite3_errmsg(backup_db));
    }
    
    sqlite3_backup* backup = sqlite3_backup_init(backup_db, "main", db, "main");
    if(backup) {
        sqlite3_backup_step(backup, -1);
        sqlite3_backup_finish(backup);
    }
    sqlite3_close(backup_db);
}

Database::~Database() {
    if(db) {
        sqlite3_close(db);
        db = nullptr;
    }
}