#ifndef CLI_H
#define CLI_H

#include <sqlite3.h>

void show_menu();
void handle_user_input(sqlite3* db);

#endif // CLI_H
