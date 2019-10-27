#include "defines.h"
#include "assertions.h"
#include <string.h>

#define TEST(NAME) void test_##NAME() {\
  sprintf(test_name, #NAME);\
  printf("Running %s\n", test_name);

char test_name[256];
