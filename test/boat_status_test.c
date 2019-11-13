#include "test.h"
#include "boat_status_test.h"
#include "../boat_status.c"

#define int_TEST_VALUE 137
#define TURN_DIRECTION_TEST_VALUE TURN_DIRECTION_RIGHT

#define assert_equal_TURN_DIRECTION(REF_VALUE, TEST_VALUE, MESSAGE) assert_equal_int(REF_VALUE, TEST_VALUE, MESSAGE)

#define TEST_GET_SET_INVALIDATE_TIMEOUT(STATUS_NAME, TYPE) TEST(set_and_get_##STATUS_NAME)\
  TYPE value = TYPE##_TEST_VALUE;\
  TYPE got_value;\
  refute(get_##STATUS_NAME(&got_value) == 0, #STATUS_NAME " timeout should have been invalid on initialization");\
  set_##STATUS_NAME(value);\
  assert_equal_int(0, get_##STATUS_NAME(&got_value), "get_" #STATUS_NAME "() returnd without valid value (timeout expired)");\
  assert_equal_##TYPE(TYPE##_TEST_VALUE, got_value, "get_" #STATUS_NAME "() value does not match value set");\
}

TEST_GET_SET_INVALIDATE_TIMEOUT(heading, int);
TEST_GET_SET_INVALIDATE_TIMEOUT(turn_direction, TURN_DIRECTION);
TEST_GET_SET_INVALIDATE_TIMEOUT(water_speed_in_knots_times_100, int);
TEST_GET_SET_INVALIDATE_TIMEOUT(average_water_speed_in_knots_times_100, int);
TEST_GET_SET_INVALIDATE_TIMEOUT(rudder_position_in_degrees_right, int)
TEST_GET_SET_INVALIDATE_TIMEOUT(course_over_ground, int);
TEST_GET_SET_INVALIDATE_TIMEOUT(speed_over_ground_in_knots_times_100, int);
TEST_GET_SET_INVALIDATE_TIMEOUT(trip_mileage_in_nautical_miles_times_100, int);
TEST_GET_SET_INVALIDATE_TIMEOUT(total_mileage_in_nautical_miles_times_10, int);
TEST_GET_SET_INVALIDATE_TIMEOUT(water_temperature_in_degrees_celsius_times_10, int);
TEST_GET_SET_INVALIDATE_TIMEOUT(compass_variation_in_degrees_west, int);

// engine status

void assert_position(LATITUDE_HEMISPHERE hemisphere_latitude, int degrees_latitude, int minutes_latitude_times_1000, LONGITUDE_HEMISPHERE hemisphere_longitude, int degrees_longitude, int minutes_longitude_times_1000, POSITION position) {
  assert_equal_int(hemisphere_latitude, position.hemisphere_latitude, "hemisphere_latitude");
  assert_equal_int(degrees_latitude, position.degrees_latitude, "degrees_latitude");
  assert_equal_int(minutes_latitude_times_1000, position.minutes_latitude_times_1000, "minutes_latitude_times_1000");
  assert_equal_int(hemisphere_longitude, position.hemisphere_longitude, "hemisphere_longitude");
  assert_equal_int(degrees_longitude, position.degrees_longitude, "degrees_longitude");
  assert_equal_int(minutes_longitude_times_1000, position.minutes_longitude_times_1000, "minutes_longitude_times_1000");
}

TEST(set_and_get_position)
  LATITUDE_HEMISPHERE hemisphere_latitude = LATITUDE_HEMISPHERE_SOUTH;
  int degrees_latitude = 12, minutes_latitude_times_1000 = 34567;
  LONGITUDE_HEMISPHERE hemisphere_longitude = LONGITUDE_HEMISPHERE_EAST;
  int degrees_longitude = 123, minutes_longitude_times_1000 = 4567;
  POSITION position;
  refute(get_position(&position) == 0, "position timeout should have been invalid on initialization");
  set_position_latitude(hemisphere_latitude, degrees_latitude, minutes_latitude_times_1000);
  refute(get_position(&position) == 0, "position timeout should have been invalid after only latitude set");
  set_position_longitude(hemisphere_longitude, degrees_longitude, minutes_longitude_times_1000);
  assert_equal_int(0, get_position(&position), "get_position() returned without valid value (timeout expired)");
  assert_position(hemisphere_latitude, degrees_latitude, minutes_latitude_times_1000, hemisphere_longitude, degrees_longitude, minutes_longitude_times_1000, position);
}

