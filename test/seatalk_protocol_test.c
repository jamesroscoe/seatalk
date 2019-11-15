#include <stdio.h>
#include <math.h>
#include "test.h"
#include "seatalk_protocol_test.h"
#include "../seatalk_protocol.c"
#include "seatalk_hardware_layer.h"
#include "seatalk_command_test.h"

void simulate_receive_datagram(char *datagram, int length) {
  for (int i = 0; i < length; i++) {
    simulate_receive_character(datagram[i], (i == 0) ? 1 : 0);
  }
}

void invalidate_all_sensors() {
  invalidate_heading_sensor();
  invalidate_water_speed_in_knots_times_100_sensor();
  invalidate_apparent_wind_angle_sensor();
  invalidate_apparent_wind_speed_in_knots_times_10_sensor();
  invalidate_depth_below_transducer_in_feet_times_10_sensor();
  invalidate_course_over_ground_sensor();
  invalidate_speed_over_ground_in_knots_times_100_sensor();
  invalidate_water_temperature_in_degrees_celsius_times_10_sensor();
  invalidate_rudder_position_in_degrees_right_sensor();
}

#define ASSERT_GET_VALUE(NAME, VARIABLE) assert_equal_int(0, get_##NAME(&VARIABLE), "get_" #NAME "() should return a value")

#define RAW_RAW_TEST_RECEIVE(NAME, TYPE) TEST(receive_##NAME)\
  char datagram[256];\
  int length;\
  TYPE status_value;

#define RAW_BUILD_AND_SIMULATE_DATAGRAM(DATAGRAM) length = DATAGRAM;\
  simulate_receive_datagram(datagram, length)

#define BUILD_AND_SIMULATE_DATAGRAM(NAME, DATAGRAM) RAW_BUILD_AND_SIMULATE_DATAGRAM(DATAGRAM);\
  ASSERT_GET_VALUE(NAME, status_value)

#define RAW_TEST_RECEIVE(NAME, DATAGRAM, TYPE) RAW_RAW_TEST_RECEIVE(NAME, TYPE)\
  RAW_BUILD_AND_SIMULATE_DATAGRAM(DATAGRAM);

#define TEST_RECEIVE(NAME, DATAGRAM, TYPE) RAW_TEST_RECEIVE(NAME, DATAGRAM, TYPE)\
  ASSERT_GET_VALUE(NAME, status_value);

#define TEST_RECEIVE_INT(NAME, DATAGRAM, REFERENCE_VALUE) TEST_RECEIVE(NAME, DATAGRAM, int)\
  assert_equal_int(REFERENCE_VALUE, status_value, "status_value");\
}

#define DEPTH_BELOW_TRANSDUCER_TEST_VALUE 1234
TEST_RECEIVE_INT(depth_below_transducer_in_feet_times_10, build_depth_below_transducer(datagram, DEPTH_BELOW_TRANSDUCER_TEST_VALUE, 0, 0, 0), DEPTH_BELOW_TRANSDUCER_TEST_VALUE)

#define ENGINE_RPM_TEST_VALUE 3600
#define ENGINE_PITCH_TEST_VALUE 10

