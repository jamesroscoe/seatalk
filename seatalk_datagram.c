#include "seatalk_datagram.h"
#include "seatalk_state.h"

#define INITIALIZE_DATAGRAM(DATAGRAM_NUMBER, HIGH_NIBBLE_OF_SECOND_BYTE) initialize_datagram(datagram, 0x##DATAGRAM_NUMBER, DATAGRAM_##DATAGRAM_NUMBER##_LENGTH, HIGH_NIBBLE_OF_SECOND_BYTE)

#define DATAGRAM_00_LENGTH 5
#define DATAGRAM_05_LENGTH 6
#define DATAGRAM_10_LENGTH 4
#define DATAGRAM_11_LENGTH 4
#define DATAGRAM_20_LENGTH 4
#define DATAGRAM_21_LENGTH 5
#define DATAGRAM_22_LENGTH 5
#define DATAGRAM_23_LENGTH 4
#define DATAGRAM_24_LENGTH 5
#define DATAGRAM_25_LENGTH 7
#define DATAGRAM_26_LENGTH 7
#define DATAGRAM_27_LENGTH 4
#define DATAGRAM_30_LENGTH 3
#define DATAGRAM_36_LENGTH 3
#define DATAGRAM_38_LENGTH 4
#define DATAGRAM_50_LENGTH 5
#define DATAGRAM_51_LENGTH 5
#define DATAGRAM_52_LENGTH 4
#define DATAGRAM_53_LENGTH 3
#define DATAGRAM_54_LENGTH 4
#define DATAGRAM_55_LENGTH 4
#define DATAGRAM_56_LENGTH 4
#define DATAGRAM_57_LENGTH 3
#define DATAGRAM_58_LENGTH 8
#define DATAGRAM_59_LENGTH 5
#define DATAGRAM_61_LENGTH 6
#define DATAGRAM_65_LENGTH 3
#define DATAGRAM_66_LENGTH 3
#define DATAGRAM_68_LENGTH 4
#define DATAGRAM_6C_LENGTH 8
#define DATAGRAM_6E_LENGTH 10
#define DATAGRAM_70_LENGTH 3
#define DATAGRAM_80_LENGTH 3
#define DATAGRAM_81_LENGTH 4
#define DATAGRAM_81_LENGTH_ALTERNATE 3
#define DATAGRAM_82_LENGTH 8
#define DATAGRAM_83_LENGTH 10
#define DATAGRAM_84_LENGTH 9
#define DATAGRAM_85_LENGTH 9
#define DATAGRAM_86_LENGTH 4
#define DATAGRAM_87_LENGTH 3
#define DATAGRAM_88_LENGTH 6
#define DATAGRAM_89_LENGTH 5
#define DATAGRAM_90_LENGTH 5
#define DATAGRAM_91_LENGTH 3
#define DATAGRAM_92_LENGTH 5
#define DATAGRAM_93_LENGTH 3
#define DATAGRAM_95_LENGTH 9
#define DATAGRAM_99_LENGTH 3
#define DATAGRAM_9A_LENGTH 12
#define DATAGRAM_9C_LENGTH 4
#define DATAGRAM_9E_LENGTH 15
#define DATAGRAM_A1_LENGTH 16
#define DATAGRAM_A2_LENGTH 7
#define DATAGRAM_A4_LENGTH 5
#define DATAGRAM_A4_LENGTH_ALTERNATE 9
#define DATAGRAM_A5_LENGTH 10
#define DATAGRAM_A5_LENGTH_ALTERNATE 4
#define DATAGRAM_A5_LENGTH_ALTERNATE2 7
#define DATAGRAM_A5_LENGTH_ALTERNATE3 16
#define DATAGRAM_A7_LENGTH 12
#define DATAGRAM_A8_LENGTH 6
#define DATAGRAM_AB_LENGTH 6

char first_nibble(char c) {
  return (c & 0xf0) >> 4;
}

char last_nibble(char c) {
  return (c & 0x0f);
}

void flag(int *output, char c, char m) {
  if (c & m) {
    *output = 1;
  } else {
    *output = 0;
  }
}

