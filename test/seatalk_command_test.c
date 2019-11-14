#include <stdio.h>
#include "defines.h"
#include "test.h"
#include "seatalk_command_test.h"
#include "../seatalk_command.c"

void clear_all_commands() {
  command_datagram_bytes_remaining = 0;
}

void refute_command_pending() {
  refute(seatalk_command_pending(), "expected no command to be pending");
}

void assert_command_pending() {
  assert(seatalk_command_pending(), "expected command to be pending after triggering one");
}

#define TEST_COMMAND(NAME, SEND_COMMAND) TEST(NAME##_command)\
  clear_all_commands();\
  refute_command_pending();\
  SEND_COMMAND;\
  assert_command_pending();\
}

TEST_COMMAND(autopilot_remote_keystroke, autopilot_remote_keystroke(ST_AUTOPILOT_COMMAND_AUTO))

TEST_COMMAND(set_autopilot_response_level, set_autopilot_response_level(AUTOPILOT_RESPONSE_LEVEL_MINIMUM_DEADBAND))

TEST_COMMAND(set_lamp_intensity, set_lamp_intensity(3))

void test_seatalk_command() {
  printf("--- Testing seatalk_command.c\n");
  test_autopilot_remote_keystroke_command();
  test_set_autopilot_response_level_command();
  test_set_lamp_intensity_command();
}

