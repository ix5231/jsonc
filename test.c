#include <CUnit/Basic.h>
#include "json.h"

void test_parse_json(void) { CU_ASSERT_EQUAL(parse_json(), 2); }

int main(void) {
  CU_pSuite suite;
  CU_initialize_registry();
  
  suite = CU_add_suite("JSON test", NULL, NULL);
  CU_add_test(suite, "basic", test_parse_json);
  
  CU_basic_run_tests();
  CU_cleanup_registry();

  return 0;
}