int get_datagram_length(char second_byte) {
  return last_nibble(second_byte);
}

void parse_autopilot_status(char *datagram) {
  char u, vw, xy, z, m, rr, ss, tt;
  int compass_heading, turning_direction, target_heading, mode, off_course_alarm, wind_shift_alarm, rudder_position, heading_display_off, no_data, large_xte, auto_rel;

  // printf("parse_autopilot_status\n");
  u = first_nibble(datagram[1]);
  vw = datagram[2];
  xy = datagram[3];
  z = last_nibble(datagram[4]);
  m = last_nibble(datagram[5]);
  rr = datagram[6];
  ss = datagram[7];
  tt = datagram[8];

  compass_heading = ((u & 0x03) * 90) + ((vw & 0x3f) * 2) + ((u & 0x0c) ? (((u & 0x0c) == 0x0c) ? 2 : 1) : 0);
  turning_direction = (u & 0x08) ? 1 : -1;
  target_heading = ((vw & 0xc0) >> 6 * 90) + (xy / 2);
  if ((z & 0x02) == 0) {
    mode = 0;
  } else if (z & 0x02) {
    mode = 1;
  } else if (z & 0x04) {
    mode = 2;
  } else {
    mode = 3;
  }
  flag(&off_course_alarm, m, 0x04);
  flag(&wind_shift_alarm, m, 0x08);
  if (rr & 0xf0) {
    rudder_position = -((rr ^ 0xff) + 1);
  } else {
    rudder_position = rr;
  }
  flag(&heading_display_off, ss, 0x01);
  flag(&no_data, ss, 0x08);
  flag(&large_xte, ss, 0x10);
  flag(&auto_rel, ss, 0x80);
  set_autopilot_status(compass_heading, turning_direction, target_heading, mode, off_course_alarm, wind_shift_alarm, rudder_position, heading_display_off, no_data, large_xte, auto_rel);
}

// parse datagram
void parse_seatalk_datagram(char *datagram) {
  // printf("parse_seatalk_datagram: %x", datagram[0]);
  switch (datagram[0]) {
    case 0x00: // depth below transducer
      break;
    case 0x01: // equipment ID
      break;
    case 0x05: // engine RPM and blade pitch
      break;
    case 0x10: // apparent wind angle
      break;
    case 0x11: // apparent wind speed
      break;
    case 0x20: // water speed xxxx/10 kt
      break;
    case 0x21: // trip mileage xxxxx/100 nm
      break;
    case 0x22: // trip mileage xxxx/10 nm
      break;
    case 0x23: // water temperature
      break;
    case 0x24: // display units for mileage & speed
      break;
    case 0x25: // total & trip log
      break;
    case 0x26: // water speed with average
      break;
    case 0x27: // water teperature (xxxx-100)/10 degrees C break;
      break;
    case 0x30: // set lamp intensity
      break;
    case 0x36: // cancel MOB
      break;
    case 0x38: // codelock data
      break;
    case 0x50: // lat position
      break;
    case 0x51: // lon position
      break;
    case 0x52: // speed over ground
      break;
    case 0x53: // course over ground
      break;
    case 0x54: // gmt time
      break;
    case 0x55: // TRACK keystroke on GPS unit
      break;
    case 0x56: // date
      break;
    case 0x57: // satellite status
      break;
    case 0x58: // lat/lon
      break;
    case 0x59: // set countdown timer
      break;
    case 0x61: // e-80 unit initialization broadcast
      break;
    case 0x65: // select fathom or feed depth display units
      break;
    case 0x66: // wind alarm
      break;
    case 0x68: // alarm acknowledgement keystroke
      break;
    case 0x6c: // second equipment id message
      break;
    case 0x6e: // MOB keystroke
      break;
    case 0x70: // ST60 remote control keystroke
      break;
    case 0x80: // set lamp intensity
      break;
    case 0x81: // set by course computer during setup when going past user cal
      break;
    case 0x82: // target waypoint name
      break;
    case 0x83: // course computer powered up or cleared failure condition
      break;
    case 0x84: // autopilot status broadcast
      parse_autopilot_status(datagram);
      break;
    case 0x85: // navigation to waypoint status
      break;
    case 0x86: // autopilot command
      break;
    case 0x87: // set autopilot response level
      break;
    case 0x88: // autopilot parameter
      break;
    case 0x89: // steering compass course
      break;
    case 0x90: // device identification
      break;
    case 0x91: // set rudder gain
      break;
    case 0x92: // set autopilot parameter
      break;
    case 0x93: // enter autopilot setup
      break;
    case 0x95: // replaces 84 while autpilot is in value setting mode
      break;
    case 0x99: // compass variation sent by st40 or autopilot
      break;
    case 0x9a: // version string
      break;
    case 0x9c: // compass heading and rudder position
      break;
    case 0x9e: // waypoint definition
      break;
    case 0xa1: // destination waypoint info
      break;
    case 0xa2: // waypoint arrival notice
      break;
    case 0xa4: // broadcast query to identify all devices on the bus
      break;
    case 0xa5: // GPS and DGPS info
      break;
    case 0xa7: // unknown
      break;
    case 0xa8: // alarm notice for guard #1 or guard #2
      break;
    case 0xab: // alarm notice for guard #1 or guard #2
      break;
  }
}
void initialize_datagram(char *datagram, int datagram_number, int total_length, int high_nibble_of_second_byte) {
  int i;
  datagram[0] = datagram_number;
  datagram[1] = (total_length - 3) + (high_nibble_of_second_byte << 4);
  for (i = 2; i < total_length; i++) { datagram[i] = 0; }
}

