#include "json.h"
#include <CUnit/Basic.h>

void test_parse_json(void) {
  json *j = json_parse_from("{\"field\":1}");
  if (!j) {
    CU_FAIL_FATAL("Allocation failed");
  }

  const json_item *ent = json_get_item(j, "field");
  if (!ent) {
    json_destroy(j);
    CU_FAIL_FATAL("Entry field was expected to be found.");
  }
  CU_ASSERT_EQUAL(ent->type, JSON_TYPE_INTEGER);
  CU_ASSERT_EQUAL(*(int *)ent->data, 1);
  
  json_destroy(j);
}

int main(void) {
  CU_pSuite suite;
  CU_initialize_registry();

  suite = CU_add_suite("JSON test", NULL, NULL);
  CU_add_test(suite, "basic", test_parse_json);

  CU_basic_run_tests();
  CU_cleanup_registry();

  return 0;
}