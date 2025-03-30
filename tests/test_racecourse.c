#include <check.h>
#include <stdlib.h>  // Добавьте эту строку
#include "racecourse.h"
#include "database.h"

START_TEST(test_add_horse) {
    sqlite3* db = connect_db(":memory:");
    initialize_db(db);

    add_horse(db, "TestHorse", 5, 3, "Owner1", 1000.0);

    char query[] = "SELECT COUNT(*) FROM Horses WHERE name = 'TestHorse';";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, 0) != SQLITE_OK) {
        ck_abort_msg("Failed to execute statement: %s\n", sqlite3_errmsg(db));
    }
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int count = sqlite3_column_int(stmt, 0);
        ck_assert_int_eq(count, 1);
    }
    sqlite3_finalize(stmt);

    disconnect_db(db);
}
END_TEST

START_TEST(test_add_jockey) {
    sqlite3* db = connect_db(":memory:");
    initialize_db(db);

    add_jockey(db, "TestJockey", 10, 1990, "123 Test St");

    char query[] = "SELECT COUNT(*) FROM Jockeys WHERE last_name = 'TestJockey';";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, 0) != SQLITE_OK) {
        ck_abort_msg("Failed to execute statement: %s\n", sqlite3_errmsg(db));
    }
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int count = sqlite3_column_int(stmt, 0);
        ck_assert_int_eq(count, 1);
    }
    sqlite3_finalize(stmt);

    disconnect_db(db);
}
END_TEST

Suite* racecourse_suite(void) {
    Suite* s;
    TCase* tc_core;

    s = suite_create("Racecourse");

    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_add_horse);
    tcase_add_test(tc_core, test_add_jockey);
    suite_add_tcase(s, tc_core);

    return s;
}

int main(void) {
    int number_failed;
    Suite* s;
    SRunner* sr;

    s = racecourse_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
