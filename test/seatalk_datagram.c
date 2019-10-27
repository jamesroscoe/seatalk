#include <stdio.h>
#include <stdlib.h>
#include "test.h"
#include "../seatalk_datagram.c"

#define TEST_BUILD(DATAGRAM_CODE) TEST(build_##DATAGRAM_CODE)\
  char datagram[256];

#define TEST_DATAGRAM(DATAGRAM_CODE) TEST(datagram_##DATAGRAM_CODE)
#define CHECK_INT(NAME) assert_equal_int(NAME ## _in, NAME ## _out, #NAME)

void print_datagram(char *datagram, int length) {
  for (int i = 0; i < length; i++) {
    printf("%.2x ", datagram[i]);
  }
  printf("\n");
}

void assert_depth_below_transducer(int depth_in_feet_times_10_in, int display_in_metres_in, int active_alarms_in, int transducer_defective_in) {
  char datagram[256];
  int depth_in_feet_times_10_out, display_in_metres_out, active_alarms_out, transducer_defective_out;
  assert_equal_int(DATAGRAM_00_LENGTH, build_depth_below_transducer(datagram, depth_in_feet_times_10_in, display_in_metres_in, active_alarms_in, transducer_defective_in), "incorrect datagram length");
  parse_depth_below_transducer(datagram, &depth_in_feet_times_10_out, &display_in_metres_out, &active_alarms_out, &transducer_defective_out);
  CHECK_INT(depth_in_feet_times_10);
//  CHECK_INT(depth_in_feet_times_10);
  CHECK_INT(display_in_metres);
  CHECK_INT(active_alarms);
  CHECK_INT(transducer_defective);
}

TEST_DATAGRAM(00) // depth below transducer
  for (int i = 0; i < 10000; i++) {
    assert_depth_below_transducer(i, 0, 0, 0);
  }
  assert_depth_below_transducer(5, 1, 0, 0);
  assert_depth_below_transducer(5, 0, SHALLOW_WATER_ALARM, 0);
  assert_depth_below_transducer(5, 0, DEEP_WATER_ALARM, 0);
  assert_depth_below_transducer(5, 0, ANCHOR_ALARM, 0);
  assert_depth_below_transducer(5, 0, SHALLOW_WATER_ALARM | DEEP_WATER_ALARM | ANCHOR_ALARM, 0); // unrealistic situation since shallow and deep can't be active together, of course
  assert_depth_below_transducer(5, 0, 0, 1);
}

//TEST_DATAGRAM(01) // equipment id

void assert_engine_rpm_and_pitch(enum ENGINE_ID engine_id_in, int rpm_in, int pitch_percent_in) {
  char datagram[256];
  enum ENGINE_ID engine_id_out;
  int rpm_out, pitch_percent_out;
  assert_equal_int(DATAGRAM_05_LENGTH, build_engine_rpm_and_pitch(datagram, engine_id_in, rpm_in, pitch_percent_in), "incorrect datagram length");
  parse_engine_rpm_and_pitch(datagram, &engine_id_out, &rpm_out, &pitch_percent_out);
  CHECK_INT(engine_id);
  CHECK_INT(rpm);
  CHECK_INT(pitch_percent);
}

TEST_DATAGRAM(05) // engine RPM and pitch
  for (int i = 0; i < 10000; i++) {
    assert_engine_rpm_and_pitch(ENGINE_ID_SINGLE, i, 0);
  }
  assert_engine_rpm_and_pitch(ENGINE_ID_PORT, 1000, 0);
  assert_engine_rpm_and_pitch(ENGINE_ID_STARBOARD, 1000, 0);
  assert_engine_rpm_and_pitch(ENGINE_ID_SINGLE, 1000, 10);
  assert_engine_rpm_and_pitch(ENGINE_ID_SINGLE, 1000, -10);
}

void assert_apparent_wind_angle(int degrees_right_times_2_in) {
  char datagram[256];
  int degrees_right_times_2_out;
  assert_equal_int(DATAGRAM_10_LENGTH, build_apparent_wind_angle(datagram, degrees_right_times_2_in), "incorrect datagram length");
  parse_apparent_wind_angle(datagram, &degrees_right_times_2_out);
  CHECK_INT(degrees_right_times_2);
}

TEST_DATAGRAM(10) // apparent wind angle
  for (int i = -180; i <= 180; i++) {
    for (int j = 0; j <= 1; j++) {
      assert_apparent_wind_angle((2 * i) + j);
    }
  }
}