#define TEST_RECEIVE_ENGINE_STATUS(ENGINE_METHOD, ENGINE_ID) RAW_TEST_RECEIVE(ENGINE_METHOD##_engine_status, build_engine_rpm_and_pitch(datagram, ENGINE_ID_##ENGINE_ID, ENGINE_RPM_TEST_VALUE, ENGINE_PITCH_TEST_VALUE), ENGINE_STATUS)\
  assert_equal_int(0, get_engine_status(ENGINE_ID_##ENGINE_ID, &status_value), "get_engine_status() should have returned engine status");\
  assert_equal_int(ENGINE_RPM_TEST_VALUE, status_value.rpm, "rpm");\
  assert_equal_int(ENGINE_PITCH_TEST_VALUE, status_value.pitch_percent, "pitch_percent");\
}

TEST_RECEIVE_ENGINE_STATUS(single, SINGLE)
TEST_RECEIVE_ENGINE_STATUS(port, PORT)
TEST_RECEIVE_ENGINE_STATUS(starboard, STARBOARD)

#define APPARENT_WIND_ANGLE_TEST_VALUE 91
TEST_RECEIVE_INT(apparent_wind_angle, build_apparent_wind_angle(datagram, APPARENT_WIND_ANGLE_TEST_VALUE), APPARENT_WIND_ANGLE_TEST_VALUE / 2)

#define APPARENT_WIND_SPEED_TEST_VALUE 227
TEST_RECEIVE_INT(apparent_wind_speed_in_knots_times_10, build_apparent_wind_speed(datagram, APPARENT_WIND_SPEED_TEST_VALUE, 0), APPARENT_WIND_SPEED_TEST_VALUE)

#define HEADING_TEST_VALUE 347
#define HEADING_WITH_RUDDER_TEST_VALUE 015
TEST_RECEIVE(heading, build_heading(datagram, HEADING_TEST_VALUE, 0, 0), int)
  assert_equal_int(HEADING_TEST_VALUE, status_value, "heading value from build_heading()");
  BUILD_AND_SIMULATE_DATAGRAM(heading, build_heading_and_rudder_position(datagram, HEADING_WITH_RUDDER_TEST_VALUE, TURN_DIRECTION_LEFT, 0));
  assert_equal_int(HEADING_WITH_RUDDER_TEST_VALUE, status_value, "hedading value from build_heading_and_rudder_position()");
}

#define TURN_DIRECTION_TEST_VALUE TURN_DIRECTION_RIGHT
#define TURN_DIRECTION_WITH_RUDDER_TEST_VALUE TURN_DIRECTION_LEFT
TEST_RECEIVE(turn_direction, build_autopilot_status(datagram, 123, TURN_DIRECTION_TEST_VALUE, 125, AUTOPILOT_MODE_AUTO, 0, 0, 0), TURN_DIRECTION)
  assert_equal_int(TURN_DIRECTION_TEST_VALUE, status_value, "turn_direction from build_autopilot_status()");
  BUILD_AND_SIMULATE_DATAGRAM(turn_direction, build_heading_and_rudder_position(datagram, 0, TURN_DIRECTION_WITH_RUDDER_TEST_VALUE, 0));
  assert_equal_int(TURN_DIRECTION_WITH_RUDDER_TEST_VALUE, status_value, "turb_direction from build_heading_and_rudder_position()");
}

#define WATER_SPEED_TEST_VALUE 620
TEST_RECEIVE_INT(water_speed_in_knots_times_100, build_water_speed(datagram, WATER_SPEED_TEST_VALUE / 10),  WATER_SPEED_TEST_VALUE)

#define AVERAGE_WATER_SPEED_TEST_VALUE 619
TEST_RECEIVE_INT(average_water_speed_in_knots_times_100, build_average_water_speed(datagram, WATER_SPEED_TEST_VALUE, AVERAGE_WATER_SPEED_TEST_VALUE, 1, 1, 0, 0), AVERAGE_WATER_SPEED_TEST_VALUE)

#define RUDDER_POSITION_TEST_VALUE -10
#define RUDDER_POSITION_AUTOPILOT_TEST_VALUE 13
TEST_RECEIVE(rudder_position_in_degrees_right, build_heading_and_rudder_position(datagram, 0, TURN_DIRECTION_LEFT, RUDDER_POSITION_TEST_VALUE), int)
  assert_equal_int(RUDDER_POSITION_TEST_VALUE, status_value, "rudder_position from build_heading_and_rudder_position()");
  BUILD_AND_SIMULATE_DATAGRAM(rudder_position_in_degrees_right, build_autopilot_status(datagram, 321, TURN_DIRECTION_LEFT, 312, AUTOPILOT_MODE_AUTO, RUDDER_POSITION_AUTOPILOT_TEST_VALUE, 0, 0));
  assert_equal_int(RUDDER_POSITION_AUTOPILOT_TEST_VALUE, status_value, "rudder position from build_autopilot_status()");
}

