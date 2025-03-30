#include "racecourse.h"
#include "database.h"
#include <stdio.h>
#include <stdlib.h>

void add_horse(sqlite3* db, const char* name, int age, int experience, const char* owner, float price) {
    char query[256];
    snprintf(query, sizeof(query), "INSERT INTO Horses (name, age, experience, owner, price) VALUES ('%s', %d, %d, '%s', %.2f);", name, age, experience, owner, price);
    execute_query(db, query);
}

void add_jockey(sqlite3* db, const char* last_name, int experience, int birth_year, const char* address) {
    char query[256];
    snprintf(query, sizeof(query), "INSERT INTO Jockeys (last_name, experience, birth_year, address) VALUES ('%s', %d, %d, '%s');", last_name, experience, birth_year, address);
    execute_query(db, query);
}

void add_race(sqlite3* db, const char* date, int race_number, int horse_id, int jockey_id, int place) {
    char query[256];
    snprintf(query, sizeof(query), "INSERT INTO Races (date, race_number, horse_id, jockey_id, place) VALUES ('%s', %d, %d, %d, %d);", date, race_number, horse_id, jockey_id, place);
    execute_query(db, query);
}

void update_horse(sqlite3* db, int id, const char* name, int age, int experience, const char* owner, float price) {
    char query[256];
    snprintf(query, sizeof(query), "UPDATE Horses SET name = '%s', age = %d, experience = %d, owner = '%s', price = %.2f WHERE id = %d;", name, age, experience, owner, price, id);
    execute_query(db, query);
}

void delete_horse(sqlite3* db, int id) {
    char query[256];
    snprintf(query, sizeof(query), "DELETE FROM Horses WHERE id = %d;", id);
    execute_query(db, query);
}

void distribute_prize_fund(sqlite3* db, float fund) {
    // «апрос на получение horse_id и place именно из последнего забега
    const char* query =
        "WITH LastRace AS ("
        "   SELECT date, race_number "
        "   FROM Races "
        "   ORDER BY date DESC, race_number DESC "
        "   LIMIT 1"
        ") "
        "SELECT R.horse_id, R.place "
        "FROM Races R "
        "JOIN LastRace LR ON R.date = LR.date AND R.race_number = LR.race_number "
        "WHERE R.place <= 3 "       // берЄм только 1,2,3 места
        "ORDER BY R.place ASC;";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
        return;
    }

    float prizes[3] = { fund * 0.5f, fund * 0.3f, fund * 0.2f };
    int counter = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW && counter < 3) {
        int horse_id = sqlite3_column_int(stmt, 0);
        int place = sqlite3_column_int(stmt, 1);

        // place - 1 даст индекс 0..2
        if (place >= 1 && place <= 3) {
            float prize = prizes[place - 1];
            char prize_query[256];
            snprintf(prize_query, sizeof(prize_query),
                "INSERT INTO PrizeFund (horse_id, amount) VALUES (%d, %.2f);",
                horse_id, prize);
            execute_query(db, prize_query);
            counter++;
        }
    }
    sqlite3_finalize(stmt);
}
sqlite3_finalize(stmt);
}

void get_jockey_races(sqlite3* db, const char* jockey_name) {
    char query[256];
    snprintf(query, sizeof(query), "SELECT date, horse_id, place FROM Races JOIN Jockeys ON Races.jockey_id = Jockeys.id WHERE Jockeys.last_name = '%s';", jockey_name);
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
        return;
    }
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char* date = (const char*)sqlite3_column_text(stmt, 0);
        int horse_id = sqlite3_column_int(stmt, 1);
        int place = sqlite3_column_int(stmt, 2);
        printf("Date: %s, Horse ID: %d, Place: %d\n", date, horse_id, place);
    }
    sqlite3_finalize(stmt);
}

