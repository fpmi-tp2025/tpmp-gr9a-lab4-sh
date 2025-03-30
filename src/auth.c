#include "auth.h"
#include "database.h"  
#include <stdio.h>
#include <string.h>

bool register_user(sqlite3* db, const char* username, const char* password, bool is_admin) {
    char query[256];
    snprintf(query, sizeof(query), "INSERT INTO Users (username, password, is_admin) VALUES ('%s', '%s', %d);", username, password, is_admin);
    execute_query(db, query);
    return true;
}

bool login_user(sqlite3* db, const char* username, const char* password) {
    char query[256];
    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM Users WHERE username = '%s' AND password = '%s';", username, password);
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
        return false;
    }
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int count = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);
        return count == 1;
    }
    sqlite3_finalize(stmt);
    return false;
}

bool is_admin(sqlite3* db, const char* username) {
    char query[256];
    snprintf(query, sizeof(query), "SELECT is_admin FROM Users WHERE username = '%s';", username);
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
        return false;
    }
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int is_admin = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);
        return is_admin == 1;
    }
    sqlite3_finalize(stmt);
    return false;
}