#define COURSE_OVER_GROUND_TEST_VALUE 179
TEST_RECEIVE_INT(course_over_ground, build_course_over_ground(datagram, COURSE_OVER_GROUND_TEST_VALUE), COURSE_OVER_GROUND_TEST_VALUE)

#define SPEED_OVER_GROUND_TEST_VALUE 65
// note: the speed over ground datagram has only 1/10 kt precision. It is stored internally with 1/100 kt precision to be consistent with water speed
TEST_RECEIVE_INT(speed_over_ground_in_knots_times_100, build_speed_over_ground(datagram, SPEED_OVER_GROUND_TEST_VALUE), SPEED_OVER_GROUND_TEST_VALUE * 10)

#define TRIP_MILEAGE_TEST_VALUE 34567
#define TRIP_MILEAGE_WITH_TOTAL_TEST_VALUE 567890
TEST_RECEIVE(trip_mileage_in_nautical_miles_times_100, build_trip_mileage(datagram, TRIP_MILEAGE_TEST_VALUE), int)
  assert_equal_int(TRIP_MILEAGE_TEST_VALUE, status_value, "trip_mileage from build_trip_mileage()");
  BUILD_AND_SIMULATE_DATAGRAM(trip_mileage_in_nautical_miles_times_100, build_total_and_trip_mileage(datagram, 0, TRIP_MILEAGE_WITH_TOTAL_TEST_VALUE));
  assert_equal_int(TRIP_MILEAGE_WITH_TOTAL_TEST_VALUE, status_value, "trip_mileage from build_total_and_trip_mileage()");
}

#define TOTAL_MILEAGE_TEST_VALUE 65535
#define TOTAL_MILEAGE_WITH_TRIP_TEST_VALUE 987654
TEST_RECEIVE(total_mileage_in_nautical_miles_times_10, build_total_mileage(datagram, TOTAL_MILEAGE_TEST_VALUE), int)
  assert_equal_int(TOTAL_MILEAGE_TEST_VALUE, status_value, "total_mileage from build_total_mileage()");
  BUILD_AND_SIMULATE_DATAGRAM(total_mileage_in_nautical_miles_times_10, build_total_and_trip_mileage(datagram, TOTAL_MILEAGE_WITH_TRIP_TEST_VALUE, 0));
  assert_equal_int(TOTAL_MILEAGE_WITH_TRIP_TEST_VALUE, status_value, "total_mileage from build_total_and_trip_mileage()");
}

// Fahrenheit but gets converted to Celsius * 10
#define WATER_TEMPERATURE_TEST_VALUE 70
#define WATER_TEMPERATURE_CELSIUS_TEST_VALUE 191
TEST_RECEIVE(water_temperature_in_degrees_celsius_times_10, build_water_temperature(datagram, WATER_TEMPERATURE_TEST_VALUE, 0), int)
  assert_equal_int(round(((WATER_TEMPERATURE_TEST_VALUE - 32) / 9.0 * 5.0) * 10.0), status_value, "water_temperature from build_water_temperature()");
  BUILD_AND_SIMULATE_DATAGRAM(water_temperature_in_degrees_celsius_times_10, build_precise_water_temperature(datagram, WATER_TEMPERATURE_CELSIUS_TEST_VALUE));
  assert_equal_int(WATER_TEMPERATURE_CELSIUS_TEST_VALUE, status_value, "water_temperature from build_precise_water_temperature()");
}

