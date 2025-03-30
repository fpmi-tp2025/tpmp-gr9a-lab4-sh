#include "auth.h"
#include "database.h"
#include <stdio.h>
#include <string.h>

bool register_user(sqlite3* db, const char* username, const char* password, const char* role,
    int jockey_id, const char* owner_name)
{
    // Если роль = "jockey", проставим jockey_id (но owner_name может быть NULL или пустая строка)
    // Если роль = "owner", проставим owner_name (jockey_id = 0)

    char query[512];
    snprintf(query, sizeof(query),
        "INSERT INTO Users (username, password, role, jockey_id, owner_name) "
        "VALUES ('%s', '%s', '%s', %d, '%s');",
        username, password, role,
        (role && strcmp(role, "jockey") == 0) ? jockey_id : 0,
        (role && strcmp(role, "owner") == 0) ? owner_name : ""
    );
    execute_query(db, query);
    return true;
}


bool login_user(sqlite3* db, const char* username, const char* password, char* role) {
    char query[256];
    snprintf(query, sizeof(query), "SELECT role FROM Users WHERE username = '%s' AND password = '%s';", username, password);
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
        return false;
    }
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        strcpy(role, (const char*)sqlite3_column_text(stmt, 0));
        sqlite3_finalize(stmt);
        return true;
    }
    sqlite3_finalize(stmt);
    return false;
}

bool is_admin(sqlite3* db, const char* username) {
    char query[256];
    snprintf(query, sizeof(query), "SELECT role FROM Users WHERE username = '%s';", username);
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
        return false;
    }
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const char* role = (const char*)sqlite3_column_text(stmt, 0);
        sqlite3_finalize(stmt);
        return strcmp(role, "admin") == 0;
    }
    sqlite3_finalize(stmt);
    return false;
}

bool is_owner(sqlite3* db, const char* username) {
    char query[256];
    snprintf(query, sizeof(query), "SELECT role FROM Users WHERE username = '%s';", username);
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
        return false;
    }
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const char* role = (const char*)sqlite3_column_text(stmt, 0);
        sqlite3_finalize(stmt);
        return strcmp(role, "owner") == 0;
    }
    sqlite3_finalize(stmt);
    return false;
}

bool is_jockey(sqlite3* db, const char* username) {
    char query[256];
    snprintf(query, sizeof(query), "SELECT role FROM Users WHERE username = '%s';", username);
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
        return false;
    }
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const char* role = (const char*)sqlite3_column_text(stmt, 0);
        sqlite3_finalize(stmt);
        return strcmp(role, "jockey") == 0;
    }
    sqlite3_finalize(stmt);
    return false;
}