void assert_apparent_wind_speed(int knots_times_10_in, int display_in_metric_in) {
  char datagram[256];
  int knots_times_10_out, display_in_metric_out;
  assert_equal_int(DATAGRAM_11_LENGTH, build_apparent_wind_speed(datagram, knots_times_10_in, display_in_metric_in), "invalid datagram_length");
  parse_apparent_wind_speed(datagram, &knots_times_10_out, &display_in_metric_out);
  CHECK_INT(knots_times_10);
  CHECK_INT(display_in_metric);
}

TEST_DATAGRAM(11) // apparent wind speed
  for (int i = 0; i <= 100; i++) {
    for (int j = 0; j <= 9; j++) {
      assert_apparent_wind_speed((i * 10) + j, 0);
    }
  }
  assert_apparent_wind_speed(100, 1);
}

void assert_water_speed(int knots_times_10_in) {
  char datagram[256];
  int knots_times_10_out;
  assert_equal_int(DATAGRAM_20_LENGTH, build_water_speed(datagram, knots_times_10_in), "invalid datagram_length");
  parse_water_speed(datagram, &knots_times_10_out);
  CHECK_INT(knots_times_10);
}

TEST_DATAGRAM(20) // water speed
  for (int i = 0; i < 250; i++) {
    for (int j = 0; j <= 0; j++) {
      assert_water_speed((i * 10) + j);
    }
  }
}

void assert_trip_mileage(int mileage_times_100_in) {
  char datagram[256];
  int mileage_times_100_out;
  assert_equal_int(DATAGRAM_21_LENGTH, build_trip_mileage(datagram, mileage_times_100_in), "incorrect datagram length");
  parse_trip_mileage(datagram, &mileage_times_100_out);
  CHECK_INT(mileage_times_100);
}

TEST_DATAGRAM(21) // trip mileage
  for (int i = 0; i <= 10484 / 100; i += 101) {
    for (int j = 0; j <= 99; j++) {
      assert_trip_mileage((i * 100) + j);
    }
  }
}

void assert_total_mileage(int mileage_times_10_in) {
  char datagram[256];
  int mileage_times_10_out;
  assert_equal_int(DATAGRAM_22_LENGTH, build_total_mileage(datagram, mileage_times_10_in), "incorrect_datagram_length");
  parse_total_mileage(datagram, &mileage_times_10_out);
  CHECK_INT(mileage_times_10);
}

TEST_DATAGRAM(22) // total mileage
  for (int i = 0; i <= 0xfff0 / 10; i++) {
    for (int j = 0; j <= 9; j++) {
      assert_total_mileage((i * 10) + j);
    }
  }
}

void assert_water_temperature(int degrees_fahrenheit_in, int transducer_defective_in) {
  char datagram[256];
  int degrees_fahrenheit_out, transducer_defective_out;
  assert_equal_int(DATAGRAM_23_LENGTH, build_water_temperature(datagram, degrees_fahrenheit_in, transducer_defective_in), "incorrect datagram length");
  parse_water_temperature(datagram, &degrees_fahrenheit_out, &transducer_defective_out);
  CHECK_INT(degrees_fahrenheit);
  CHECK_INT(transducer_defective);
}

TEST_DATAGRAM(23) // water temperature
  for (int i = 0; i <= 255; i++) {
    assert_water_temperature(i, 0);
  }
  assert_water_temperature(75, 1);
}

void assert_speed_distance_units(enum DISTANCE_UNITS distance_units_in) {
  char datagram[256];
  enum DISTANCE_UNITS distance_units_out;
  assert_equal_int(DATAGRAM_24_LENGTH, build_speed_distance_units(datagram, distance_units_in), "incorrect datagram length");
  parse_speed_distance_units(datagram, &distance_units_out);
  CHECK_INT(distance_units);
}

TEST_DATAGRAM(24) // display units for mileage and speed
  assert_speed_distance_units(DISTANCE_UNITS_NAUTICAL);
  assert_speed_distance_units(DISTANCE_UNITS_STATUTE);
  assert_speed_distance_units(DISTANCE_UNITS_METRIC);
}

void assert_total_and_trip_mileage(int total_nm_times_10_in, int trip_nm_times_100_in) {
  char datagram[256];
  int total_nm_times_10_out, trip_nm_times_100_out;
  assert_equal_int(DATAGRAM_25_LENGTH, build_total_and_trip_mileage(datagram, total_nm_times_10_in, trip_nm_times_100_in), "incorrect datagram length");
  parse_total_and_trip_mileage(datagram, &total_nm_times_10_out, &trip_nm_times_100_out);
  CHECK_INT(total_nm_times_10);
  CHECK_INT(trip_nm_times_100);
}

