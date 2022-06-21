#include <doctest.h>
#include "func.h"




TEST_CASE("Test1") {
            REQUIRE(ratio_t(67) == -8);}

TEST_CASE("Test2") {
            REQUIRE(ratio_t(1239) == -10);}

TEST_CASE("Test3") {
            REQUIRE(ratio_t(719) == -30);}




TEST_CASE("Test4") {
            REQUIRE(project_t(22,33,11) == -78196.35938f);}

TEST_CASE("Test5") {
            REQUIRE(project_t(440,666,225) == -3822.93311f);}

TEST_CASE("Test6") {
            REQUIRE(project_t(10,150,15) == -57343.99609f);}


// Для проверки всех ошибок
extern "C" const char *__asan_default_options() {
    return "debug=1:detect_invalid_pointer_pairs=2:detect_leak=1:detect_leaks=1:leak_check_at_exit=true:color=always";
}