void get_all_horses(sqlite3* db) {
    char query[] = "SELECT * FROM Horses;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
        return;
    }
    printf("Horses:\n");
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char* name = (const char*)sqlite3_column_text(stmt, 1);
        int age = sqlite3_column_int(stmt, 2);
        int experience = sqlite3_column_int(stmt, 3);
        const char* owner = (const char*)sqlite3_column_text(stmt, 4);
        float price = sqlite3_column_double(stmt, 5);
        printf("ID: %d, Name: %s, Age: %d, Experience: %d, Owner: %s, Price: %.2f\n", id, name, age, experience, owner, price);
    }
    sqlite3_finalize(stmt);
}

void get_all_jockeys(sqlite3* db) {
    char query[] = "SELECT * FROM Jockeys;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
        return;
    }
    printf("Jockeys:\n");
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char* last_name = (const char*)sqlite3_column_text(stmt, 1);
        int experience = sqlite3_column_int(stmt, 2);
        int birth_year = sqlite3_column_int(stmt, 3);
        const char* address = (const char*)sqlite3_column_text(stmt, 4);
        printf("ID: %d, Last Name: %s, Experience: %d, Birth Year: %d, Address: %s\n", id, last_name, experience, birth_year, address);
    }
    sqlite3_finalize(stmt);
}

void get_all_races(sqlite3* db) {
    char query[] = "SELECT * FROM Races;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
        return;
    }
    printf("Races:\n");
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char* date = (const char*)sqlite3_column_text(stmt, 1);
        int race_number = sqlite3_column_int(stmt, 2);
        int horse_id = sqlite3_column_int(stmt, 3);
        int jockey_id = sqlite3_column_int(stmt, 4);
        int place = sqlite3_column_int(stmt, 5);
        printf("ID: %d, Date: %s, Race Number: %d, Horse ID: %d, Jockey ID: %d, Place: %d\n", id, date, race_number, horse_id, jockey_id, place);
    }
    sqlite3_finalize(stmt);
}

void get_all_users(sqlite3* db) {
    char query[] = "SELECT * FROM Users;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
        return;
    }
    printf("Users:\n");
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char* username = (const char*)sqlite3_column_text(stmt, 1);
        const char* password = (const char*)sqlite3_column_text(stmt, 2);
        const char* role = (const char*)sqlite3_column_text(stmt, 3);
        printf("ID: %d, Username: %s, Password: %s, Role: %s\n", id, username, password, role);
    }
    sqlite3_finalize(stmt);
}

void get_all_prize_funds(sqlite3* db) {
    char query[] = "SELECT * FROM PrizeFund;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
        return;
    }
    printf("Prize Funds:\n");
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        int horse_id = sqlite3_column_int(stmt, 1);
        float amount = sqlite3_column_double(stmt, 2);
        printf("ID: %d, Horse ID: %d, Amount: %.2f\n", id, horse_id, amount);
    }
    sqlite3_finalize(stmt);
}

void get_most_winning_horse(sqlite3* db) {
    char query[] = "SELECT Horses.*, COUNT(Races.place) AS wins FROM Horses JOIN Races ON Horses.id = Races.horse_id WHERE Races.place = 1 GROUP BY Horses.id ORDER BY wins DESC LIMIT 1;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
        return;
    }
    printf("Most Winning Horse:\n");
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char* name = (const char*)sqlite3_column_text(stmt, 1);
        int age = sqlite3_column_int(stmt, 2);
        int experience = sqlite3_column_int(stmt, 3);
        const char* owner = (const char*)sqlite3_column_text(stmt, 4);
        float price = sqlite3_column_double(stmt, 5);
        int wins = sqlite3_column_int(stmt, 6);
        printf("ID: %d, Name: %s, Age: %d, Experience: %d, Owner: %s, Price: %.2f, Wins: %d\n", id, name, age, experience, owner, price, wins);
    }
    sqlite3_finalize(stmt);
}