void uvw_compass(int *u, int *vw, int compass) {
  int temp_compass = compass;
  *u = (int)(temp_compass / 90);
  temp_compass = temp_compass - (*u * 90);
  *vw = (int)(temp_compass / 2);
  temp_compass -= *vw * 2;
  *u |= (temp_compass * 2) << 2;
}

int build_depth_datagram(char *datagram, int depth_in_feet_times_10, int anchor_alarm, int display_in_metres, int deep_water_alarm, int shallow_water_alarm) {
  // 00 02 yz xx xx
  // depth below transducer (in feet): xxxx/10
  // anchor alarm: y & 0x8
  // metric display: y & 4
  // deep water alarm: z & 2
  // shallow water alarm: z & 1
  int xxxx = depth_in_feet_times_10;
  int y = (anchor_alarm ? 0x8 : 0x0) | (display_in_metres ? 0x4 : 0x0);
  int z = (deep_water_alarm ? 0x2 : 0x0) | (shallow_water_alarm ? 0x1 : 0x0);
  INITIALIZE_DATAGRAM(00, 0);
  datagram[2] = (y << 4) | z;
  datagram[3] = xxxx >> 8;
  datagram[4] = xxxx & 0xff;
  return DATAGRAM_00_LENGTH;
}

int build_engine_rpm_datagram(char *datagram, int rpm, int pitch, int starboard_indicator) {
  // 05 03 0x yy zz pp
  // x = 0: single engine (supply starboard_indicator as -1)
  // x = 1: starboard engine (supply staboard_indicator as 1)
  // x = 2: port engine (supply starboard_indicator as 0)
  // rpm: yy * 256 + zz (signed value)
  // pp: prop pitch, signed value
  int x, yy, zz, pp;
  switch (starboard_indicator) {
    case -1:
      x = 0;
      break;
    case 0:
      x = 2;
      break;
    default:
      x = 1;
  }
  yy = (int)(rpm >> 8);
  zz = rpm & 0xff;;
  pp = pitch;
  INITIALIZE_DATAGRAM(05, 0);
  datagram[2] = x;
  datagram[3] = yy;
  datagram[4] = zz;
  datagram[5] = pp;
  return DATAGRAM_05_LENGTH;
}