TEST_DATAGRAM(25) // total and trip mileage
  for (int i = 0; i < 104856; i += 101) {
    for (int j = 0; j <= 9; j++) {
      assert_total_and_trip_mileage((i * 10) + j, 0);
    }
  }
  for (int i = 0; i < 10484; i += 97) {
    for (int j = 0; j <= 99; j++) {
      assert_total_and_trip_mileage(0, (i * 100) + j);
    }
  }
}

void assert_average_water_speed(int knots_1_times_100_in, int knots_2_times_100_in, int speed_1_from_sensor_in, int speed_2_is_average_in, int average_is_stopped_in, int display_in_statute_miles_in) {
  char datagram[256];
  int knots_1_times_100_out, knots_2_times_100_out, speed_1_from_sensor_out, speed_2_is_average_out, average_is_stopped_out, display_in_statute_miles_out;
  assert_equal_int(DATAGRAM_26_LENGTH, build_average_water_speed(datagram, knots_1_times_100_in, knots_2_times_100_in, speed_1_from_sensor_in, speed_2_is_average_in, average_is_stopped_in, display_in_statute_miles_in), "incorrect datagram length");
  parse_average_water_speed(datagram, &knots_1_times_100_out, &knots_2_times_100_out, &speed_1_from_sensor_out, &speed_2_is_average_out, &average_is_stopped_out, &display_in_statute_miles_out);
  CHECK_INT(knots_1_times_100);
  CHECK_INT(knots_2_times_100);
  CHECK_INT(speed_1_from_sensor);
  CHECK_INT(speed_2_is_average);
  CHECK_INT(average_is_stopped);
  CHECK_INT(display_in_statute_miles);
}

TEST_DATAGRAM(26) // water speed for two sensors or with average
  for (int i = 0; i < 65000; i += 101) {
    assert_average_water_speed(i, 600, 1, 1, 0, 0);
    assert_average_water_speed(600, i, 1, 1, 0, 0);
  }
  assert_average_water_speed(600, 600, 0, 0, 1, 1);
}

void assert_precise_water_temperature(int degrees_celsius_times_10_in) {
  char datagram[256];
  int degrees_celsius_times_10_out;
  assert_equal_int(DATAGRAM_27_LENGTH, build_precise_water_temperature(datagram, degrees_celsius_times_10_in), "incorrect datagram length");
  parse_precise_water_temperature(datagram, &degrees_celsius_times_10_out);
  CHECK_INT(degrees_celsius_times_10);
}

TEST_DATAGRAM(27) // water temperature precise to 0.1 degrees C
  for (int i = 0; i <= 650; i += 101) {
    for (int j = 0; j < 100; j++) {
      assert_precise_water_temperature((i * 100) + j);
    }
  }
}

void assert_lamp_intensity(int intensity_in) {
  char datagram[256];
  int intensity_out;
  assert_equal_int(DATAGRAM_30_LENGTH, build_lamp_intensity(datagram, intensity_in), "incorrect datagram length");
  parse_lamp_intensity(datagram, &intensity_out);
  CHECK_INT(intensity);
}

TEST_DATAGRAM(30) // set lamp intensity
  for (int i = 0; i <= 3; i++) {
    assert_lamp_intensity(i);
  }
}

void assert_lat_position(int degrees_in, int minutes_times_100_in) {
  char datagram[256];
  int degrees_out, minutes_times_100_out;
  assert_equal_int(DATAGRAM_50_LENGTH, build_lat_position(datagram, degrees_in, minutes_times_100_in), "incorrect datagram length");
  parse_lat_position(datagram, &degrees_out, &minutes_times_100_out);
  CHECK_INT(degrees);
  CHECK_INT(minutes_times_100);
}

TEST_DATAGRAM(50) // latitude
  for (int i = -89; i < 90; i += 5) {
    for (int j = 0; j < 6000; j += 37) {
      assert_lat_position(i, j);
    }
  }
}

void assert_lon_position(int degrees_in, int minutes_times_100_in) {
  char datagram[256];
  int degrees_out, minutes_times_100_out;
  assert_equal_int(DATAGRAM_51_LENGTH, build_lon_position(datagram, degrees_in, minutes_times_100_in), "incorrect datagram length");
  parse_lon_position(datagram, &degrees_out, &minutes_times_100_out);
  CHECK_INT(degrees);
  CHECK_INT(minutes_times_100);
}

TEST_DATAGRAM(51) // longitude
  for (int i = -179; i < 180; i += 5) {
    for (int j = 6000; j <= 6000; j += 37) {
      assert_lon_position(i, j);
    }
  }
}

