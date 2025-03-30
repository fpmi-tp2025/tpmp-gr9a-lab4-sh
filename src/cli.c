#include "cli.h"
#include "auth.h"
#include "racecourse.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void show_menu() {
    printf("\n--- Racecourse Management System ---\n");
    printf("1. Register User\n");
    printf("2. Login\n");
    printf("3. Add Horse\n");
    printf("4. Add Jockey\n");
    printf("5. Add Race\n");
    printf("6. Update Horse\n");
    printf("7. Delete Horse\n");
    printf("8. Distribute Prize Fund\n");
    printf("9. Get Jockey Races\n");
    printf("10. Get All Horses\n");
    printf("11. Get All Jockeys\n");
    printf("12. Get All Races\n");
    printf("13. Get All Users\n");
    printf("14. Get All Prize Funds\n");
    printf("15. Exit\n");
    printf("Enter your choice: ");
}

void handle_user_input(sqlite3* db) {
    int choice;
    char username[100], password[100];
    char horse_name[100], owner[100], jockey_name[100], address[255];
    int age, experience, birth_year, race_number, place, horse_id, jockey_id;
    float price;
    char date[11];
    float fund;

    while (1) {
        show_menu();
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Enter username: ");
                scanf("%s", username);
                printf("Enter password: ");
                scanf("%s", password);
                printf("Is admin (1 for yes, 0 for no): ");
                int is_admin;
                scanf("%d", &is_admin);
                register_user(db, username, password, is_admin);
                break;
            case 2:
                printf("Enter username: ");
                scanf("%s", username);
                printf("Enter password: ");
                scanf("%s", password);
                if (login_user(db, username, password)) {
                    printf("Login successful!\n");
                } else {
                    printf("Login failed!\n");
                }
                break;
            case 3:
                printf("Enter horse name: ");
                scanf("%s", horse_name);
                printf("Enter age: ");
                scanf("%d", &age);
                printf("Enter experience: ");
                scanf("%d", &experience);
                printf("Enter owner: ");
                scanf("%s", owner);
                printf("Enter price: ");
                scanf("%f", &price);
                add_horse(db, horse_name, age, experience, owner, price);
                break;
            case 4:
                printf("Enter jockey last name: ");
                scanf("%s", jockey_name);
                printf("Enter experience: ");
                scanf("%d", &experience);
                printf("Enter birth year: ");
                scanf("%d", &birth_year);
                printf("Enter address: ");
                scanf("%s", address);
                add_jockey(db, jockey_name, experience, birth_year, address);
                break;
            case 5:
                printf("Enter date (YYYY-MM-DD): ");
                scanf("%s", date);
                printf("Enter race number: ");
                scanf("%d", &race_number);
                printf("Enter horse ID: ");
                scanf("%d", &horse_id);
                printf("Enter jockey ID: ");
                scanf("%d", &jockey_id);
                printf("Enter place: ");
                scanf("%d", &place);
                add_race(db, date, race_number, horse_id, jockey_id, place);
                break;
            case 6:
                printf("Enter horse ID: ");
                scanf("%d", &horse_id);
                printf("Enter new horse name: ");
                scanf("%s", horse_name);
                printf("Enter new age: ");
                scanf("%d", &age);
                printf("Enter new experience: ");
                scanf("%d", &experience);
                printf("Enter new owner: ");
                scanf("%s", owner);
                printf("Enter new price: ");
                scanf("%f", &price);
                update_horse(db, horse_id, horse_name, age, experience, owner, price);
                break;
            case 7:
                printf("Enter horse ID: ");
                scanf("%d", &horse_id);
                delete_horse(db, horse_id);
                break;
            case 8:
                printf("Enter prize fund amount: ");
                scanf("%f", &fund);
                distribute_prize_fund(db, fund);
                break;
            case 9:
                printf("Enter jockey last name: ");
                scanf("%s", jockey_name);
                get_jockey_races(db, jockey_name);
                break;
            case 10:
                get_all_horses(db);
                break;
            case 11:
                get_all_jockeys(db);
                break;
            case 12:
                get_all_races(db);
                break;
            case 13:
                get_all_users(db);
                break;
            case 14:
                get_all_prize_funds(db);
                break;
            case 15:
                exit(0);
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }
}