int build_apparent_wind_angle_datagram(char *datagram, int degrees_times_2) {
  // 10 01 xx yy
  // xxyy/2 = degrees right of bow
  int xx, yy;
  xx = degrees_times_2 >> 8;
  yy = degrees_times_2 & 0xff;
  INITIALIZE_DATAGRAM(10, 0);
  datagram[2] = xx;
  datagram[3] = yy;
  return DATAGRAM_10_LENGTH;
}

int build_apparent_wind_speed_datagram(char *datagram, int knots_times_10, int metric_flag) {
  // 11 01 xx 0y
  // wind speed: (xx & 0x7f) + y/10
  // units flag xx & 0x80; 0 knots/0x80 m/s
  int xx, y;
  xx = (knots_times_10 / 10);
  y = knots_times_10 % 10;
  if (xx > 0x7f) {
    xx = 0x7f;
  }
  if (metric_flag) {
    xx |= 0x80;
  }
  INITIALIZE_DATAGRAM(11, 0);
  datagram[2] = xx;
  datagram[3] = y;
  return DATAGRAM_11_LENGTH;
}

int build_water_speed_datagram(char *datagram, int knots_times_10) {
  // 20 01 xx xx
  // water speed = xxxx/10 knots
  int xx1, xx2;
  xx1 = knots_times_10 >> 8;
  xx2 = knots_times_10 & 0xff;
  INITIALIZE_DATAGRAM(20, 0);
  datagram[2] = xx1;
  datagram[3] = xx2;
  return DATAGRAM_20_LENGTH;
}

int build_trip_mileage_datagram(char *datagram, int nmiles_times_100) {
  // 21 02 x xx 0x
  // mileage = xxxxx/100 nautical  miles
  int xx1, xx2, x;
  xx1 = nmiles_times_100 >> 12;
  xx2 = nmiles_times_100 >> 4;
  x = nmiles_times_100 & 0x0f;
  INITIALIZE_DATAGRAM(21, 0);
  datagram[2] = xx1;
  datagram[3] = xx2;
  datagram[4] = x;
  return DATAGRAM_21_LENGTH;
}

int build_total_mileage_datagram(char *datagram, int nmiles_times_10) {
  // 22 02 x xx 0x
  // mileage = xxxx/10 nautical  miles
  int xx1, xx2;
  xx1 = nmiles_times_10 >> 8;
  xx2 = nmiles_times_10 & 0xff;
  INITIALIZE_DATAGRAM(22, 0);
  datagram[2] = xx1;
  datagram[3] = xx2;
  datagram[4] = 0;
  return DATAGRAM_22_LENGTH;
}

int build_water_temperature_datagram(char *datagram, int fahrenheit_degrees) {
  // 23 z1 xx yy
  // z & 0x4 sensor problem
  // xx degrees Celsius, yy degrees Fahrenheit
  int xx, yy, z;
  xx = (int)((fahrenheit_degrees - 32) * 5 / 9);
  yy = fahrenheit_degrees;
  z = 0;
  INITIALIZE_DATAGRAM(23, z);
  datagram[2] = xx;
  datagram[3] = yy;
  return DATAGRAM_23_LENGTH;
}

int build_distance_units_datagram(char *datagram, int nautical_zero_statute_negative_metric_positive) {
  // 24 02 00 00 xx
  // xx == 00 nm/kts; 06 sm/mph; 86 km/kph
  int xx;
  if (nautical_zero_statute_negative_metric_positive == 0) {
    xx = 0;
  } else if (nautical_zero_statute_negative_metric_positive < 0) {
    xx = 0x06;
  } else {
    xx = 0x86;
  }
  INITIALIZE_DATAGRAM(24, 0);
  datagram[4] = xx;
  return DATAGRAM_24_LENGTH;
}

