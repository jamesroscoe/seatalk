#include <stdio.h>
#include <stdlib.h>
#include "test.h"
#include "../seatalk_datagram.c"

#define TEST_BUILD(DATAGRAM_CODE) TEST(build_##DATAGRAM_CODE)\
  char datagram[256];
void print_datagram(char *datagram, int length) {
  for (int i = 0; i < length; i++) {
    printf("%.2x ", datagram[i]);
  }
  printf("\n");
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
  parse_seatalk_datagram("\x84\xc6\x07\x00\x00\x00\x00\x00\x0f");
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
  parse_seatalk_datagram("\x9c\xc1\x07\x00");
}

TEST_BUILD(00)
  // 00: depth below transducer
  // note: limited to whole number feet due to kernel mode restrictions
  //       on using floating point numbers
  int length = build_depth_datagram(datagram, 1000, 0, 0, 0, 0);
  assert_equal_int(5, length, "datagram should be 5 characters");
  int xxxx = datagram[3] * 0x100 + datagram[4];
  assert_equal_int(100, (int)(xxxx / 10), "depth should be reported as 100'");
  length = build_depth_datagram(datagram, 1000, 1, 0, 0, 0);
  int y;
  y = first_nibble(datagram[2]);
  assert(y & 0x8, "anchor alarm should be active");
  length = build_depth_datagram(datagram, 1000, 0, 1, 0, 0);
  y = first_nibble(datagram[2]);
  assert(y & 0x4, "metric display should be active");
  length = build_depth_datagram(datagram, 1000, 0, 0, 1, 0);
  int z;
  z = last_nibble(datagram[2]);
  assert(z & 0x2, "deep water alarm should be active");
  length = build_depth_datagram(datagram, 1000, 0, 0, 0, 1);
  z = last_nibble(datagram[2]);
  assert(z & 0x1, "shallow water alarm should be active");
}

TEST_BUILD(05)
  // 05: engine RPM and pitch
  int length = build_engine_rpm_datagram(datagram, 3400, 3, -1); // single engine
  assert_equal_int(6, length, "datagram should be 6 characters");
  int x,  yyzz, pp;
  x = datagram[2];
  yyzz = (datagram[3] << 8) | datagram[4];
  pp = datagram[5];
  assert_equal_int(0, x, "should be single-engine");
  assert_equal_int(3400, yyzz, "reported RPM should be 3400");
  assert_equal_int(3, pp, "reported pitch should be 3");
  length = build_engine_rpm_datagram(datagram, 3400, 3, 0); // port engine
  x = datagram[2];
  assert_equal_int(2, x, "should be port engine");
  length = build_engine_rpm_datagram(datagram, 3400, 3, 1); // starboard engine
  x = datagram[2];
  assert_equal_int(1, x, "should be starboard engine");
  length = build_engine_rpm_datagram(datagram, -3400, -3, -1); // -ve rpm+pitch
  yyzz = (datagram[3] << 8) | datagram[4];
  pp = datagram[5];
  if (yyzz >= 0x8000) {
    yyzz -= 0x10000;
  }
  if (pp >= 0x80) {
    pp -= 0x100;
  }
  assert_equal_int(-3400, yyzz, "reported RPM should be negative");
  assert_equal_int(-3, pp, "reported pitch should be negative");
}

TEST_BUILD(10)
  // 10 apparent wind angle
  for (int i = -360; i <= 360; i++) {
    int length = build_apparent_wind_angle_datagram(datagram, i);
    assert_equal_int(length, 4, "datagram should be 4 characters");
    int xxyy = (datagram[2] << 8) | datagram[3];;
    if (xxyy >= 0x8000) {
      xxyy -= 0x10000;
    }
    assert_equal_int((int)(i / 2), (int)(xxyy / 2), "apparent wind angle");
  }
}

TEST_BUILD(11)
  // 11 apparent wind speed
  for (int i = 0; i <= 1279; i++) {
    int length = build_apparent_wind_speed_datagram(datagram, i, 0);
    assert_equal_int(4, length, "datagram should be 4 characters");
    int xx = datagram[2] & 0x7f;
    int y = last_nibble(datagram[3]);
    assert_equal_float((float)i/10, (float)xx + y/10.0, "apparent wind speed");
  }
  int length = build_apparent_wind_speed_datagram(datagram, 5.5, 1);
  int xx = datagram[2];
  assert(xx & 0x80, "metric display flag should be set");
}

TEST_BUILD(20)
  // 20 speed through water
  for (int i = 0; i <= 9999; i++) {
    int length = build_water_speed_datagram(datagram, i);
    assert_equal_int(4, length, "datagram should be 4 characters");
    int xxxx = (datagram[2] << 8) + datagram[3];
    assert_equal_float((float)(i / 10.0), (float)(xxxx / 10.0), "water speed");
  }
}

TEST_BUILD(21)
  // 21 trip mileage
  for (int i = 0; (i < 10000); i++) {
    int length = build_trip_mileage_datagram(datagram, i);
    assert_equal_int(5, length, "datagram should be 5 characters");
    int xxxxx = datagram[2] << 12 | datagram[3] << 4 | datagram[4];
    assert_equal_float((float)(i / 100.0), (float)(xxxxx / 100.0), "mileage");
  }
}

