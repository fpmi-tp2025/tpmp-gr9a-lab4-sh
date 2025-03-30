#include "racecourse.h"
#include "database.h"
#include <stdio.h>
#include <stdlib.h>

void add_horse(sqlite3* db, const char* name, int age, int experience, const char* owner, float price) {
    char query[256];
    snprintf(query, sizeof(query), "INSERT INTO Horses (name, age, experience, owner, price) VALUES ('%s', %d, %d, '%s', %.2f);", name, age, experience, owner, price);
    execute_query(db, query);
}

void add_jockey(sqlite3* db, const char* last_name, int experience, int birth_year, const char* address) {
    char query[256];
    snprintf(query, sizeof(query), "INSERT INTO Jockeys (last_name, experience, birth_year, address) VALUES ('%s', %d, %d, '%s');", last_name, experience, birth_year, address);
    execute_query(db, query);
}

void add_race(sqlite3* db, const char* date, int race_number, int horse_id, int jockey_id, int place) {
    char query[256];
    snprintf(query, sizeof(query), "INSERT INTO Races (date, race_number, horse_id, jockey_id, place, jockeys_id, horses_id) VALUES ('%s', %d, %d, %d, %d, %d, %d);", date, race_number, horse_id, jockey_id, place, jockey_id, horse_id);
    execute_query(db, query);
}

void update_horse(sqlite3* db, int id, const char* name, int age, int experience, const char* owner, float price) {
    char query[256];
    snprintf(query, sizeof(query), "UPDATE Horses SET name = '%s', age = %d, experience = %d, owner = '%s', price = %.2f WHERE id = %d;", name, age, experience, owner, price, id);
    execute_query(db, query);
}

void delete_horse(sqlite3* db, int id) {
    char query[256];
    snprintf(query, sizeof(query), "DELETE FROM Horses WHERE id = %d;", id);
    execute_query(db, query);
}

void distribute_prize_fund(sqlite3* db, float fund) {
    char query[256];
    snprintf(query, sizeof(query), "SELECT horse_id, place FROM Races ORDER BY date DESC, race_number DESC LIMIT 3;");
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
        return;
    }
    float prizes[3] = {fund * 0.5, fund * 0.3, fund * 0.2};
    int i = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int horse_id = sqlite3_column_int(stmt, 0);
        int place = sqlite3_column_int(stmt, 1);
        char prize_query[256];
        snprintf(prize_query, sizeof(prize_query), "INSERT INTO PrizeFund (horse_id, amount) VALUES (%d, %.2f);", horse_id, prizes[place - 1]);
        execute_query(db, prize_query);
        i++;
    }
    sqlite3_finalize(stmt);
}

void get_jockey_races(sqlite3* db, const char* jockey_name) {
    char query[256];
    snprintf(query, sizeof(query), "SELECT date, horse_id, place FROM Races JOIN Jockeys ON Races.jockey_id = Jockeys.id WHERE Jockeys.last_name = '%s';", jockey_name);
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
        return;
    }
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char* date = (const char*)sqlite3_column_text(stmt, 0);
        int horse_id = sqlite3_column_int(stmt, 1);
        int place = sqlite3_column_int(stmt, 2);
        printf("Date: %s, Horse ID: %d, Place: %d\n", date, horse_id, place);
    }
    sqlite3_finalize(stmt);
}

void get_all_horses(sqlite3* db) {
    char query[] = "SELECT * FROM Horses;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
        return;
    }
    printf("Horses:\n");
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char* name = (const char*)sqlite3_column_text(stmt, 1);
        int age = sqlite3_column_int(stmt, 2);
        int experience = sqlite3_column_int(stmt, 3);
        const char* owner = (const char*)sqlite3_column_text(stmt, 4);
        float price = sqlite3_column_double(stmt, 5);
        printf("ID: %d, Name: %s, Age: %d, Experience: %d, Owner: %s, Price: %.2f\n", id, name, age, experience, owner, price);
    }
    sqlite3_finalize(stmt);
}

void get_all_jockeys(sqlite3* db) {
    char query[] = "SELECT * FROM Jockeys;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
        return;
    }
    printf("Jockeys:\n");
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char* last_name = (const char*)sqlite3_column_text(stmt, 1);
        int experience = sqlite3_column_int(stmt, 2);
        int birth_year = sqlite3_column_int(stmt, 3);
        const char* address = (const char*)sqlite3_column_text(stmt, 4);
        printf("ID: %d, Last Name: %s, Experience: %d, Birth Year: %d, Address: %s\n", id, last_name, experience, birth_year, address);
    }
    sqlite3_finalize(stmt);
}

void get_all_races(sqlite3* db) {
    char query[] = "SELECT * FROM Races;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
        return;
    }
    printf("Races:\n");
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char* date = (const char*)sqlite3_column_text(stmt, 1);
        int race_number = sqlite3_column_int(stmt, 2);
        int horse_id = sqlite3_column_int(stmt, 3);
        int jockey_id = sqlite3_column_int(stmt, 4);
        int place = sqlite3_column_int(stmt, 5);
        printf("ID: %d, Date: %s, Race Number: %d, Horse ID: %d, Jockey ID: %d, Place: %d\n", id, date, race_number, horse_id, jockey_id, place);
    }
    sqlite3_finalize(stmt);
}

void get_all_users(sqlite3* db) {
    char query[] = "SELECT * FROM Users;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
        return;
    }
    printf("Users:\n");
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char* username = (const char*)sqlite3_column_text(stmt, 1);
        const char* password = (const char*)sqlite3_column_text(stmt, 2);
        int is_admin = sqlite3_column_int(stmt, 3);
        printf("ID: %d, Username: %s, Password: %s, Is Admin: %d\n", id, username, password, is_admin);
    }
    sqlite3_finalize(stmt);
}

void get_all_prize_funds(sqlite3* db) {
    char query[] = "SELECT * FROM PrizeFund;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
        return;
    }
    printf("Prize Funds:\n");
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        int horse_id = sqlite3_column_int(stmt, 1);
        float amount = sqlite3_column_double(stmt, 2);
        printf("ID: %d, Horse ID: %d, Amount: %.2f\n", id, horse_id, amount);
    }
    sqlite3_finalize(stmt);
}