void assert_speed_over_ground(int knots_times_10_in) {
  char datagram[256];
  int knots_times_10_out;
  assert_equal_int(DATAGRAM_52_LENGTH, build_speed_over_ground(datagram, knots_times_10_in), "incorrect datagram length");
  parse_speed_over_ground(datagram, &knots_times_10_out);
  CHECK_INT(knots_times_10);
}

TEST_DATAGRAM(52) // speed over ground
  for (int i = 0; i <= 6500; i += 101) {
    for (int j = 0; j <= 9; j++) {
      assert_speed_over_ground((i * 10) + j);
    }
  }
}

void assert_course_over_ground(int degrees_in) {
  char datagram[256];
  int degrees_out;
  assert_equal_int(DATAGRAM_53_LENGTH, build_course_over_ground(datagram, degrees_in), "incorrect datagram length");
  parse_course_over_ground(datagram, &degrees_out);
  CHECK_INT(degrees);
}

TEST_DATAGRAM(53) // course over ground
  for (int i = 0; i < 360; i++) {
    assert_course_over_ground(i);
  }
}

TEST(parse_84)
  // 84: compass heading, autopilot course and rudder position
  // sample message states compass = 016
  // autopilot not turning
  // autopilot course 000
  // autopilot in standby mode
  // no alarms
  // rudder position on centre
  // no display instructions
  // 0x0f in final byte meaning unknown; signal copied from Autohelm 1000+
  struct AUTOPILOT_STATUS ap_status;
  handle_seatalk_datagram("\x84\xc6\x07\x00\x00\x00\x00\x00\x0f");
  get_autopilot_status(&ap_status);
  assert_equal_int(16, ap_status.compass_heading,
    "ap_status.compass_heading");
  assert_equal_string("right", ap_status.turning_direction,
    "ap_status.turning_direction");
  assert_equal_int(0, ap_status.target_heading, "ap_status.target");
  assert_equal_string("standby", ap_status.mode, "ap_status.mode");
  assert_equal_int(0, ap_status.off_course_alarm,
    "ap_status.off_course_alarm");
  assert_equal_int(0, ap_status.wind_shift_alarm,
    "ap_status.wind_shift_alarm");
  assert_equal_int(0, ap_status.rudder_position,
    "ap_status.rudder_position");
  assert_equal_int(0, ap_status.heading_display_off,
    "ap_status.heading_display_off");
  assert_equal_int(0, ap_status.no_data, "ap_status.no_data");
  assert_equal_int(0, ap_status.large_xte, "ap_status.large_xte");
  assert_equal_int(0, ap_status.auto_rel, "ap_status.auto_rel");
  // printf("ap_status: %s", buf);
}

TEST(parse_9c)
  // 9c: compass heading and rudder position
  // sample message states compass 0 * 90 + 07 * 2 + 2 = 016
  // rudder position 00
  handle_seatalk_datagram("\x9c\xc1\x07\x00");
}

TEST_BUILD(89)
  // 89: compass heading
  for (int i = 0; i <= 359; i++) {
    int length = build_compass_datagram(datagram, i);
    assert_equal_int(5, length, "datagram should be 5 characters");
//    printf("%i: ", i);
//    print_datagram(datagram, length);
    int u = first_nibble(datagram[1]);
    int vw = datagram[2];
//    printf("u: %x, vw: %x\n", u, vw);
//    printf("(u & 0x3) * 90: %d\n", (u & 0x3) * 90);
//    printf("(vw & 0x3f) * 2: %d\n", (vw & 0x3f) * 2);
//    printf("((u & 0xc) >> 2 (%d)) / 2: %d\n", (u & 0xc) >> 2, ((u & 0xc) >> 2) / 2);
    assert_equal_int(i, ((datagram[1] >> 4) & 0x3) * 90 +  (datagram[2] & 0x3f) * 2 + (datagram[1] >> 6) / 2, "datagram compass value");
  }
}

void test_seatalk_datagram() {
  printf("---Testing seatalk_datagram.c\n");
  test_datagram_00();
  test_datagram_05();
  test_datagram_10();
  test_datagram_11();
  test_datagram_20();
  test_datagram_21();
  test_datagram_22();
  test_datagram_23();
  test_datagram_24();
  test_datagram_25();
  test_datagram_26();
  test_datagram_27();
  test_datagram_30();
  // test_datagram_36(); // this datagram has no variability
  test_datagram_50();
  test_datagram_51();
  test_datagram_52();
  test_datagram_53();
  test_parse_84();
  test_parse_9c();
  test_build_89();
}
