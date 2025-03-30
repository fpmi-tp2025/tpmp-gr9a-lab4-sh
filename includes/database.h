#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>

sqlite3* connect_db(const char* db_name);
void disconnect_db(sqlite3* db);
void execute_query(sqlite3* db, const char* query);
void initialize_db(sqlite3* db); 

#endif // DATABASE_H
