#ifndef AUTH_H
#define AUTH_H

#include <sqlite3.h>
#include <stdbool.h>

bool register_user(sqlite3* db, const char* username, const char* password, bool is_admin);
bool login_user(sqlite3* db, const char* username, const char* password);
bool is_admin(sqlite3* db, const char* username);

#endif // AUTH_H
