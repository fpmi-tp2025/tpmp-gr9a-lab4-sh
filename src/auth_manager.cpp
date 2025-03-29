// auth_manager.cpp
#include "auth_manager.h"
#include <sqlite3.h>
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>

using namespace std;

AuthManager::AuthManager(sqlite3* db) : Database(db) {}

bool AuthManager::login(const string& username, const string& password) {
    string hashed_password = hash_password(password);
    
    string sql = 
        "SELECT user_type, password_hash FROM users "
        "WHERE username = ?;";
    
    bool auth_success = false;
    
    try {
        execute(sql, [&](sqlite3_stmt* stmt) {
            string stored_hash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            
            if(hashed_password == stored_hash) {
                this->user_type = sqlite3_column_int(stmt, 0);
                this->current_user = username;
                auth_success = true;
            }
        }, {username});
    } catch(const exception& e) {
        cerr << "Ошибка аутентификации: " << e.what() << endl;
    }
    
    if(!auth_success) {
        cerr << "Неверное имя пользователя или пароль" << endl;
    }
    
    return auth_success;
}

bool AuthManager::signup(const string& username, 
                       const string& password,
                       int user_type) {
    if(user_type < 1 || user_type > 3) {
        throw invalid_argument("Некорректный тип пользователя");
    }
    
    if(username_exists(username)) {
        cerr << "Имя пользователя уже занято" << endl;
        return false;
    }
    
    string hashed_password = hash_password(password);
    string sql = 
        "INSERT INTO users (username, password_hash, user_type) "
        "VALUES (?, ?, ?);";
    
    try {
        execute_parameterized(sql, {username, hashed_password, to_string(user_type)});
        cout << "Пользователь успешно зарегистрирован" << endl;
        return true;
    } catch(const exception& e) {
        cerr << "Ошибка регистрации: " << e.what() << endl;
        return false;
    }
}

void AuthManager::reset_password(const string& username) {
    if(!username_exists(username)) {
        throw invalid_argument("Пользователь не найден");
    }
    
    string new_password = generate_temp_password();
    string hashed_password = hash_password(new_password);
    
    string sql = 
        "UPDATE users SET password_hash = ? "
        "WHERE username = ?;";
    
    try {
        execute_parameterized(sql, {hashed_password, username});
        cout << "Временный пароль: " << new_password << endl;
        cout << "Рекомендуем сразу изменить его в личном кабинете" << endl;
    } catch(const exception& e) {
        cerr << "Ошибка сброса пароля: " << e.what() << endl;
        throw;
    }
}

// Приватные методы
string AuthManager::hash_password(const string& password) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, password.c_str(), password.size());
    SHA256_Final(hash, &sha256);
    
    stringstream ss;
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << hex << setw(2) << setfill('0') << (int)hash[i];
    }
    return ss.str();
}

bool AuthManager::username_exists(const string& username) {
    string sql = "SELECT COUNT(*) FROM users WHERE username = ?;";
    int count = 0;
    
    execute(sql, [&count](sqlite3_stmt* stmt) {
        count = sqlite3_column_int(stmt, 0);
    }, {username});
    
    return count > 0;
}

string AuthManager::generate_temp_password() {
    const string chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    const int length = 12;
    string password;
    
    for(int i = 0; i < length; ++i) {
        password += chars[rand() % chars.size()];
    }
    
    return password;
}