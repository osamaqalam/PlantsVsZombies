#include "unity\unity.h"
#include "module1.h"

void setUp(void) {
    // Optional: Run before every test
}

void tearDown(void) {
    // Optional: Run after every test
}

void test_add_should_return_sum_of_two_numbers(void) {
    TEST_ASSERT_EQUAL(5, add(2, 3));  // Example test case
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_add_should_return_sum_of_two_numbers);
    return UNITY_END();
}