void assert_gps_fix_quality_valids(GPS_FIX_QUALITY gps_fix_quality, int signal_quality_valid, int position_error_valid, int satellite_count_valid, int dgps_age_valid, int dgps_station_id_valid) {
  assert_equal_int(signal_quality_valid == 0, gps_fix_quality.signal_quality_valid == 0, "signal_quality_valid");
  assert_equal_int(position_error_valid == 0, gps_fix_quality.position_error_valid == 0, "position_error_valid");
  assert_equal_int(satellite_count_valid == 0, gps_fix_quality.satellite_count_valid == 0, "satellite_count_valid");
  assert_equal_int(dgps_age_valid == 0, gps_fix_quality.dgps_age_valid == 0, "dgps_age_valid");
  assert_equal_int(dgps_station_id_valid == 0, gps_fix_quality.dgps_station_id_valid == 0, "dgps_station_id_valid");
}

TEST(set_and_get_gps_fix_quality)
  int signal_quality_valid = 1, signal_quality = 2, position_error_valid = 3, position_error = 4, antenna_height = 5, satellite_count_valid = 6, satellite_count = 7, geoseparation = 8, dgps_age_valid = 9, dgps_age = 10, dgps_station_id_valid = 11, dgps_station_id = 12;
  GPS_FIX_QUALITY gps_fix_quality;
  refute(get_gps_fix_quality(&gps_fix_quality) == 0, "fix_quality timeout should have been invalid on intialization");
  set_gps_fix_signal_quality(signal_quality);
  assert_equal_int(0, get_gps_fix_quality(&gps_fix_quality), "fix_quality() returned without valid value (timeout expired");
  assert_gps_fix_quality_valids(gps_fix_quality, 1, 0, 0, 0, 0);
  set_gps_fix_position_error(position_error);
  assert_equal_int(0, get_gps_fix_quality(&gps_fix_quality), "fix_quality() returned without valid value (timeout expired");
  assert_gps_fix_quality_valids(gps_fix_quality, 1, 1, 0, 0, 0);
  set_gps_fix_antenna_height(antenna_height);
  assert_equal_int(0, get_gps_fix_quality(&gps_fix_quality), "fix_quality() returned without valid value (timeout expired");
  assert_gps_fix_quality_valids(gps_fix_quality, 1, 1, 0, 0, 0);
  set_gps_fix_satellite_count(satellite_count, geoseparation);
  assert_equal_int(0, get_gps_fix_quality(&gps_fix_quality), "fix_quality() returned without valid value (timeout expired");
  assert_gps_fix_quality_valids(gps_fix_quality, 1, 1, 1, 0, 0);
  set_gps_fix_dgps_age(dgps_age);
  assert_equal_int(0, get_gps_fix_quality(&gps_fix_quality), "fix_quality() returned without valid value (timeout expired");
  assert_gps_fix_quality_valids(gps_fix_quality, 1, 1, 1, 1, 0);
  set_gps_fix_dgps_station_id(dgps_station_id);
  assert_equal_int(0, get_gps_fix_quality(&gps_fix_quality), "fix_quality() returned without valid value (timeout expired");
  assert_gps_fix_quality_valids(gps_fix_quality, 1, 1, 1, 1, 1);
  assert_equal_int(signal_quality, gps_fix_quality.signal_quality, "signal_quality");
  assert_equal_int(position_error, gps_fix_quality.position_error, "position_error");
  assert_equal_int(antenna_height, gps_fix_quality.antenna_height, "antenna_height");
  assert_equal_int(satellite_count, gps_fix_quality.satellite_count, "satellite_count");
  assert_equal_int(geoseparation, gps_fix_quality.geoseparation, "geoseparation");
  assert_equal_int(dgps_age, gps_fix_quality.dgps_age, "dgps_age");
  assert_equal_int(dgps_station_id, gps_fix_quality.dgps_station_id, "dgps_station_id");
}