void get_most_active_jockey(sqlite3* db) {
    char query[] = "SELECT Jockeys.*, COUNT(Races.id) AS total_races FROM Jockeys JOIN Races ON Jockeys.id = Races.jockey_id GROUP BY Jockeys.id ORDER BY total_races DESC LIMIT 1;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
        return;
    }
    printf("Most Active Jockey:\n");
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char* last_name = (const char*)sqlite3_column_text(stmt, 1);
        int experience = sqlite3_column_int(stmt, 2);
        int birth_year = sqlite3_column_int(stmt, 3);
        const char* address = (const char*)sqlite3_column_text(stmt, 4);
        int total_races = sqlite3_column_int(stmt, 5);
        printf("ID: %d, Last Name: %s, Experience: %d, Birth Year: %d, Address: %s, Total Races: %d\n", id, last_name, experience, birth_year, address, total_races);
    }
    sqlite3_finalize(stmt);
}

void get_jockey_races_by_name(sqlite3* db, const char* jockey_name) {
    char query[256];
    snprintf(query, sizeof(query), "SELECT date, horse_id, place FROM Races JOIN Jockeys ON Races.jockey_id = Jockeys.id WHERE Jockeys.last_name = '%s';", jockey_name);
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
        return;
    }
    printf("Races for Jockey %s:\n", jockey_name);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char* date = (const char*)sqlite3_column_text(stmt, 0);
        int horse_id = sqlite3_column_int(stmt, 1);
        int place = sqlite3_column_int(stmt, 2);
        printf("Date: %s, Horse ID: %d, Place: %d\n", date, horse_id, place);
    }
    sqlite3_finalize(stmt);
}

void get_owner_horses_and_races(sqlite3* db, const char* owner_name) {
    char query[256];
    snprintf(query, sizeof(query), "SELECT Horses.*, Races.date, Races.place FROM Horses LEFT JOIN Races ON Horses.id = Races.horse_id WHERE Horses.owner = '%s';", owner_name);
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
        return;
    }
    printf("Horses and Races for Owner %s:\n", owner_name);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char* name = (const char*)sqlite3_column_text(stmt, 1);
        int age = sqlite3_column_int(stmt, 2);
        int experience = sqlite3_column_int(stmt, 3);
        const char* owner = (const char*)sqlite3_column_text(stmt, 4);
        float price = sqlite3_column_double(stmt, 5);
        const char* date = (const char*)sqlite3_column_text(stmt, 6);
        int place = sqlite3_column_int(stmt, 7);
        printf("ID: %d, Name: %s, Age: %d, Experience: %d, Owner: %s, Price: %.2f, Date: %s, Place: %d\n", id, name, age, experience, owner, price, date, place);
    }
    sqlite3_finalize(stmt);
}

void get_races_by_period(sqlite3* db, const char* start_date, const char* end_date) {
    char query[256];
    snprintf(query, sizeof(query), "SELECT * FROM Races WHERE date BETWEEN '%s' AND '%s';", start_date, end_date);
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
        return;
    }
    printf("Races between %s and %s:\n", start_date, end_date);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char* date = (const char*)sqlite3_column_text(stmt, 1);
        int race_number = sqlite3_column_int(stmt, 2);
        int horse_id = sqlite3_column_int(stmt, 3);
        int jockey_id = sqlite3_column_int(stmt, 4);
        int place = sqlite3_column_int(stmt, 5);
        printf("ID: %d, Date: %s, Race Number: %d, Horse ID: %d, Jockey ID: %d, Place: %d\n", id, date, race_number, horse_id, jockey_id, place);
    }
    sqlite3_finalize(stmt);
}


void update_all_horses_photo(sqlite3* db, const char* file_path) {
    FILE* f = fopen(file_path, "rb");
    if (!f) {
        fprintf(stderr, "Cannot open file %s\n", file_path);
        return;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);

    unsigned char* buffer = (unsigned char*)malloc(size);
    if (!buffer) {
        fclose(f);
        fprintf(stderr, "Memory allocation error.\n");
        return;
    }
    fread(buffer, 1, size, f);
    fclose(f);

    const char* sql = "UPDATE Horses SET photo = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        free(buffer);
        return;
    }

 
    sqlite3_bind_blob(stmt, 1, buffer, size, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Failed to execute UPDATE: %s\n", sqlite3_errmsg(db));
    }
    else {
        printf("All horses updated with photo from \"%s\".\n", file_path);
    }

    sqlite3_finalize(stmt);
    free(buffer);
}

