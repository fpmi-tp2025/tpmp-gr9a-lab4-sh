#ifndef ADMIN_CONTROLLER_H
#define ADMIN_CONTROLLER_H

#include "database.h"

class AdminController : public Database {
private:
    void manage_horses();
    void manage_jockeys();
    void manage_races();
    void manage_owners();
    
public:
    AdminController(sqlite3* db);
    
    void insert() override;
    void update() override;
    void remove() override;
    void select() override;
    
    void generate_reports();
    void validate_data_integrity();
    void manage_backups();
};

#endif