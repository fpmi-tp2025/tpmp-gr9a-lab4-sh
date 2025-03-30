#include "database.h"
#include <stdio.h>
#include <stdlib.h>

sqlite3* connect_db(const char* db_name) {
    sqlite3* db;
    if (sqlite3_open(db_name, &db) != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
    return db;
}

void disconnect_db(sqlite3* db) {
    sqlite3_close(db);
}

void execute_query(sqlite3* db, const char* query) {
    char* err_msg = 0;
    if (sqlite3_exec(db, query, 0, 0, &err_msg) != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
    }
}

void initialize_db(sqlite3* db) {
    const char* create_tables_sql =
        "CREATE TABLE IF NOT EXISTS Horses ("
        "id integer NOT NULL CONSTRAINT id PRIMARY KEY AUTOINCREMENT,"
        "name varchar(100) NOT NULL,"
        "age integer NOT NULL,"
        "experience integer NOT NULL,"
        "owner varchar(100) NOT NULL,"
        "price decimal(10,2) NOT NULL);"

        "CREATE TABLE IF NOT EXISTS Jockeys ("
        "id integer NOT NULL CONSTRAINT Jockeys_pk PRIMARY KEY AUTOINCREMENT,"
        "last_name varchar(100) NOT NULL,"
        "experience integer NOT NULL,"
        "birth_year integer NOT NULL,"
        "address varchar(255) NOT NULL);"

        "CREATE TABLE IF NOT EXISTS Races ("
        "id integer NOT NULL CONSTRAINT Races_pk PRIMARY KEY AUTOINCREMENT,"
        "date date NOT NULL,"
        "race_number integer NOT NULL,"
        "horse_id integer NOT NULL,"
        "jockey_id integer NOT NULL,"
        "place integer NOT NULL,"
        "jockeys_id integer NOT NULL,"
        "horses_id integer NOT NULL,"
        "CONSTRAINT Races_Jockeys FOREIGN KEY (jockeys_id) REFERENCES Jockeys (id),"
        "CONSTRAINT Races_Horses FOREIGN KEY (horses_id) REFERENCES Horses (id));"

        "CREATE TABLE IF NOT EXISTS Users ("
        "id integer NOT NULL CONSTRAINT Users_pk PRIMARY KEY AUTOINCREMENT,"
        "username varchar(100) NOT NULL UNIQUE,"
        "password varchar(100) NOT NULL,"
        "is_admin boolean NOT NULL);"

        "CREATE TABLE IF NOT EXISTS PrizeFund ("
        "id integer NOT NULL CONSTRAINT PrizeFund_pk PRIMARY KEY AUTOINCREMENT,"
        "horse_id integer NOT NULL,"
        "amount decimal(10,2) NOT NULL,"
        "CONSTRAINT PrizeFund_Horses FOREIGN KEY (horse_id) REFERENCES Horses (id));";

    execute_query(db, create_tables_sql);
}