#define LAT_HEMISPHERE_TEST_VALUE LATITUDE_HEMISPHERE_SOUTH
#define LAT_DEGREES_TEST_VALUE 62
#define LAT_MINUTES_TEST_VALUE 1592
#define LON_HEMISPHERE_TEST_VALUE LONGITUDE_HEMISPHERE_EAST
#define LON_DEGREES_TEST_VALUE 171
#define LON_MINUTES_TEST_VALUE 3712
#define LAT_HEMISPHERE_COMBINED_TEST_VALUE LATITUDE_HEMISPHERE_NORTH
#define LAT_DEGREES_COMBINED_TEST_VALUE 19
#define LAT_MINUTES_COMBINED_TEST_VALUE 57321
#define LON_HEMISPHERE_COMBINED_TEST_VALUE LONGITUDE_HEMISPHERE_WEST
#define LON_DEGREES_COMBINED_TEST_VALUE 135
#define LON_MINUTES_COMBINED_TEST_VALUE 5123
RAW_RAW_TEST_RECEIVE(position, POSITION)
  RAW_BUILD_AND_SIMULATE_DATAGRAM(build_lat_position(datagram, LAT_HEMISPHERE_TEST_VALUE, LAT_DEGREES_TEST_VALUE, LAT_MINUTES_TEST_VALUE));
  RAW_BUILD_AND_SIMULATE_DATAGRAM(build_lon_position(datagram, LON_HEMISPHERE_TEST_VALUE, LON_DEGREES_TEST_VALUE, LON_MINUTES_TEST_VALUE));
  ASSERT_GET_VALUE(position, status_value);
  assert_equal_int(LAT_HEMISPHERE_TEST_VALUE, status_value.hemisphere_latitude, "hemisphere_latitude from separate lat/lon datagrams");
  assert_equal_int(LAT_DEGREES_TEST_VALUE, status_value.degrees_latitude, "degrees_latitude from separate lat/lon datagrams");
  assert_equal_int(LAT_MINUTES_TEST_VALUE * 10, status_value.minutes_latitude_times_1000, "minutes_latitude from separate lat/lon datagrams");
  assert_equal_int(LON_HEMISPHERE_TEST_VALUE, status_value.hemisphere_longitude, "hemisphere_longitude from separate lat/lon datagrams");
  assert_equal_int(LON_DEGREES_TEST_VALUE, status_value.degrees_longitude, "degrees_lonitude from separate lat/lon datagrams");
  assert_equal_int(LON_MINUTES_TEST_VALUE * 10, status_value.minutes_longitude_times_1000, "minutes_longitude from separate lat/lon datagrams");
  BUILD_AND_SIMULATE_DATAGRAM(position, build_lat_lon_position(datagram, LAT_HEMISPHERE_COMBINED_TEST_VALUE, LAT_DEGREES_COMBINED_TEST_VALUE, LAT_MINUTES_COMBINED_TEST_VALUE, LON_HEMISPHERE_COMBINED_TEST_VALUE, LON_DEGREES_COMBINED_TEST_VALUE, LON_MINUTES_COMBINED_TEST_VALUE));
  assert_equal_int(LAT_HEMISPHERE_COMBINED_TEST_VALUE, status_value.hemisphere_latitude, "hemisphere_latitude from comgined lat/lon datagram");
  assert_equal_int(LAT_DEGREES_COMBINED_TEST_VALUE, status_value.degrees_latitude, "degrees_latitude from combined lat/lon datagram");
  assert_equal_int(LAT_MINUTES_COMBINED_TEST_VALUE, status_value.minutes_latitude_times_1000, "minutes_latitude from combined lat/lon datagram");
  assert_equal_int(LON_HEMISPHERE_COMBINED_TEST_VALUE, status_value.hemisphere_longitude, "hemisphere_longitude from combined lat/lon datagram");
  assert_equal_int(LON_DEGREES_COMBINED_TEST_VALUE, status_value.degrees_longitude, "degrees_lonitude from combined lat/lon datagram");
  assert_equal_int(LON_MINUTES_COMBINED_TEST_VALUE, status_value.minutes_longitude_times_1000, "minutes_longitude from combined lat/lon datagram");
}

#define SATELLITE_COUNT_TEST_VALUE 15
#define POSITION_ERROR_TEST_VALUE 26
RAW_RAW_TEST_RECEIVE(gps_fix_quality, GPS_FIX_QUALITY)
  BUILD_AND_SIMULATE_DATAGRAM(gps_fix_quality, build_satellite_info(datagram, SATELLITE_COUNT_TEST_VALUE, POSITION_ERROR_TEST_VALUE));
  assert_equal_int(1, status_value.satellite_count_valid, "satellite_count_valid"); // not really a great test
  assert_equal_int(SATELLITE_COUNT_TEST_VALUE, status_value.satellite_count, "satellite_count");
  assert_equal_int(1, status_value.position_error_valid, "position_error_valid"); // not really a great test
  assert_equal_int(POSITION_ERROR_TEST_VALUE, status_value.position_error, "position_error");
  // TODO: build_gps_and_dgps_fix_info
}

