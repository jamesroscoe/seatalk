#include "logger.h"
#include "timeout.h"
#include "settings.h"
#include "boat_sensor.h"
#include "seatalk_protocol.h"

int sensors_initialized = 0;

#define SENSOR_VARIABLE(NAME) sensor_ ## NAME
#define SENSOR_EXPIRY_TIME(NAME) sensor_##NAME##_expiry
#define SENSOR_VALUE_TRANSMITTED_VARIABLE(NAME) sensor_##NAME##_transmitted
#define VALID_SENSOR(NAME) (sensors_initialized && timeout_still_valid(SENSOR_EXPIRY_TIME(NAME)))
#define SENSOR_VALUE_UNTRANSMITTED(NAME) !SENSOR_VALUE_TRANSMITTED_VARIABLE(NAME)
#define SET_SENSOR_EXPIRY(NAME, VALUE) SENSOR_EXPIRY_TIME(NAME) = VALUE
#define VALIDATE_SENSOR(NAME) restart_timeout(&SENSOR_EXPIRY_TIME(NAME), SENSOR_TIME_TO_LIVE)
#define INVALIDATE_SENSOR(NAME) invalidate_timeout(&SENSOR_EXPIRY_TIME(NAME))
#define UNTRANSMITTED_SENSOR_VALUE(NAME) SENSOR_VALUE_TRANSMITTED_VARIABLE(NAME) = 0
#define TRANSMITTED_SENSOR_VALUE(NAME) SENSOR_VALUE_TRANSMITTED_VARIABLE(NAME) = 1
#define DEFINE_SENSOR_EXPIRY(NAME) timeout SENSOR_EXPIRY_TIME(NAME) = TIMEOUT_DEFAULT
#define DEFINE_SENSOR_VALUE_TRANSMITTED(NAME) int SENSOR_VALUE_TRANSMITTED_VARIABLE(NAME) = 1

#define DEFINE_RAW_SENSOR(NAME, TYPE) TYPE SENSOR_VARIABLE(NAME);\

#define DEFINE_READ_ONLY_SENSOR(NAME, TYPE) DEFINE_RAW_SENSOR(NAME, TYPE)\
  DEFINE_SENSOR_EXPIRY(NAME);\
  DEFINE_SENSOR_VALUE_TRANSMITTED(NAME);\
int has_##NAME##_sensor_value_to_transmit(void) {\
  return (VALID_SENSOR(NAME) && SENSOR_VALUE_UNTRANSMITTED(NAME));\
}\
\
int pop_##NAME##_sensor_value(TYPE *value) {\
  if (has_##NAME##_sensor_value_to_transmit()) {\
    *value = SENSOR_VARIABLE(NAME);\
    TRANSMITTED_SENSOR_VALUE(NAME);\
    return 0;\
  }\
  return 1;\
}\
void invalidate_##NAME##_sensor() {\
  INVALIDATE_SENSOR(NAME);\
}

#define DEFINE_SENSOR(NAME, TYPE) DEFINE_READ_ONLY_SENSOR(NAME, TYPE)\
void update_##NAME##_sensor(TYPE NAME) {\
  SENSOR_VARIABLE(NAME) = NAME;\
  VALIDATE_SENSOR(NAME);\
  UNTRANSMITTED_SENSOR_VALUE(NAME);\
  wake_transmitter();\
}

DEFINE_SENSOR(heading, int);
DEFINE_SENSOR(water_speed_in_knots_times_100, int);
DEFINE_SENSOR(apparent_wind_angle, int);
DEFINE_SENSOR(apparent_wind_speed_in_knots_times_10, int);
DEFINE_SENSOR(depth_below_transducer_in_feet_times_10, int);
DEFINE_SENSOR(course_over_ground, int);
DEFINE_SENSOR(speed_over_ground_in_knots_times_100, int);
DEFINE_SENSOR(water_temperature_in_degrees_celsius_times_10, int);
DEFINE_SENSOR(rudder_position_in_degrees_right, int);

void initialize_sensors(void) {
  sensors_initialized = 1;
  INVALIDATE_SENSOR(heading);
  INVALIDATE_SENSOR(water_speed_in_knots_times_100);
  INVALIDATE_SENSOR(apparent_wind_angle);
  INVALIDATE_SENSOR(apparent_wind_speed_in_knots_times_10);
  INVALIDATE_SENSOR(depth_below_transducer_in_feet_times_10);
  INVALIDATE_SENSOR(course_over_ground);
  INVALIDATE_SENSOR(speed_over_ground_in_knots_times_100);
  INVALIDATE_SENSOR(water_temperature_in_degrees_celsius_times_10);
  INVALIDATE_SENSOR(rudder_position_in_degrees_right);
}

