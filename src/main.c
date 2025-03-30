#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "database.h"
#include "auth.h"
#include "racecourse.h"
#include "cli.h"

int main() {
    sqlite3* db = connect_db("racecourse.db");
    initialize_db(db);  

    handle_user_input(db); 

    disconnect_db(db);
    return 0;
}