#define CROSS_TRACK_TEST_VALUE 1066
#define WAYPOINT_BEARING_TEST_VALUE 62
#define WAYPOINT_BEARING_REFERENCE_TEST_VALUE ANGLE_REFERENCE_TRUE
#define WAYPOINT_RANGE_TEST_VALUE 2900
#define DIRECTION_TO_STEER_TEST_VALUE 59
RAW_RAW_TEST_RECEIVE(navigation, NAVIGATION_STATUS)
  BUILD_AND_SIMULATE_DATAGRAM(navigation, build_waypoint_navigation(datagram, 1, CROSS_TRACK_TEST_VALUE, 1, WAYPOINT_BEARING_TEST_VALUE, WAYPOINT_BEARING_REFERENCE_TEST_VALUE, 1, WAYPOINT_RANGE_TEST_VALUE, DIRECTION_TO_STEER_TEST_VALUE));
  assert_equal_int(CROSS_TRACK_TEST_VALUE, status_value.cross_track_error_in_nautical_miles_times_100, "cross_track_error");
  assert_equal_int(WAYPOINT_BEARING_TEST_VALUE, status_value.waypoint_bearing, "waypoint_bearing");
  assert_equal_int(WAYPOINT_BEARING_REFERENCE_TEST_VALUE, status_value.waypoint_bearing_reference, "waypoint_bearing_reference");
  assert_equal_int(WAYPOINT_RANGE_TEST_VALUE, status_value.waypoint_range_in_nautical_miles_times_100, "waypoint_range");
//  assert_equal_int(DIRECTION_TO_STEER_TEST_VALUE, status_value.direction_to_steer, "direction_to_steer");
  // TODO: waypoint position and waypoint name
}

#define YEAR_TEST_VALUE 2019
#define MONTH_TEST_VALUE 11
#define DAY_TEST_VALUE 13
#define HOUR_TEST_VALUE 23
#define MINUTE_TEST_VALUE 40
#define SECOND_TEST_VALUE 59
RAW_RAW_TEST_RECEIVE(gmt_date_and_time, DATE_AND_TIME)
  RAW_BUILD_AND_SIMULATE_DATAGRAM(build_date(datagram, YEAR_TEST_VALUE, MONTH_TEST_VALUE, DAY_TEST_VALUE));
  RAW_BUILD_AND_SIMULATE_DATAGRAM(build_gmt_time(datagram, HOUR_TEST_VALUE, MINUTE_TEST_VALUE, SECOND_TEST_VALUE));
  ASSERT_GET_VALUE(gmt_date_and_time, status_value);
  assert_equal_int(YEAR_TEST_VALUE, status_value.year, "year");
  assert_equal_int(MONTH_TEST_VALUE, status_value.month, "month");
  assert_equal_int(DAY_TEST_VALUE, status_value.day, "day");
  assert_equal_int(HOUR_TEST_VALUE, status_value.hour, "hour");
  assert_equal_int(MINUTE_TEST_VALUE, status_value.minute, "minute");
  assert_equal_int(SECOND_TEST_VALUE, status_value.second, "second");
}

#define AUTOPILOT_TARGET_TEST_VALUE 37
#define AUTOPILOT_MODE_TEST_VALUE AUTOPILOT_MODE_AUTO
RAW_RAW_TEST_RECEIVE(autopilot_status, AUTOPILOT_STATUS)
  BUILD_AND_SIMULATE_DATAGRAM(autopilot, build_autopilot_status(datagram, 0, TURN_DIRECTION_LEFT, AUTOPILOT_TARGET_TEST_VALUE, AUTOPILOT_MODE_TEST_VALUE, 0, 0, 0));
  assert_equal_int(AUTOPILOT_TARGET_TEST_VALUE, status_value.target_heading, "target_heading");
  assert_equal_int(AUTOPILOT_MODE_TEST_VALUE, status_value.mode, "mode");
  // response_level, rudder gain
}