TEST(set_and_get_navigation_status)
  char waypoint_name[5] = "ZXWV";
  LATITUDE_HEMISPHERE hemisphere_latitude = LATITUDE_HEMISPHERE_SOUTH;
  int degrees_latitude = 12, minutes_latitude_times_1000 = 34567;
  LONGITUDE_HEMISPHERE hemisphere_longitude = LONGITUDE_HEMISPHERE_EAST;
  int degrees_longitude = 123, minutes_longitude_times_1000 = 4567;
  int waypoint_bearing;
  ANGLE_REFERENCE waypoint_bearing_reference;
  int waypoint_range_in_nautical_miles_times_100;
  int cross_track_error_in_nautical_miles_times_100;
  NAVIGATION_STATUS navigation_status;
  refute(get_navigation(&navigation_status) == 0, "navigation timeout should have been invalid on initialization");
  set_navigation_waypoint_name(waypoint_name);
  assert_equal_int(0, get_navigation(&navigation_status), "navigation timeout should have been invalid after waypoint name");
  set_navigation_waypoint_position_latitude(hemisphere_latitude, degrees_latitude, minutes_latitude_times_1000);
  assert_equal_int(0, get_navigation(&navigation_status), "navigation should have been valid after latitude");
  set_navigation_waypoint_position_longitude(hemisphere_longitude, degrees_longitude, minutes_longitude_times_1000);
  assert_equal_int(0, get_navigation(&navigation_status), "navigation should have been valid after longitude");
  set_navigation_waypoint_bearing_and_range_in_nautical_miles_times_100(waypoint_bearing, waypoint_range_in_nautical_miles_times_100);
  assert_equal_int(0, get_navigation(&navigation_status), "navigation should have been valid after waypoint bearing and range");
  set_navigation_waypoint_bearing_reference(waypoint_bearing_reference);
  assert_equal_int(0, get_navigation(&navigation_status), "navigation should have been valid after waypoitn bearnig reference");
  set_navigation_cross_track_error_in_nautical_miles_times_100(cross_track_error_in_nautical_miles_times_100);
  assert_equal_int(0, get_navigation(&navigation_status), "navigation should have been valid after cross track error");
  assert_equal_string(waypoint_name, navigation_status.waypoint_name_last_4, "waypoint_name");
  assert_position(hemisphere_latitude, degrees_latitude, minutes_latitude_times_1000, hemisphere_longitude, degrees_longitude, minutes_longitude_times_1000, navigation_status.waypoint_position);
  assert_equal_int(waypoint_bearing, navigation_status.waypoint_bearing, "waypoint_bearing");
  assert_equal_int(waypoint_bearing_reference, navigation_status.waypoint_bearing_reference, "waypoint_bearing_reference");
  assert_equal_int(waypoint_range_in_nautical_miles_times_100, navigation_status.waypoint_range_in_nautical_miles_times_100, "waypoint_range_in_nautical_miles_times_100");
  assert_equal_int(cross_track_error_in_nautical_miles_times_100, navigation_status.cross_track_error_in_nautical_miles_times_100, "cross_track_error_in_nautical_miles_times_100");
}

TEST(set_and_get_gmt_date_and_time)
  int year = 1778, month = 6, day = 10, hour = 12, minute = 20, second = 37;
  DATE_AND_TIME gmt_date_and_time;
  refute(get_gmt_date_and_time(&gmt_date_and_time) == 0, "gmt_date_and_time timeout should have been invalid on installation");
  set_gmt_date(year, month, day);
  refute(get_gmt_date_and_time(&gmt_date_and_time) == 0, "gmt_date_and_time timeout should have been invalid on installation");
  set_gmt_time(hour, minute, second);
  assert_equal_int(0, get_gmt_date_and_time(&gmt_date_and_time), "gmd_date_and_time should be valid after date and time set");;
  assert_equal_int(year, gmt_date_and_time.year, "year");
  assert_equal_int(month, gmt_date_and_time.month, "month");
  assert_equal_int(day, gmt_date_and_time.day, "day");
  assert_equal_int(hour, gmt_date_and_time.hour, "hour");
  assert_equal_int(minute, gmt_date_and_time.minute, "minute");
  assert_equal_int(second, gmt_date_and_time.second, "second");
}

TEST(set_and_get_autopilot_status)
  int target_heading = 321;
  AUTOPILOT_MODE autopilot_mode = AUTOPILOT_MODE_TRACK;
//  AUTOPILOT_RESPONSE_LEVEL autopilot_response = AUTOPILOT_RESPONSE_LEVEL_MINIMUM_DEADBAND;
  AUTOPILOT_STATUS autopilot_status;
  refute(get_autopilot(&autopilot_status) == 0, "autopilot timeout should have been invalid on initialization");
  set_autopilot_target_heading(target_heading);
  set_autopilot_mode(autopilot_mode);
//  set_autopilot_response_level(autopilot_response);
  assert_equal_int(0, get_autopilot(&autopilot_status), "autopilot should return status after setting values");
  assert_equal_int(target_heading, autopilot_status.target_heading, "target_heading");
  assert_equal_int(autopilot_mode, autopilot_status.mode, "mode");
//  assert_equal_int(autopilot_response, autopilot_status.response_level, "response_level");
}

//autopilot

void test_boat_status() {
  printf("--- Testing boat_status.c\n");
  initialize_status();
  test_set_and_get_heading();
  test_set_and_get_turn_direction();
  test_set_and_get_water_speed_in_knots_times_100();
  test_set_and_get_average_water_speed_in_knots_times_100();
  test_set_and_get_rudder_position_in_degrees_right();
  test_set_and_get_course_over_ground();
  test_set_and_get_speed_over_ground_in_knots_times_100();
  test_set_and_get_trip_mileage_in_nautical_miles_times_100();
  test_set_and_get_total_mileage_in_nautical_miles_times_10();
  test_set_and_get_water_temperature_in_degrees_celsius_times_10();
  test_set_and_get_compass_variation_in_degrees_west();
  test_set_and_get_position();
  test_set_and_get_gps_fix_quality();
  test_set_and_get_navigation_status();
  test_set_and_get_gmt_date_and_time();
  test_set_and_get_autopilot_status();
}
