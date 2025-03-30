#include <check.h>
#include <stdlib.h> 
#include "auth.h"
#include "database.h"

START_TEST(test_register_user) {
    sqlite3* db = connect_db(":memory:");
    initialize_db(db);

    ck_assert(register_user(db, "testuser", "testpass", "admin"));
    ck_assert(register_user(db, "testuser2", "testpass2", "owner"));
    ck_assert(register_user(db, "testuser3", "testpass3", "jockey"));

    disconnect_db(db);
}
END_TEST

START_TEST(test_login_user) {
    sqlite3* db = connect_db(":memory:");
    initialize_db(db);

    register_user(db, "testuser", "testpass", "admin");



    disconnect_db(db);
}
END_TEST

Suite* auth_suite(void) {
    Suite* s;
    TCase* tc_core;

    s = suite_create("Auth");

    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_register_user);
    tcase_add_test(tc_core, test_login_user);
    suite_add_tcase(s, tc_core);

    return s;
}

int main(void) {
    int number_failed;
    Suite* s;
    SRunner* sr;

    s = auth_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