void refute_seatalk_sensor_pending() {
  SENSOR_ID sensor_id;
  refute(seatalk_sensor_pending(&sensor_id), "no sensors should be pending after invalidate_all_sensors()");
}

void assert_seatalk_sensor_pending() {
  SENSOR_ID sensor_id;
  assert(seatalk_sensor_pending(&sensor_id), "sensor should be pending after setting sensor value");
}

#define ASSERT_EQUAL_DATAGRAM(REFERENCE_DATAGRAM, REFERENCE_LENGTH, TEST_DATAGRAM, TEST_LENGTH, NAME) assert_equal_int(REFERENCE_LENGTH, TEST_LENGTH, #NAME " datagram length");\
  for (int i = 0; i < REFERENCE_LENGTH; i++) {\
    char message[256];\
    sprintf(message, #NAME " datagram[%d]", i);\
    assert_equal_int(REFERENCE_DATAGRAM[i], TEST_DATAGRAM[i], message);\
  }

#define TEST_SENSOR_UPDATED(NAME, REFERENCE_VALUE, DATAGRAM) TEST(update_##NAME##_sensor)\
  char test_datagram[256], datagram[256];\
  int test_length, length;\
  invalidate_all_sensors();\
  refute_seatalk_sensor_pending();\
  update_##NAME##_sensor(REFERENCE_VALUE);\
  assert_seatalk_sensor_pending();\
  test_length = get_pending_datagram(test_datagram);\
  length = DATAGRAM;\
  ASSERT_EQUAL_DATAGRAM(datagram, length, test_datagram, test_length, NAME);\
}

#define HEADING_SENSOR_TEST_VALUE 359
TEST_SENSOR_UPDATED(heading, HEADING_SENSOR_TEST_VALUE, build_heading(datagram, HEADING_SENSOR_TEST_VALUE, 0, 0))

#define WATER_SPEED_SENSOR_TEST_VALUE 789
TEST_SENSOR_UPDATED(water_speed_in_knots_times_100, WATER_SPEED_SENSOR_TEST_VALUE, build_average_water_speed(datagram, WATER_SPEED_SENSOR_TEST_VALUE, 0, 1, 0, 1, 0))

#define APPARENT_WIND_ANGLE_SENSOR_TEST_VALUE 167
TEST_SENSOR_UPDATED(apparent_wind_angle, APPARENT_WIND_ANGLE_SENSOR_TEST_VALUE, build_apparent_wind_angle(datagram, APPARENT_WIND_ANGLE_SENSOR_TEST_VALUE * 2))

#define APPARENT_WIND_SPEED_SENSOR_TEST_VALUE 193
TEST_SENSOR_UPDATED(apparent_wind_speed_in_knots_times_10, APPARENT_WIND_SPEED_SENSOR_TEST_VALUE, build_apparent_wind_speed(datagram, APPARENT_WIND_SPEED_SENSOR_TEST_VALUE, 0))

#define DEPTH_SENSOR_TEST_VALUE 2999
TEST_SENSOR_UPDATED(depth_below_transducer_in_feet_times_10, DEPTH_SENSOR_TEST_VALUE, build_depth_below_transducer(datagram, DEPTH_SENSOR_TEST_VALUE, 0, 0, 0))

#define COURSE_OVER_GROUND_SENSOR_TEST_VALUE 354
TEST_SENSOR_UPDATED(course_over_ground, COURSE_OVER_GROUND_SENSOR_TEST_VALUE, build_course_over_ground(datagram, COURSE_OVER_GROUND_SENSOR_TEST_VALUE))

#define SPEED_OVER_GROUND_SENSOR_TEST_VALUE 278
TEST_SENSOR_UPDATED(speed_over_ground_in_knots_times_100, SPEED_OVER_GROUND_SENSOR_TEST_VALUE, build_speed_over_ground(datagram, SPEED_OVER_GROUND_SENSOR_TEST_VALUE / 10))

