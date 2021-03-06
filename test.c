#include "json.h"
#include <CUnit/Basic.h>

json *parse_or_fail(const char *json_str) {
  json *j = json_parse_from(json_str);
  if (!j) {
    CU_FAIL_FATAL("Allocation failed or parse failed");
  }
  return j;
}

void expect_int_item(json *j, const char *key, int expected) {
  const json_item *ent = json_get_item(j, key);
  if (!ent) {
    json_destroy(j);
    fprintf(stderr, "Key: %s, ExpectedValue: %d \n", key, expected);
    CU_FAIL_FATAL("Entry field was expected to be found.");
  }
  CU_ASSERT_EQUAL(ent->type, JSON_TYPE_INTEGER);
  CU_ASSERT_EQUAL(*(int *)ent->data, expected);
}

void test_basic(void) {
  json *j = parse_or_fail("{\"field\":1}");

  expect_int_item(j, "field", 1);

  json_destroy(j);
}

void test_basic_named(void) {
  json *j = parse_or_fail("{\"value\":1}");

  expect_int_item(j, "value", 1);

  json_destroy(j);
}

void test_hash(void) {
  json *j = parse_or_fail("{\"field\":1}");

  const json_item *ent = json_get_item(j, "feild");
  CU_ASSERT_EQUAL(ent, NULL);

  json_destroy(j);
}

void test_hash2(void) {
  json *j = parse_or_fail("{\"field\":1, \"feild\":2, \"efild\":3}");

  expect_int_item(j, "field", 1);
  expect_int_item(j, "feild", 2);
  expect_int_item(j, "efild", 3);

  json_destroy(j);
}

void test_int(void) {
  json *j = parse_or_fail("{\"field\":123}");

  expect_int_item(j, "field", 123);

  json_destroy(j);
}

void test_basic_space(void) {
  json *j = parse_or_fail(" { \"field\":\n\r\t1 }");

  expect_int_item(j, "field", 1);

  json_destroy(j);
}

void test_multi_field(void) {
  json *j = parse_or_fail(" { \"field\": 1, \"value\": 25 }");

  expect_int_item(j, "field", 1);
  expect_int_item(j, "value", 25);

  json_destroy(j);
}

int main(void) {
  CU_pSuite suite;
  CU_initialize_registry();

  suite = CU_add_suite("JSON test", NULL, NULL);
  CU_add_test(suite, "basic", test_basic);
  CU_add_test(suite, "basic_named", test_basic_named);
  CU_add_test(suite, "basic_space", test_basic_space);
  CU_add_test(suite, "hash", test_hash);
  CU_add_test(suite, "hash2", test_hash2);
  CU_add_test(suite, "int", test_int);
  CU_add_test(suite, "multi_field", test_multi_field);

  CU_basic_run_tests();
  CU_cleanup_registry();

  return 0;
}