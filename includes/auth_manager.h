#ifndef AUTH_MANAGER_H
#define AUTH_MANAGER_H

#include "database.h"
#include <optional>

class AuthManager : public Database {
private:
    struct UserCredentials {
        std::string username;
        std::string password_hash;
        int user_type;
    };

    std::optional<UserCredentials> authenticate(const std::string& username, 
                                               const std::string& password);

public:
    AuthManager(sqlite3* db);
    
    bool login(const std::string& username, const std::string& password);
    bool signup(const std::string& username, 
               const std::string& password, 
               int user_type);
    
    void reset_password(const std::string& username);
};

#endif