#define WATER_TEMPERATURE_SENSOR_TEST_VALUE 182
TEST_SENSOR_UPDATED(water_temperature_in_degrees_celsius_times_10, WATER_TEMPERATURE_SENSOR_TEST_VALUE, build_precise_water_temperature(datagram, WATER_TEMPERATURE_SENSOR_TEST_VALUE))

//#define RUDDER_POSITION_SENSOR_TEST_VALUE // this one is more difficult due to reliance on heading sensor as well. Need to roll full test

#define TEST_COMMAND(NAME, TRIGGER_COMMAND, DATAGRAM) TEST(trigger_##NAME##_command)\
  char test_datagram[256], datagram[256];\
  int test_length, length;\
  length = DATAGRAM;\
  TRIGGER_COMMAND;\
  test_length = get_pending_datagram(test_datagram);\
  length = DATAGRAM;\
  ASSERT_EQUAL_DATAGRAM(datagram, length, test_datagram, test_length, NAME);\
}

#define AUTOPILOT_COMMAND_TEST_VALUE ST_AUTOPILOT_COMMAND_AUTO
TEST_COMMAND(autopilot_remote_keystroke, autopilot_remote_keystroke(AUTOPILOT_COMMAND_TEST_VALUE), build_autopilot_command(datagram, AUTOPILOT_COMMAND_TEST_VALUE))

#define AUTOPILOT_RESPONSE_TEST_VALUE AUTOPILOT_RESPONSE_LEVEL_MINIMUM_DEADBAND
TEST_COMMAND(set_autopilot_response_level, set_autopilot_response_level(AUTOPILOT_RESPONSE_TEST_VALUE), build_set_autopilot_response_level(datagram, AUTOPILOT_RESPONSE_TEST_VALUE))

#define LAMP_INTENSITY_TEST_VALUE 2
TEST_COMMAND(set_lamp_intensity, set_lamp_intensity(LAMP_INTENSITY_TEST_VALUE), build_lamp_intensity(datagram, LAMP_INTENSITY_TEST_VALUE))

void test_seatalk_protocol() {
  clear_all_commands();
  printf("--- Testing seatalk_protocol.c\n");
  // ensure status updates when receiving datagrams
  test_receive_depth_below_transducer_in_feet_times_10();
  test_receive_single_engine_status();
  test_receive_port_engine_status();
  test_receive_starboard_engine_status();
  test_receive_apparent_wind_angle();
  test_receive_apparent_wind_speed_in_knots_times_10();
  test_receive_heading();
  test_receive_turn_direction();
  test_receive_water_speed_in_knots_times_100();
  test_receive_average_water_speed_in_knots_times_100();
  test_receive_rudder_position_in_degrees_right();
  test_receive_course_over_ground();
  test_receive_speed_over_ground_in_knots_times_100();
  test_receive_trip_mileage_in_nautical_miles_times_100();
  test_receive_total_mileage_in_nautical_miles_times_10();
  test_receive_water_temperature_in_degrees_celsius_times_10();
  test_receive_position();
  test_receive_gps_fix_quality();
  test_receive_navigation();
  test_receive_gmt_date_and_time();
  test_receive_autopilot_status();
  // test datagrams get queued for output when sensors updated
  test_update_heading_sensor();
  test_update_water_speed_in_knots_times_100_sensor();
  test_update_apparent_wind_angle_sensor();
  test_update_apparent_wind_speed_in_knots_times_10_sensor();
  test_update_depth_below_transducer_in_feet_times_10_sensor();
  test_update_course_over_ground_sensor();
  test_update_speed_over_ground_in_knots_times_100_sensor();
  test_update_water_temperature_in_degrees_celsius_times_10_sensor();
//  test_update_rudder_position_in_degrees_right();
  // test command datagrams get sent immediately when triggered
  test_trigger_autopilot_remote_keystroke_command();
  test_trigger_set_autopilot_response_level_command();
  test_trigger_set_lamp_intensity_command();
}
