#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include <string>
#include <functional>
#include <vector>

class Database {
protected:
    sqlite3* db;
    int user_type;
    std::string current_user;
    
    struct StatementGuard {
        sqlite3_stmt* stmt;
        StatementGuard(sqlite3* db, const std::string& sql);
        ~StatementGuard();
        operator sqlite3_stmt*() { return stmt; }
    };

    void execute(const std::string& sql, 
                std::function<int(sqlite3_stmt*)> callback = nullptr,
                const std::vector<std::string>& params = {});

public:
    enum UserType { ADMIN, JOCKEY, OWNER, GUEST };
    
    Database(sqlite3* db);
    virtual ~Database();

    virtual void insert() = 0;
    virtual void update() = 0;
    virtual void remove() = 0;
    virtual void select() = 0;
    
    void transaction(std::function<void()> operations);
    void backup(const std::string& filename);
    
    static void initialize(sqlite3** db, const std::string& db_name = "hippodrome.db");
};

#endif