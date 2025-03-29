// main.cpp
#include "database.h"
#include "auth_manager.h"
#include "admin_controller.h"
#include "jockey_interface.h"
#include "owner_interface.h"
#include <sqlite3.h>
#include <iostream>
#include <memory>

using namespace std;

void show_main_menu() {
    cout << "\nГлавное меню:\n"
         << "1. Вход\n"
         << "2. Регистрация\n"
         << "3. Выход\n"
         << "Выберите действие: ";
}

void admin_menu(AdminController& admin) {
    while(true) {
        cout << "\nАдминистративное меню:\n"
             << "1. Добавить запись\n"
             << "2. Обновить запись\n"
             << "3. Удалить запись\n"
             << "4. Просмотреть записи\n"
             << "5. Создать отчет\n"
             << "6. Распределить призы\n"
             << "7. Выход\n"
             << "Выберите действие: ";
        
        int choice;
        cin >> choice;
        
        try {
            switch(choice) {
                case 1: admin.insert(); break;
                case 2: admin.update(); break;
                case 3: admin.remove(); break;
                case 4: admin.select(); break;
                case 5: admin.generate_reports(); break;
                case 6: {
                    double fund;
                    cout << "Введите размер призового фонда: ";
                    cin >> fund;
                    admin.distribute_prizes(fund);
                    break;
                }
                case 7: return;
                default: throw invalid_argument("Неверный выбор");
            }
        } catch(const exception& e) {
            cerr << "Ошибка: " << e.what() << endl;
        }
    }
}

int main() {
    sqlite3* db = nullptr;
    try {
        Database::initialize(&db, "hippodrome.db");
        
        AuthManager auth(db);
        int user_type = 0;
        string username;

        while(true) {
            show_main_menu();
            int choice;
            cin >> choice;

            if(choice == 3) break;

            string password;
            cout << "Логин: ";
            cin >> username;
            cout << "Пароль: ";
            cin >> password;

            if(choice == 1) {
                if(auth.login(username, password)) {
                    user_type = auth.getUserType();
                    break;
                }
            } else if(choice == 2) {
                cout << "Выберите тип пользователя (1-Владелец, 2-Жокей, 3-Админ): ";
                int type;
                cin >> type;
                auth.signup(username, password, type);
            }
        }

        // Создание соответствующего интерфейса
        switch(user_type) {
            case Database::ADMIN: {
                AdminController admin(db);
                admin_menu(admin);
                break;
            }
            case Database::JOCKEY: {
                JockeyInterface jockey(db, username);
                jockey.select();
                break;
            }
            case Database::OWNER: {
                OwnerInterface owner(db, username);
                owner.select();
                break;
            }
            default:
                cerr << "Неизвестный тип пользователя" << endl;
        }

    } catch(const exception& e) {
        cerr << "Критическая ошибка: " << e.what() << endl;
        return 1;
    }

    sqlite3_close(db);
    cout << "Работа программы завершена" << endl;
    return 0;
}