TEST_BUILD(22)
  // 22 total mileage
  for (int i = 0; (i < 10000); i++) {
    int length = build_total_mileage_datagram(datagram, i);
    assert_equal_int(5, length, "datagram should be 5 characters");
    int xxxx = datagram[2] << 8 | datagram[3];
    assert_equal_float((float)(i / 10.0), (float)(xxxx / 10.0), "mileage");
  }
}

TEST_BUILD(23)
  // 23 water temperature
  for (int i = 32; i <= 128; i++) {
    int length = build_water_temperature_datagram(datagram, i);
    assert_equal_int(4, length, "datagram should be 4 characters");
    int xx = datagram[2];
    int yy = datagram[3];
    int z = first_nibble(datagram[1]);
    assert_equal_int((int)(((i - 32) * 5) / 9), xx, "Celsius");
    assert_equal_int(i, yy, "Fahrenheit");
    assert_equal_int(0, z, "flags");
  }
}

TEST_BUILD(24)
  // 24 display units for mileage and speed
  int length = build_distance_units_datagram(datagram, -1);
  assert_equal_int(5, length, "datagram should be 5 characters");
  int xx = datagram[4];
  assert_equal_int(0x6, xx, "statute miles");
  build_distance_units_datagram(datagram, 0);
  xx = datagram[4];
  assert_equal_int(0x0, xx, "nautical miles");
  build_distance_units_datagram(datagram, 1);
  xx = datagram[4];
  assert_equal_int(0x86, xx, "km");
}

TEST_BUILD(25)
  // 25 total and trip log
  for (int i = 0; i < 1048575; i += 111) {
    int length = build_total_and_trip_log_datagram(datagram, i, i);
    assert_equal_int(7, length, "datagram should be 7 characters");
    int xx, yy, z, uu, vv, w;
    xx = datagram[2];
    yy = datagram[3];
    z = first_nibble(datagram[1]);
    uu = datagram[4];
    vv = datagram[5];
    w = last_nibble(datagram[6]);
    assert_equal_float((float)(i / 10.0), (float)(((z << 16) | (yy << 8) | xx) / 10.0), "total");
    assert_equal_float((float)(i / 100.0), (float)(((w << 16) | (vv << 8) | uu) / 100.0), "trip");
  }
}

TEST_BUILD(26)
  // 26 average water speed
  for (int i = 0; i <= 1000; i++) {
    int length = build_average_water_speed_datagram(datagram, i, i, 1, 1, 0, 0);
    assert_equal_int(7, length, "datagram should be 7 characters");
    int xxxx = (datagram[2] << 8) | datagram[3];
    int yyyy = (datagram[4] << 8) | datagram[5];
    int d = first_nibble(datagram[6]);
    int e = last_nibble(datagram[6]);
    assert_equal_float((float)(i / 100.0), (float)(xxxx / 100.0), "current speed");
    assert_equal_float((float)(i / 100.0), (float)(yyyy / 100.0), "average speed");
    assert(d & 0x4, "current speed sensor");
    assert(d & 0x8, "average speed sensor");
    refute(e & 0x1, "stopped");
    refute(e & 0x2, "display in statute miles");
  }
}
TEST_BUILD(27)
  // water temperature 2
  for (int i = -100; i <= 155; i++) {
    int length = build_water_temperature2_datagram(datagram, i);
    assert_equal_int(4, length, "datagram should be 4 characters");
    int xxxx = (datagram[2] << 8) | datagram[3];
    assert_equal_float((float)(i / 10.0), (float)((xxxx - 100) / 10.0), "temperature");
  }
}

TEST_BUILD(30)
  // set lamp intensity
  int length = build_set_lamp_intensity_datagram(datagram, 0);
  assert_equal_int(3, length, "datagram should be 3 characters");
  int x = datagram[2];
  assert_equal_int(0x0, x, "level 0");
  build_set_lamp_intensity_datagram(datagram, 1);
  x = datagram[2];
  assert_equal_int(0x4, x, "level 1");
  build_set_lamp_intensity_datagram(datagram, 1);
  x = datagram[2];
  assert_equal_int(0x4, x, "level 2");
  build_set_lamp_intensity_datagram(datagram, 2);
  x = datagram[2];
  assert_equal_int(0x8, x, "level 2");
  for (int i = 3; i <= 6; i++) {
    build_set_lamp_intensity_datagram(datagram, i);
    x = datagram[2];
    assert_equal_int(0xc, x, "level 3");
  }
}

TEST_BUILD(36)
  // cancel MOB
  int length = build_cancel_mob_datagram(datagram);
  assert_equal_int(3, length, "datagram should be 3 characters");
  int x = datagram[2];
  assert_equal_int(1, x, "should be 1");
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
  test_parse_84();
  test_parse_9c();
  test_build_00();
  test_build_05();
  test_build_10();
  test_build_11();
  test_build_20();
  test_build_21();
  test_build_22();
  test_build_23();
  test_build_24();
  test_build_25();
  test_build_26();
  test_build_27();
  test_build_30();
  test_build_36();
  test_build_89();
}
