#include <stdio.h>
#include <string.h>
#include "assertions.h"

int assertion_count = 0;
int failure_count = 0;

int total_assertions() {
  return assertion_count;
}

int total_failures() {
  return failure_count;
}

void _assert(int true_or_false, char *failure_text, char *explanation) {
  assertion_count += 1;
  if (true_or_false) {
    return;
  }
  failure_count += 1;
  printf("%s: %s\n", failure_text, explanation);
}

void assert(int true_or_false, char *explanation) {
  _assert(true_or_false, "expected TRUE but found FALSE", explanation);
}

void refute(int true_or_false, char *explanation) {
  _assert(!true_or_false, "expected FALSE but found TRUE", explanation);
}

void assert_equal_int(int reference, int test, char *explanation) {
  char failure_message[256];
  sprintf(failure_message, "expected %d to equal %d", test, reference);
  _assert(reference == test, failure_message, explanation);
}

void assert_equal_string(char *reference, char *test, char *explanation) {
  char failure_message[256];
  sprintf(failure_message, "expected %s to equal %s", test, reference); // overflow possible here
  _assert(strcmp(reference, test) == 0, failure_message, explanation);
}

void assert_equal_float(float reference, float test, char *explanation) {
  char failure_message[256];
  sprintf(failure_message, "expected %f to equal %f", test, reference);
  _assert(reference == test, failure_message, explanation);
}

