#ifndef OWNER_INTERFACE_H
#define OWNER_INTERFACE_H

#include "database.h"

class OwnerInterface : public Database {
private:
    std::string owner_id;
    
    void view_horses();
    void view_race_results();
    void financial_statements();

public:
    OwnerInterface(sqlite3* db, const std::string& owner_id);
    
    void insert() override;
    void update() override;
    void remove() override;
    void select() override;
    
    void register_new_horse();
    void transfer_ownership();
};

#endif