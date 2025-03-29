#ifndef JOCKEY_INTERFACE_H
#define JOCKEY_INTERFACE_H

#include "database.h"

class JockeyInterface : public Database {
private:
    std::string jockey_id;
    
    void view_personal_info();
    void view_horse_info();
    void view_race_history();

public:
    JockeyInterface(sqlite3* db, const std::string& jockey_id);
    
    void insert() override;
    void update() override;
    void remove() override;
    void select() override;
    
    void request_leave();
    void view_schedule();
};

#endif