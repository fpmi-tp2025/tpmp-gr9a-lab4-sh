#ifndef RACECOURSE_H
#define RACECOURSE_H

#include <sqlite3.h>

void add_horse(sqlite3* db, const char* name, int age, int experience, const char* owner, float price);
void add_jockey(sqlite3* db, const char* last_name, int experience, int birth_year, const char* address);
void add_race(sqlite3* db, const char* date, int race_number, int horse_id, int jockey_id, int place);
void update_horse(sqlite3* db, int id, const char* name, int age, int experience, const char* owner, float price);
void delete_horse(sqlite3* db, int id);
void distribute_prize_fund(sqlite3* db, float fund);
void get_jockey_races(sqlite3* db, const char* jockey_name);
void get_all_horses(sqlite3* db);
void get_all_jockeys(sqlite3* db);
void get_all_races(sqlite3* db);
void get_all_users(sqlite3* db);
void get_all_prize_funds(sqlite3* db);
void get_most_winning_horse(sqlite3* db);
void get_most_active_jockey(sqlite3* db);
void get_jockey_races_by_name(sqlite3* db, const char* jockey_name);
void get_owner_horses_and_races(sqlite3* db, const char* owner_name);
void get_races_by_period(sqlite3* db, const char* start_date, const char* end_date);

#endif // RACECOURSE_H