int build_total_and_trip_log_datagram(char *datagram, int total_nm_times_10, int trip_nm_times_100) {
  // 25 z4 xx yy uu vv aw
  // total = (z * 4096 + yy * 256 + xx) / 10 (max 104857.5 nm)
  // trip = (w * 65546 + vv * 256 + uu) / 100 (max 10485.85 nm)
  int z, xx, yy, uu, vv, w;
  z = total_nm_times_10 >> 16;
  if (z > 0x000fffff) {
    z = 0x0f;
  }
  yy = total_nm_times_10 >> 8;
  xx = total_nm_times_10 & 0xff;
  w = trip_nm_times_100 >> 16;
  if (w > 0x0f) {
    w = 0x0f;
  }
  vv = trip_nm_times_100 >> 8;
  uu = trip_nm_times_100 & 0xff;
  INITIALIZE_DATAGRAM(25, z);
  datagram[2] = xx;
  datagram[3] = yy;
  datagram[4] = uu;
  datagram[5] = vv;
  datagram[6] = 0xa0 | w;
  return DATAGRAM_25_LENGTH;
}

int build_average_water_speed_datagram(char *datagram, int current_times_100, int average_times_100, int use_current_sensor, int use_average_sensor, int stopped, int display_in_statute_miles) {
  // 26 04 xx xx yy yy de
  // xxxx/100 = current speed (kts), valid if d & 0x4 == 4
  // yyyy/100 = average speed (kts), d & 0x8: 8 => sensor; 0 => trip log/time
  // e & 0x1 => calculation stopped
  // e & 0x2 => display in statute miles
  int xx1, xx2, yy1, yy2, d, e;
  xx1 = current_times_100 >> 8;
  xx2 = current_times_100 & 0xff;
  yy1 = average_times_100 >> 8;
  yy2 = average_times_100 & 0xff;
  d = 0;
  d |= (use_current_sensor ? 0x4 : 0);
  d |= (use_average_sensor ? 0x8 : 0);
  e = 0;
  e |= (stopped ? 1 : 0);
  e |= (display_in_statute_miles ? 2 : 0);
  INITIALIZE_DATAGRAM(26, 0);
  datagram[2] = xx1;
  datagram[3] = xx2;
  datagram[4] = yy1;
  datagram[5] = yy2;
  datagram[6] = (d << 4) | e;
  return DATAGRAM_26_LENGTH;
}

int build_water_temperature2_datagram(char *datagram, int celsius_temperature_times_10) {
  // 27 01 xx xx
  // (xxxx - 100) / 10 = temperature in Celsius
  int temp_temperature, xx1, xx2;
  temp_temperature = celsius_temperature_times_10 + 100;
  xx1 = temp_temperature >> 8;
  xx2 = temp_temperature & 0xff;
  INITIALIZE_DATAGRAM(27, 0);
  datagram[2] = xx1;
  datagram[3] = xx2;
  return DATAGRAM_27_LENGTH;
}

int build_set_lamp_intensity_datagram(char *datagram, int intensity) {
  // 30 00 0x x value/intensity 0x0/0, 0x4/1, 0x8/2, 0xc/3
  int x;
  switch (intensity) {
    case 0:
      x = 0x0;
      break;
    case 1:
      x = 0x4;
      break;
    case 2:
      x = 0x8;
      break;
    default:
      x = 0xc;
  }
  INITIALIZE_DATAGRAM(30, 0);
  datagram[2] = x;
  return DATAGRAM_30_LENGTH;
}

int build_cancel_mob_datagram(char *datagram) {
  // 36 00 01 no variation
  INITIALIZE_DATAGRAM(36, 0);
  datagram[2] = 1;
  return DATAGRAM_36_LENGTH;
}

int build_latitude_position(char *datagram, int degrees, int minutes_times_100) {
  // use -ve degrees for southern hemisphere
  // 50 Z2 XX YY YY
  // XX degrees, (YYYY & 0x7fff)/100 minutes
  // YYYY & 0x8000 south if set, north if clear
  // Z = 0xa or 0x0 (Raystar 120 GPS, meaning unknown)
  int xx, yyyy, z;
  if (degrees < 0) {
    xx = -degrees;
    yyyy = 0x8000;
  } else {
    xx = degrees;
    yyyy = 0;
  }
  yyyy |= minutes_times_100;
  z = 0; // not impersonating a Raystar 120 so always 0
  INITIALIZE_DATAGRAM(50, z);
  datagram[2] = xx;
  datagram[3] = yyyy >> 8;
  datagram[4] = yyyy & 0xff;
  return DATAGRAM_50_LENGTH;
}

