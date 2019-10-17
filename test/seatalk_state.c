#include "test.h"
#include "../seatalk_state.c"

TEST(set_compass_sensor)
  INVALIDATE_SENSOR(compass);
  refute(VALID_SENSOR(compass), "compass_sensor should not be valid on startup");
  set_compass_sensor(123);
  assert(VALID_SENSOR(compass), "compass_sensor should be valid after setting it");
  assert_equal_int(123, SENSOR_VARIABLE(compass), "compass_sensor should have been set");
}

TEST(seatalk_sensor_pending)
  INVALIDATE_SENSOR(compass);
  refute(seatalk_sensor_pending(), "no sensor data is awaiting transmission");
  set_compass_sensor(123);
  assert(seatalk_sensor_pending(), "sensor data should be pending after setting ap_heading");
}

void test_seatalk_state() {
  printf("--- Testing seatalk_state.c\n");
  test_set_compass_sensor();
  test_seatalk_sensor_pending();
}