int build_longitude_position(char *datagram, int degrees, int minutes_times_100) {
  // use -ve degrees for eastern hemisphere
  // 51 Z2 XX YY YY
  // XX degrees, (YYYY & 0x7fff)/100 minutes
  // YYYY & 0x8000 east if set, west if clear
  // Z = 0xa or 0x0 (Raystar 120 GPS, meaning unknown)
  int xx, yyyy, z;
  if (degrees < 0) {
    xx = -degrees;
    yyyy = 0x8000;
  } else {
    xx = degrees;
    yyyy = 0;
  }
  yyyy |= minutes_times_100;
  z = 0; // not impersonaing a Raystar 120 so always 0)
  INITIALIZE_DATAGRAM(51, z);
  datagram[2] = xx;
  datagram[3] = yyyy >> 8;
  datagram[4] = yyyy & 0xff;
  return DATAGRAM_51_LENGTH;
}

int build_speed_over_ground_datagram(char *datagram, int speed_times_10) {
  // 52 01 XX XX
  // XXXX/10 is speed over ground in kts
  int xxxx = speed_times_10;
  INITIALIZE_DATAGRAM(52, 0);
  datagram[2] = xxxx >> 8;
  datagram[3] = xxxx & 0xff;
  return DATAGRAM_52_LENGTH;
}

int build_course_over_ground_datagram(char *datagram, int course) {
  // 53 U0 VW
  // magnetic course = (U & 0x3) * 90 + (VW & 0x3F) * 2 + (U & 0xC) >> 2
  int u, vw;
  uvw_compass(&u, &vw, course);
  INITIALIZE_DATAGRAM(53, u);
  datagram[2] = vw;
  return DATAGRAM_53_LENGTH;
}

int build_gmt_time(char *datagram, int hours, int minutes, int seconds) {
  // 54, T1, RS, HH
  // HH = hours
  // 6 highest bits of RST = minutes = (RS & 0xfc) >> 2
  // 6 lowest bits of RST = seconds = ST & 0x3f
  int t, rs, hh;
  hh = hours;
  rs = minutes << 2;
  rs |= seconds >> 6;
  t = seconds & 0xf;
  INITIALIZE_DATAGRAM(54, t);
  datagram[2] = rs;
  datagram[3] = hh;
  return DATAGRAM_54_LENGTH;
}

//int build_track_keystroke_from_gps(char *datagram) {
//}

int build_date_datagram(char *datagram, int year, int month, int day) {
  // 56 M1 DD YY
  // YY year M month D day of month
  int yy, m, dd;
  yy = year;
  m = month;
  dd = day;
  INITIALIZE_DATAGRAM(56, m);
  datagram[2] = dd;
  datagram[3] = yy;
  return DATAGRAM_56_LENGTH;
}

int build_compass_datagram(char *datagram, int compass) {
  // 89 U2 VW XY 2Z
  // compass (degrees)
  // lower two bits of U * 90 +
  // the six lower bits of VW * 2 +
  // the two higher bits of U / 2
  // locked stear reference (ST40 only):
  // two higher bits of V * 90 + XY / 2
  // Z & 2 = 0: ST40 in standby mode
  // Z & 2 = 2: ST40 in locked stear mode
  int u, vw_lower, vw_higher, xy, z;
  uvw_compass(&u, &vw_lower, compass);
//  temp_compass = compass;
//  vw_higher = (int)(temp_compass / 90) << 6;
  vw_higher = 0;
//  temp_compass = temp_compass - ((vw_higher >> 6) * 90);
//  xy = temp_compass << 1;
  xy = 0;
  z = 0;
  initialize_datagram(datagram, 0x89, DATAGRAM_89_LENGTH, u);
  datagram[2] = vw_higher | vw_lower;
  datagram[3] = xy;
  datagram[4] = 0x20 | z;
  datagram[2] = vw_lower;
  return DATAGRAM_89_LENGTH;
}
