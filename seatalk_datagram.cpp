#include "seatalk_protocol.h"
#include "seatalk_datagram.h"

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

int two_bytes(char *datagram, int index) {
  int byte1, byte2;
  byte1 = datagram[index];
  byte2 = datagram[index + 1];
  return (byte1 << 8) | byte2;
}

int complement_checksum(int value) {
  return value ^ 0xff;
}

int fix_twos_complement_char(int value) {
  if (value & 0x80) {
  return -1 * ((value ^ 0xff) + 1);
  } else {
    return value;
  }
}

int fix_twos_complement_word(int value) {
  if (value & 0x8000) {
    return -1 * ((value ^ 0xffff) + 1);
  } else {
    return value;
  }
}

int flag(int flags, int mask) {
  return (flags & mask) ? 1 : 0;
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

void uvw_heading_and_turning_direction(int heading, TURN_DIRECTION turning_direction, int *u, int *vw) {
  int temp_direction, odd;
  temp_direction = heading;
  odd = temp_direction % 2;
  *u = turning_direction == TURN_DIRECTION_RIGHT ? 0x8 : 0;
  if (odd) {
    if (*u & 0x8) {
      // high bit of u adds 1 to heading so nothing to do
    } else {
      *u |= 0x4;
    }
  } else {
    if (*u & 0x8) {
      // high bit of u adds 1 to heading so need to reduce vw component
      if (temp_direction == 0) {
        temp_direction = 359;
      } else {
        temp_direction -= 1;
      }
      *u |= 0x4;
    } else {
      // heading reflected accurately with no extra from u
    }
  }
  *u |= temp_direction / 90;
  temp_direction = temp_direction % 90;
  *vw = temp_direction / 2;
}

int alarm_is_active(int active_alarms, int alarm) {
  return (active_alarms & alarm) ? 0xffff : 0;
}

int st_build_depth_below_transducer(char *datagram, int depth_in_feet_times_10, int display_in_metres, int active_alarms, int transducer_defective) {
  // 00 02 yz xx xx
  // depth below transducer (in feet): xxxx/10
  // anchor alarm: y & 0x8
  // metric display: y & 4
  // deep water alarm: z & 2
  // shallow water alarm: z & 1
  int xxxx = depth_in_feet_times_10;
  int y = (alarm_is_active(active_alarms, ALARM_ANCHOR) & 0x8) | (display_in_metres ? 0x4 : 0x0);
  int z = (alarm_is_active(active_alarms, ALARM_DEEP_WATER) & 0x2) | (alarm_is_active(active_alarms, ALARM_SHALLOW_WATER) & 0x1) | (transducer_defective ? 0x4 : 0);;
  INITIALIZE_DATAGRAM(00, 0);
  datagram[2] = (y << 4) | z;
  datagram[3] = xxxx >> 8;
  datagram[4] = xxxx & 0xff;
  return DATAGRAM_00_LENGTH;
}

void st_parse_depth_below_transducer(char *datagram, int *depth_in_feet_times_10, int *display_units, int *active_alarms, int *transducer_defective) {
  int y, z, xxxx;
  y = FIRST_NIBBLE(datagram[2]);
  z = LAST_NIBBLE(datagram[2]);
  xxxx = two_bytes(datagram, 3);
  *depth_in_feet_times_10 = xxxx;
  *display_units = flag(y, 0x4);
  *active_alarms = 0;
  *active_alarms |= flag(z, 0x1) ? ALARM_SHALLOW_WATER : 0;
  *active_alarms |= flag(z, 0x2) ? ALARM_DEEP_WATER : 0;
  *active_alarms |= flag(y, 0x8) ? ALARM_ANCHOR : 0;
  *transducer_defective = flag(z, 0x4);
}

int st_build_engine_rpm_and_pitch(char *datagram, ENGINE_ID engine_id, int rpm, int pitch_percent) {
  // 05 03 0x yy zz pp
  // x = 0: single engine (supply starboard_indicator as -1)
  // x = 1: starboard engine (supply staboard_indicator as 1)
  // x = 2: port engine (supply starboard_indicator as 0)
  // rpm: yy * 256 + zz (signed value)
  // pp: prop pitch, signed value
  int x, yy, zz, pp;
  switch (engine_id) {
    case ENGINE_ID_PORT:
      x = 2;
      break;
    case ENGINE_ID_STARBOARD:
      x = 1;
      break;
    default:
      x = 0;
  }
  yy = rpm >> 8;
  zz = rpm & 0xff;;
  pp = pitch_percent;
  INITIALIZE_DATAGRAM(05, 0);
  datagram[2] = x;
  datagram[3] = yy;
  datagram[4] = zz;
  datagram[5] = pp;
  return DATAGRAM_05_LENGTH;
}

void st_parse_engine_rpm_and_pitch(char *datagram, ENGINE_ID *engine_id, int *rpm, int *pitch_percent) {
  int x, yyzz, pp;
  x = datagram[2];
  yyzz = two_bytes(datagram, 3);
  pp = datagram[5];
  switch (x) {
    case 1:
      *engine_id = ENGINE_ID_STARBOARD;
      break;
    case 2:
      *engine_id = ENGINE_ID_PORT;
      break;
    default:
      *engine_id = ENGINE_ID_SINGLE;
  }
  *rpm = yyzz;
  *pitch_percent = fix_twos_complement_char(pp);
}

int st_build_apparent_wind_angle(char *datagram, int degrees_right_times_2) {
  // 10 01 xx yy
  // xxyy/2 = degrees right of bow
  int xx, yy;
  xx = degrees_right_times_2 >> 8;
  yy = degrees_right_times_2 & 0xff;
  INITIALIZE_DATAGRAM(10, 0);
  datagram[2] = xx;
  datagram[3] = yy;
  return DATAGRAM_10_LENGTH;
}

void st_parse_apparent_wind_angle(char *datagram, int *degrees_right_times_2) {
  int xxyy;
  xxyy = two_bytes(datagram, 2);
  *degrees_right_times_2 = fix_twos_complement_word(xxyy);
}

int st_build_apparent_wind_speed(char *datagram, int knots_times_10, int display_in_metric) {
  // 11 01 xx 0y
  // wind speed: (xx & 0x7f) + y/10
  // units flag xx & 0x80; 0 knots/0x80 m/s
  int xx, y;
  xx = (knots_times_10 / 10);
  y = knots_times_10 % 10;
  if (xx > 0x7f) {
    xx = 0x7f;
  }
  if (display_in_metric) {
    xx |= 0x80;
  }
  INITIALIZE_DATAGRAM(11, 0);
  datagram[2] = xx;
  datagram[3] = y;
  return DATAGRAM_11_LENGTH;
}

void st_parse_apparent_wind_speed(char *datagram, int *knots_times_10, int *display_in_metric) {
  int xx, y;
  xx = datagram[2];
  y = LAST_NIBBLE(datagram[3]);
  *knots_times_10 = (xx & 0x7f) * 10 + y;
  *display_in_metric = flag(xx, 0x80);
}

int st_build_water_speed(char *datagram, int knots_times_10) {
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

void st_parse_water_speed(char *datagram, int *knots_times_10) {
  int xxxx;
  xxxx = two_bytes(datagram, 2);
  *knots_times_10 = xxxx;
}

int st_build_trip_mileage(char *datagram, int nmiles_times_100) {
  // 21 02 xx xx 0x
  // mileage = xxxxx/100 nautical  miles
  int xx1, xx2, x;
  xx1 = nmiles_times_100 >> 12;
  xx2 = (nmiles_times_100 >> 4) & 0xff;
  x = nmiles_times_100 & 0x0f;
  INITIALIZE_DATAGRAM(21, 0);
  datagram[2] = xx1;
  datagram[3] = xx2;
  datagram[4] = x;
  return DATAGRAM_21_LENGTH;
}

void st_parse_trip_mileage(char *datagram, int *miles_times_100) {
  int xxxxx;
  xxxxx = (two_bytes(datagram, 2) << 4) | LAST_NIBBLE(datagram[4]);
  *miles_times_100 = xxxxx;
}

int st_build_total_mileage(char *datagram, int nmiles_times_10) {
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

void st_parse_total_mileage(char *datagram, int *miles_times_10) {
  int xxxx;
  xxxx = two_bytes(datagram, 2);
  *miles_times_10 = xxxx;
}

int st_build_water_temperature(char *datagram, int degrees_fahrenheit, int transducer_defective) {
  // 23 z1 xx yy
  // z & 0x4 sensor problem
  // xx degrees Celsius, yy degrees Fahrenheit
  int xx, yy, z;
  xx = (int)((degrees_fahrenheit - 32) * 5 / 9);
  yy = degrees_fahrenheit;
  z = transducer_defective ? 0x4 : 0;
  INITIALIZE_DATAGRAM(23, z);
  datagram[2] = xx;
  datagram[3] = yy;
  return DATAGRAM_23_LENGTH;
}

void st_parse_water_temperature(char *datagram, int *degrees_fahrenheit, int *transducer_defective) {
  int xx, yy, z;
  xx = datagram[2];
  yy = datagram[3];
  z = FIRST_NIBBLE(datagram[1]);
  *degrees_fahrenheit = yy;;
  *transducer_defective = flag(z, 0x4);
}

int st_build_speed_distance_units(char *datagram, DISTANCE_UNITS distance_units) {
  // 24 02 00 00 xx
  // xx == 00 nm/kts; 06 sm/mph; 86 km/kph
  int xx;
  switch (distance_units) {
    case DISTANCE_UNITS_NAUTICAL:
      xx = 0;
      break;
    case DISTANCE_UNITS_STATUTE:
      xx = 0x06;
      break;
    default:
      xx = 0x86;
      break;
  }
  INITIALIZE_DATAGRAM(24, 0);
  datagram[4] = xx;
  return DATAGRAM_24_LENGTH;
}

void st_parse_speed_distance_units(char *datagram, DISTANCE_UNITS *distance_units) {
  int xx;
  xx = datagram[4];
  if (xx == 0) {
    *distance_units = DISTANCE_UNITS_NAUTICAL;
  } else if (xx == 6) {
    *distance_units = DISTANCE_UNITS_STATUTE;
  } else {
    *distance_units = DISTANCE_UNITS_METRIC;
  }
}

int st_build_total_and_trip_mileage(char *datagram, int total_nm_times_10, int trip_nm_times_100) {
  // 25 z4 xx yy uu vv aw
  // total = (z * 4096 + yy * 256 + xx) / 10 (max 104857.5 nm)
  // trip = (w * 65546 + vv * 256 + uu) / 100 (max 10485.85 nm)
  int z, xx, yy, uu, vv, w;
  z = total_nm_times_10 >> 16;
  if (z > 0x0f) {
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

void st_parse_total_and_trip_mileage(char *datagram, int *total_mileage_times_10, int *trip_mileage_times_100) {
  int xx, yy, uu, vv, z, w;
  xx = datagram[2];
  yy = datagram[3];
  uu = datagram[4];
  vv = datagram[5];
  z = FIRST_NIBBLE(datagram[1]);
  w = LAST_NIBBLE(datagram[6]);
  *total_mileage_times_10 = (z << 16) | (yy << 8) | xx;
  *trip_mileage_times_100 = (w << 16) | (vv << 8) | uu;
}

int st_build_average_water_speed(char *datagram, int knots_1_times_100, int knots_2_times_100, int speed_1_from_sensor, int speed_2_is_average, int average_is_stopped, int display_in_statute_miles) {
  // 26 04 xx xx yy yy de
  // xxxx/100 = current speed (kts), valid if d & 0x4 == 4
  // yyyy/100 = average speed (kts), d & 0x8: 8 => sensor; 0 => trip log/time
  // e & 0x1 => calculation stopped
  // e & 0x2 => display in statute miles
  int xx1, xx2, yy1, yy2, d, e;
  xx1 = knots_1_times_100 >> 8;
  xx2 = knots_1_times_100 & 0xff;
  yy1 = knots_2_times_100 >> 8;
  yy2 = knots_2_times_100 & 0xff;
  d = 0;
  d |= (speed_1_from_sensor ? 0x4 : 0);
  d |= (speed_2_is_average ? 0x8 : 0);
  e = 0;
  e |= (average_is_stopped ? 1 : 0);
  e |= (display_in_statute_miles ? 2 : 0);
  INITIALIZE_DATAGRAM(26, 0);
  datagram[2] = xx1;
  datagram[3] = xx2;
  datagram[4] = yy1;
  datagram[5] = yy2;
  datagram[6] = (d << 4) | e;
  return DATAGRAM_26_LENGTH;
}

void st_parse_average_water_speed(char *datagram, int *knots_1_times_100, int *knots_2_times_100, int *speed_1_from_sensor, int *speed_2_is_average, int *average_is_stopped, int *display_in_statute_miles) {
  int xxxx, yyyy, d, e;
  xxxx = two_bytes(datagram, 2);
  yyyy = two_bytes(datagram, 4);
  d = FIRST_NIBBLE(datagram[6]);
  e = LAST_NIBBLE(datagram[6]);
  *knots_1_times_100 = xxxx;
  *knots_2_times_100 = yyyy;
  *speed_1_from_sensor = flag(d, 0x4);
  *speed_2_is_average = flag(d, 0x8);
  *average_is_stopped = flag(e, 0x1);
  *display_in_statute_miles = flag(e, 0x2);
}

int st_build_precise_water_temperature(char *datagram, int degrees_celsius_times_10) {
  // 27 01 xx xx
  // (xxxx - 100) / 10 = temperature in Celsius
  int temp_temperature, xx1, xx2;
  temp_temperature = degrees_celsius_times_10 + 100;
  xx1 = temp_temperature >> 8;
  xx2 = temp_temperature & 0xff;
  INITIALIZE_DATAGRAM(27, 0);
  datagram[2] = xx1;
  datagram[3] = xx2;
  return DATAGRAM_27_LENGTH;
}

void st_parse_precise_water_temperature(char *datagram, int *degrees_celsius_times_10) {
  int xxxx;
  xxxx = two_bytes(datagram, 2);
  *degrees_celsius_times_10 = xxxx - 100;
}

int st_build_set_lamp_intensity(char *datagram, int intensity) {
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

void st_parse_set_lamp_intensity(char *datagram, int *level) {
  int x;
  x = LAST_NIBBLE(datagram[2]);
  switch (x) {
    case 0x0:
      *level = 0;
      break;
    case 0x4:
      *level = 1;
      break;
    case 0x8:
      *level = 2;
      break;
    case 0xc:
      *level = 3;
      break;
  }
}

int st_build_cancel_mob(char *datagram) {
  // 36 00 01 no variation
  INITIALIZE_DATAGRAM(36, 0);
  datagram[2] = 1;
  return DATAGRAM_36_LENGTH;
}

int st_build_lat_position(char *datagram, LATITUDE_HEMISPHERE hemisphere, int degrees, int minutes_times_100) {
  // use -ve degrees for southern hemisphere
  // 50 Z2 XX YY YY
  // XX degrees, (YYYY & 0x7fff)/100 minutes
  // YYYY & 0x8000 south if set, north if clear
  // Z = 0xa or 0x0 (Raystar 120 GPS, meaning unknown)
  int xx, yyyy, z;
  xx = degrees;
  yyyy = hemisphere == LATITUDE_HEMISPHERE_NORTH ? 0 : 0x8000;
  yyyy |= minutes_times_100;
  z = 0; // not impersonating a Raystar 120 so always 0
  INITIALIZE_DATAGRAM(50, z);
  datagram[2] = xx;
  datagram[3] = yyyy >> 8;
  datagram[4] = yyyy & 0xff;
  return DATAGRAM_50_LENGTH;
}

void st_parse_lat_position(char *datagram, LATITUDE_HEMISPHERE *hemisphere, int *degrees, int *minutes_times_100) {
  int xx, yyyy;
  xx = datagram[2];
  yyyy = two_bytes(datagram, 3);
  *hemisphere = flag(yyyy, 0x8000) ? LATITUDE_HEMISPHERE_SOUTH : LATITUDE_HEMISPHERE_NORTH;
  *degrees = xx;
  *minutes_times_100 = yyyy & 0x7fff;
}

int st_build_lon_position(char *datagram, LONGITUDE_HEMISPHERE hemisphere, int degrees, int minutes_times_100) {
  // use -ve degrees for eastern hemisphere
  // 51 Z2 XX YY YY
  // XX degrees, (YYYY & 0x7fff)/100 minutes
  // YYYY & 0x8000 east if set, west if clear
  // Z = 0xa or 0x0 (Raystar 120 GPS, meaning unknown)
  int xx, yyyy, z;
  xx = degrees;
  yyyy = hemisphere == LONGITUDE_HEMISPHERE_WEST ? 0 : 0x8000;
  yyyy |= minutes_times_100;
  z = 0; // not impersonaing a Raystar 120 so always 0)
  INITIALIZE_DATAGRAM(51, z);
  datagram[2] = xx;
  datagram[3] = yyyy >> 8;
  datagram[4] = yyyy & 0xff;
  return DATAGRAM_51_LENGTH;
}

void st_parse_lon_position(char *datagram, LONGITUDE_HEMISPHERE *hemisphere, int *degrees, int *minutes_times_100) {
  int xx, yyyy;
  xx = datagram[2];
  yyyy = two_bytes(datagram, 3);
  *hemisphere = flag(yyyy, 0x8000) ? LONGITUDE_HEMISPHERE_EAST : LONGITUDE_HEMISPHERE_WEST;
  *degrees = xx;
  *minutes_times_100 = yyyy & 0x7fff;
}

int st_build_speed_over_ground(char *datagram, int knots_times_10) {
  // 52 01 XX XX
  // XXXX/10 is speed over ground in kts
  int xxxx = knots_times_10;
  INITIALIZE_DATAGRAM(52, 0);
  datagram[2] = xxxx >> 8;
  datagram[3] = xxxx & 0xff;
  return DATAGRAM_52_LENGTH;
}

void st_parse_speed_over_ground(char *datagram, int *knots_times_10) {
  int xxxx;
  xxxx = two_bytes(datagram, 2);
  *knots_times_10 = xxxx;
}

int st_build_course_over_ground(char *datagram, int true_degrees) {
  // 53 U0 VW
  // true course = (U & 0x3) * 90 + (VW & 0x3F) * 2 + (U & 0xC) >> 2
  int u, vw;
  uvw_compass(&u, &vw, true_degrees);
  INITIALIZE_DATAGRAM(53, u);
  datagram[2] = vw;
  return DATAGRAM_53_LENGTH;
}

void st_parse_course_over_ground(char *datagram, int *true_degrees) {
  int u, vw;
  u = FIRST_NIBBLE(datagram[1]);
  vw = datagram[2];
  *true_degrees = ((u & 0x3) * 90) + ((vw & 0x3f) * 2) + (u >> 3);
}

int st_build_gmt_time(char *datagram, int hours, int minutes, int seconds) {
  // 54, T1, RS, HH
  // HH = hours
  // 6 highest bits of RST = minutes = (RS & 0xfc) >> 2
  // 6 lowest bits of RST = seconds = ST & 0x3f
  int t, rs, hh;
  hh = hours;
  rs = minutes << 2;
  rs |= seconds >> 4;
  t = seconds & 0xf;
  INITIALIZE_DATAGRAM(54, t);
  datagram[2] = rs;
  datagram[3] = hh;
  return DATAGRAM_54_LENGTH;
}

void st_parse_gmt_time(char *datagram, int *hours, int *minutes, int *seconds) {
  int t, rs, hh;
  t = FIRST_NIBBLE(datagram[1]);
  rs = datagram[2];
  hh = datagram[3];
  *hours = hh;
  *minutes = rs >> 2;
  *seconds = ((rs & 0x3) << 4) | t;
}

int st_build_date(char *datagram, int year, int month, int day) {
  // 56 M1 DD YY
  // YY year M month D day of month
  int yy, m, dd;
  yy = year - 1970; // arbitrary epoch offset until real data available
  m = month;
  dd = day;
  INITIALIZE_DATAGRAM(56, m);
  datagram[2] = dd;
  datagram[3] = yy;
  return DATAGRAM_56_LENGTH;
}

void st_parse_date(char *datagram, int *year, int *month, int *day) {
  int m, dd, yy;
  m = FIRST_NIBBLE(datagram[1]);
  dd = datagram[2];
  yy = datagram[3] + 1970; // arbitrary epoch offset until real data available
  *year = yy;
  *month = m;
  *day = dd;
}

int st_build_satellite_info(char *datagram, int satellite_count, int horizontal_dilution_of_position) {
  // 58 z5 la xx yy lo qq rr
  // lat/lon position
  // la, lo degrees latitude, longitude
  // minutes lat xxyy/1000
  // minutes lon qqrr/1000
  // z: south if z&1
  //    east if z&2
  int s, dd;
  s = satellite_count;
  dd = horizontal_dilution_of_position;
  INITIALIZE_DATAGRAM(57, s);
  datagram[2] = dd;
  return DATAGRAM_57_LENGTH;
}

void st_parse_satellite_info(char *datagram, int *satellite_count, int *horizontal_dilution_of_position) {
  int s, dd;
  s = FIRST_NIBBLE(datagram[1]);
  dd = datagram[2];
  *satellite_count = s;
  *horizontal_dilution_of_position = dd;
}

int st_build_lat_lon_position(char *datagram, LATITUDE_HEMISPHERE hemisphere_latitude, int degrees_lat, int minutes_lat_times_1000, LONGITUDE_HEMISPHERE hemisphere_longitude, int degrees_lon, int minutes_lon_times_1000) {
  int z, la, xx, yy, lo, qq, rr;
  z = 0;
  if (hemisphere_latitude == LATITUDE_HEMISPHERE_SOUTH) {
    z |= 0x1;
  }
  la = degrees_lat;
  xx = minutes_lat_times_1000 >> 8;
  yy = minutes_lat_times_1000 & 0xff;
  if (hemisphere_longitude == LONGITUDE_HEMISPHERE_EAST) {
    z |= 0x02;
  }
  lo = degrees_lon;
  qq = minutes_lon_times_1000 >> 8;
  rr = minutes_lon_times_1000 & 0xff;
  INITIALIZE_DATAGRAM(58, z);
  datagram[2] = la;
  datagram[3] = xx;
  datagram[4] = yy;
  datagram[5] = lo;
  datagram[6] = qq;
  datagram[7] = rr;
  return DATAGRAM_58_LENGTH;
}

void st_parse_lat_lon_position(char *datagram, LATITUDE_HEMISPHERE *hemisphere_latitude, int *degrees_lat, int *minutes_lat_times_1000, LONGITUDE_HEMISPHERE *hemisphere_longitude, int *degrees_lon, int *minutes_lon_times_1000) {
  int z, la, xxyy, lo, qqrr;
  z = FIRST_NIBBLE(datagram[1]);
  la = datagram[2];
  xxyy = two_bytes(datagram, 3);
  lo = datagram[5];
  qqrr = two_bytes(datagram, 6);
  *hemisphere_latitude = flag(z, 0x1) ? LATITUDE_HEMISPHERE_SOUTH : LATITUDE_HEMISPHERE_NORTH;
  *degrees_lat = la;
  *minutes_lat_times_1000 = xxyy;
  *hemisphere_longitude = flag(z, 0x2) ? LONGITUDE_HEMISPHERE_EAST : LONGITUDE_HEMISPHERE_WEST;
  *degrees_lon = lo;
  *minutes_lon_times_1000 = qqrr;
}

int st_build_set_countdown_timer(char *datagram, int hours, int minutes, int seconds, TIMER_MODE mode) {
  // 59 22 ss mm xh
  // set countdown timer
  // mm minutes, ss seconds, h hours
  // msb mm count up start flag
  // x: counter mode
  // 0 = count up and start
  // 4 = count down
  // 8 = count down and start
  int ss, mm, x, h;
  h = hours;
  mm = minutes;
  ss = seconds;
  switch (mode) {
    case TIMER_MODE_COUNT_DOWN:
      x = 4;
      break;
    case TIMER_MODE_COUNT_UP_AND_START:
      x = 0;
      mm |= 0x80;
      break;
    case TIMER_MODE_COUNT_DOWN_AND_START:
      x = 8;
      break;
    default:
      x = 0;
  }
  INITIALIZE_DATAGRAM(59, 2);
  datagram[2] = ss;
  datagram[3] = mm;
  datagram[4] = (x << 4) | h;
  return DATAGRAM_59_LENGTH;
}

void st_parse_set_countdown_timer(char *datagram, int *hours, int *minutes, int *seconds, TIMER_MODE *mode) {
  int ss, mm, x, h;
  ss = datagram[2];
  mm = datagram[3];
  x = FIRST_NIBBLE(datagram[4]);
  h = LAST_NIBBLE(datagram[4]);
  *hours = h;
  *minutes = mm & 0x7f;
  *seconds = ss;
  if (x == 0) {
    if (mm & 0x80) {
      *mode = TIMER_MODE_COUNT_UP_AND_START;
    } else {
      *mode = TIMER_MODE_COUNT_UP;
    }
  } else if (x == 4) {
    *mode = TIMER_MODE_COUNT_DOWN;
  } else if (x == 8) {
    *mode = TIMER_MODE_COUNT_DOWN_AND_START;
  }
}

int st_build_wind_alarm(char *datagram, int active_alarms) {
  // 66 00 xy
  // x: apparent wind alarm; y: true wind
  // bits: & 0x8 angle low, 0x4 angle high, 0x2 speed low, 0x1 speed high
  // xy = 0: end all wind alarms
  int x, y;
  x = 0;
  y = 0;
  x |= alarm_is_active(active_alarms, ALARM_APPARENT_WIND_ANGLE_LOW) & 0x8;
  x |= alarm_is_active(active_alarms, ALARM_APPARENT_WIND_ANGLE_HIGH) & 0x4;
  x |= alarm_is_active(active_alarms, ALARM_APPARENT_WIND_SPEED_LOW) & 0x2;
  x |= alarm_is_active(active_alarms, ALARM_APPARENT_WIND_SPEED_HIGH) & 0x1;
  y |= alarm_is_active(active_alarms, ALARM_TRUE_WIND_ANGLE_LOW) & 0x8;
  y |= alarm_is_active(active_alarms, ALARM_TRUE_WIND_ANGLE_HIGH) & 0x4;
  y |= alarm_is_active(active_alarms, ALARM_TRUE_WIND_SPEED_LOW) & 0x2;
  y |= alarm_is_active(active_alarms, ALARM_TRUE_WIND_SPEED_HIGH) & 0x1;
  INITIALIZE_DATAGRAM(66, 0);
  datagram[2] = (x << 4) | y;
  return DATAGRAM_66_LENGTH;
}

void st_parse_wind_alarm(char *datagram, int *active_alarms) {
  int x, y;
  x = FIRST_NIBBLE(datagram[2]);
  y = LAST_NIBBLE(datagram[2]);
  *active_alarms = 0;
  *active_alarms |= flag(x, 0x8) ? ALARM_APPARENT_WIND_ANGLE_LOW : 0;
  *active_alarms |= flag(x, 0x4) ? ALARM_APPARENT_WIND_ANGLE_HIGH : 0;
  *active_alarms |= flag(x, 0x2) ? ALARM_APPARENT_WIND_SPEED_LOW : 0;
  *active_alarms |= flag(x, 0x1) ? ALARM_APPARENT_WIND_SPEED_HIGH : 0;
  *active_alarms |= flag(y, 0x8) ? ALARM_TRUE_WIND_ANGLE_LOW : 0;
  *active_alarms |= flag(y, 0x4) ? ALARM_TRUE_WIND_ANGLE_HIGH : 0;
  *active_alarms |= flag(y, 0x2) ? ALARM_TRUE_WIND_SPEED_LOW : 0;
  *active_alarms |= flag(y, 0x1) ? ALARM_TRUE_WIND_SPEED_HIGH : 0;
}

int st_build_alarm_acknowledgement(char *datagram, int acknowledged_alarm) {
  // 68 x1 yy 00
  // yy indicates which device acknowledged. Hard-coding to 01 here.
  // x: 1-shallow water; 2-deep water; 3-anchor; 4-true wind speed high;
  //    5-true wind speed low; 6-true wind angle high; 7-true wind angle low
  //    8-apparent wind speed high; 9-apparent wind speed low;
  //    a-apparent wind angle high; b-apparent wind angle low
  int x;
  switch(acknowledged_alarm) {
    case ALARM_SHALLOW_WATER:
      x = 1;
      break;
    case ALARM_DEEP_WATER:
      x = 2;
      break;
    case ALARM_ANCHOR:
      x = 3;
      break;
    case ALARM_TRUE_WIND_SPEED_HIGH:
      x = 4;
      break;
    case ALARM_TRUE_WIND_SPEED_LOW:
      x = 5;
      break;
    case ALARM_TRUE_WIND_ANGLE_HIGH:
      x = 6;
      break;
    case ALARM_TRUE_WIND_ANGLE_LOW:
      x = 7;
      break;
    case ALARM_APPARENT_WIND_SPEED_HIGH:
      x = 8;
      break;
    case ALARM_APPARENT_WIND_SPEED_LOW:
      x = 9;
      break;
    case ALARM_APPARENT_WIND_ANGLE_HIGH:
      x = 0xa;
      break;
    case ALARM_APPARENT_WIND_ANGLE_LOW:
      x = 0xb;
      break;
    default:
      x = 0;
  }
  INITIALIZE_DATAGRAM(68, x);
  datagram[2] = 1;
  return DATAGRAM_68_LENGTH;
}

void st_parse_alarm_acknowledgement(char *datagram, int *acknowledged_alarm) {
  int x;
  int y;
  x = FIRST_NIBBLE(datagram[1]);
  y = datagram[2];
  if (y == 0x15) {
    *acknowledged_alarm = 0xffff;
  } else {
    switch (x) {
      case 1:
        *acknowledged_alarm = ALARM_SHALLOW_WATER;;
        break;
      case 2:
        *acknowledged_alarm = ALARM_DEEP_WATER;
        break;
      case 3:
        *acknowledged_alarm = ALARM_ANCHOR;
        break;
      case 4:
        *acknowledged_alarm = ALARM_TRUE_WIND_SPEED_HIGH;
        break;
      case 5:
        *acknowledged_alarm = ALARM_TRUE_WIND_SPEED_LOW;;
        break;
      case 6:
        *acknowledged_alarm = ALARM_TRUE_WIND_ANGLE_HIGH;
        break;
      case 7:
        *acknowledged_alarm = ALARM_TRUE_WIND_ANGLE_LOW;;
        break;
      case 8:
        *acknowledged_alarm = ALARM_APPARENT_WIND_SPEED_HIGH;
        break;
      case 9:
        *acknowledged_alarm = ALARM_APPARENT_WIND_SPEED_LOW;
        break;
      case 10:
        *acknowledged_alarm = ALARM_APPARENT_WIND_ANGLE_HIGH;
        break;
      case 11:
        *acknowledged_alarm = ALARM_APPARENT_WIND_ANGLE_LOW;
        break;
    }
  }
}

void st_parse_maxview_keystroke(char *datagram, int *key_1, int *key_2, int *held_longer) {
//  int x, y;
//  x = FIRST_NIBBLE(datagram[2]);
//  y = LAST_NIBBLE(datagram[2]);
//  *key_1 = 0;
//  *key_2 = 0;
//  *held_longer = (x & 0x4) ? 1 : 0;
//  if (x & 0x2 == 0) { // single keypress
//    switch (y) {
//      case 1:
//        *key_1 =
}

int st_build_target_waypoint_name(char *datagram, int char1, int char2, int char3, int char4) {
  // 82 05 xx !xx yy !yy zz !zz
  // !<> means reverse bits so xx + !xx = 0xff
  // last 4 chars of wpt name, upper case only; use 6 bits per char
  // subtract 0x30 from character values before applying bit masks and shifts
  // xx = six bits of char1 with high bits taken from lowest two of char2
  // yy = low four bits of char2 with high nibble from low nibble of char3
  // zz = high two bits of char3 and 6 high bits come from char4
  int xx, xx_comp, yy, yy_comp, zz, zz_comp;
  int c1, c2, c3, c4;
  c1 = char1 - 0x30;
  c2 = char2 - 0x30;
  c3 = char3 - 0x30;
  c4 = char4 - 0x30;
  xx = (c1 & 0x3f) | ((c2 & 0x3) << 6);
  xx_comp = complement_checksum(xx);
  yy = (c2 >> 2) | ((c3 & 0xf) << 4);
  yy_comp = complement_checksum(yy);
  zz = ((c3 & 0x3c) >> 4) | (c4 << 2);
  zz_comp = complement_checksum(zz);
  INITIALIZE_DATAGRAM(82, 0);
  datagram[2] = xx;
  datagram[3] = xx_comp;
  datagram[4] = yy;
  datagram[5] = yy_comp;
  datagram[6] = zz;
  datagram[7] = zz_comp;
  return DATAGRAM_82_LENGTH;
}

int st_parse_target_waypoint_name(char *datagram, int *char_1, int *char_2, int *char_3, int *char_4) {
  int xx, xx_comp, yy, yy_comp, zz, zz_comp;
  xx = datagram[2];
  xx_comp = datagram[3];
  yy = datagram[4];
  yy_comp = datagram[5];
  zz = datagram[6];
  zz_comp = datagram[7];
  if (((xx + xx_comp) != 0xff) || ((yy + yy_comp) != 0xff) || ((zz + zz_comp) != 0xff)) {
    // transmission error detected
    return -1;
  }
  *char_1 = 0x30 + (xx & 0x3f);
  *char_2 = 0x30 + (((yy & 0xf) << 2) | ((xx & 0xc0) >> 6));
  *char_3 = 0x30 + (((zz & 0x3) << 4) | ((yy & 0xf0) >> 4));
  *char_4 = 0x30 + ((zz & 0xfc) >> 2);
  return 0;
}

int st_build_course_computer_failure(char *datagram, COURSE_COMPUTER_FAILURE_TYPE failure_type) {
  // 83 07 xx 00 00 00 00 00 80 00 00
  // course compuer failure
  // xx 0: sent after clearing failure and on power-up
  // xx 1 failure, auto release error. Repeated every second
  // xx 8 failure, drive stopped
  int xx;
  switch (failure_type) {
    case COURSE_COMPUTER_FAILURE_TYPE_AUTO_RELEASE_ERROR:
      xx = 1;
      break;
    case COURSE_COMPUTER_FAILURE_TYPE_DRIVE_STOPPED:
      xx = 8;
      break;
    default:
      xx = 0;
  }
  INITIALIZE_DATAGRAM(83, 0);
  datagram[2] = xx;
  return DATAGRAM_83_LENGTH;
}

void st_parse_course_computer_failure(char *datagram, COURSE_COMPUTER_FAILURE_TYPE *failure_type) {
  int xx;
  xx = datagram[2];
  switch (xx) {
    case 0:
      *failure_type = COURSE_COMPUTER_FAILURE_TYPE_NONE;
      break;
    case 1:
      *failure_type = COURSE_COMPUTER_FAILURE_TYPE_AUTO_RELEASE_ERROR;
      break;
    case 8:
      *failure_type = COURSE_COMPUTER_FAILURE_TYPE_DRIVE_STOPPED;
      break;
  }
}

int st_build_autopilot_status(char *datagram, int compass_heading, TURN_DIRECTION  turning_direction, int target_heading, AUTOPILOT_MODE mode, int rudder_position, int alarms, int display_flags) {
  // 84 u6 vw xy 0z 0m rr ss tt
  // compass heading: (u & 0x3) * 90 + (vw & 0x3f) * 2 + (u & 0xc ? ((u & 0xc) == 0xc ? 2 : 1) : 0)
  // turning direction: msb of u; 1 right, 0 left
  // autopilot target course: high 2 bits of v * 90 + xy/2
  // mode: z & 0x2 = 0: standby; & 0x2 = 1: auto; &0x4 = vane; &0x8 = track
  // alarms: m & 4: off course; m & 8 wind shift
  // rudder position in degrees; positive numbers steer right
  // ss = display; 0x1 turn off heading display; 0x2: always on; 0x8: "no data"
  //      0x10 "large xte"; 0x80: "auto rel"
  // tt: 400G computer always 0x08; 150(G) computer always 0x05
  int u, vw, xy, z, m, rr, ss, tt;
  int temp_direction;
  uvw_heading_and_turning_direction(compass_heading, turning_direction, &u, &vw);
  temp_direction = target_heading;
  vw |= (temp_direction / 90) << 6;
  xy = (temp_direction % 90) << 1;
  z = mode;
  rr = rudder_position;
  m = 0;
  if (flag(alarms, ALARM_AUTOPILOT_OFF_COURSE)) {
    m |= 0x04;
  }
  if (flag(alarms, ALARM_AUTOPILOT_WIND_SHIFT)) {
    m |= 0x08;
  }
  ss = 0;
  ss = display_flags;
  tt = 0;
  INITIALIZE_DATAGRAM(84, u);
  datagram[2] = vw;
  datagram[3] = xy;
  datagram[4] = z;
  datagram[5] = m;
  datagram[6] = rr;
  datagram[7] = ss;
  datagram[8] = tt;
  return DATAGRAM_84_LENGTH;
}

void st_parse_autopilot_status(char *datagram, int *compass_heading, TURN_DIRECTION *turning_direction, int *target_heading, AUTOPILOT_MODE *mode, int *rudder_position, int *alarms, int *display_flags) {
  char u, vw, xy, z, m, rr, ss, tt;

  // printf("parse_autopilot_status\n");
  u = FIRST_NIBBLE(datagram[1]);
  vw = datagram[2];
  xy = datagram[3];
  z = LAST_NIBBLE(datagram[4]);
  m = LAST_NIBBLE(datagram[5]);
  rr = datagram[6];
  ss = datagram[7];
  tt = datagram[8];

  *compass_heading = ((u & 0x03) * 90) + ((vw & 0x3f) * 2) + ((u & 0x0c) ? (((u & 0x0c) == 0x0c) ? 2 : 1) : 0);
  *turning_direction = flag(u, 0x08) ? TURN_DIRECTION_RIGHT : TURN_DIRECTION_LEFT;
  *target_heading = (((vw & 0xc0) >> 6) * 90) + (xy / 2);
  *mode = (AUTOPILOT_MODE)z;
  *rudder_position = fix_twos_complement_char(rr);
  *alarms = 0;
  *alarms |= flag(m, 0x04) ? ALARM_AUTOPILOT_OFF_COURSE : 0;
  *alarms |= flag(m, 0x08) ? ALARM_AUTOPILOT_WIND_SHIFT : 0;
  *display_flags = ss;
}

int st_build_waypoint_navigation(char *datagram, int cross_track_error_present, int cross_track_error_times_100, int waypoint_bearing_present, int waypoint_bearing, int bearing_is_magnetic,\
    int waypoint_distance_present, int waypoint_distance_times_100, int direction_to_steer) {
  // 85 x6 xx vu zw zz yf 00 !yf
  // cross track error = xxx/100
  // bearing to destination (u & 0x3) * 90 + (wv / 2)
  // u & 8: 8 bearing is true/0 bearing is magnetic
  // distance 0-9.99nm zzz/100, y & 1 = 1/>= 10.0 nm zzz/10, y & 1 = 0
  // direction to steer y & 4 = 4: right, y & 4 = 0: left
  // f = flags: 1->XTE present; 2->bearing to waypoint present;
  //   4->range to destination present; 8->xte>0.3nm
  // !yf: bit complement of yf
  int x, xx, v, u, z, w, zz, y, f;
  int temp_value;
  x = cross_track_error_times_100 >> 8 & 0xf;
  xx = cross_track_error_times_100 & 0xff;
  temp_value = waypoint_bearing;
  u = temp_value / 90;
  temp_value = (temp_value % 90) << 1;
  w = temp_value >> 4;
  v = temp_value & 0xf;
  u |= bearing_is_magnetic ? 0 : 0x8;
  y = waypoint_distance_times_100 < 1000 ? 1 : 0;
  y |= (direction_to_steer > 0) ? 0x4 : 0;
  if (y & 0x1) {
    temp_value = waypoint_distance_times_100;
  } else {
    temp_value = waypoint_distance_times_100 / 10;
  }
  z = temp_value >> 8;
  zz = temp_value & 0xff;
  f = 0;
  if (cross_track_error_present) {
    f |= 0x1;
  }
  if (waypoint_bearing_present) {
    f |= 0x2;
  }
  if (waypoint_distance_present) {
    f |= 0x4;
  }
  if (cross_track_error_times_100 >= 30) {
    f |= 0x8;
  }
  INITIALIZE_DATAGRAM(85, x);
  datagram[2] = xx;
  datagram[3] = (v << 4) | u;
  datagram[4] = (z << 4) | w;
  datagram[5] = zz;
  datagram[6] = (y << 4) | f;
  datagram[7] = 0;
  datagram[8] = complement_checksum(datagram[6]);
  return DATAGRAM_85_LENGTH;
}

int st_parse_waypoint_navigation(char *datagram, int *cross_track_error_present, int *cross_track_error_times_100, int *waypoint_bearing_present, int *waypoint_bearing, int *bearing_is_magnetic,
    int *waypoint_distance_present, int *waypoint_distance_times_100, int *direction_to_steer) {
  int x, xx, v, u, z, w, zz, yf, yf_comp;
  int y, f;
  x = FIRST_NIBBLE(datagram[1]);
  xx = datagram[2];
  v = FIRST_NIBBLE(datagram[3]);
  u = LAST_NIBBLE(datagram[3]);
  z = FIRST_NIBBLE(datagram[4]);
  w = LAST_NIBBLE(datagram[4]);
  zz = datagram[5];
  yf = datagram[6];
  yf_comp = datagram[8];
  if ((yf | yf_comp) != 0xff) {
    // ignore if bit complement check fails
    return 0;
  }
  y = FIRST_NIBBLE(yf);
  f = LAST_NIBBLE(yf);
  *cross_track_error_times_100 = (x << 8) + xx;
  *waypoint_bearing = ((u & 0x3) * 90) + (((w << 4) + v) >> 1);
  *bearing_is_magnetic = flag(u, 0x8) ? 0 : 1;
  *waypoint_distance_times_100 = ((z << 8) + zz) * ((y & 0x1) ? 1 : 10);
  *direction_to_steer = flag(y, 0x4) ? 1 : -1;
  *cross_track_error_present = flag(f, 0x1);
  *waypoint_bearing_present = flag(f, 0x2);
  *waypoint_distance_present = flag(f, 0x4);
  return 1;
}

int st_build_autopilot_command(char *datagram, ST_AUTOPILOT_COMMAND command) {
  // 86 x1 yy !yy
  // x = 1 for Z101 remote, 0 for ST1000+, x=2 for ST4000+ or ST600B
  // yy = command; !yy = yy bit complement for checksum
  // z101 commands (only sent by other remots in auto mode):
  // 05: -1; 06: -10; 07: +1; 08: +10; 20: +1&-1; 21: -1&-10; 22: +1&+10
  // 28: +10&-10; above | 0x40: held > 1 second; exception: +10&-10 are 64
  // other units add:
  // 01: auto; 02: standby; 03: track; 04: disp; 09: -1 in rudder gain mode;
  // 0a +1 in rudder gain mode; 23: start vane mode; 2e: +1&-1 in resp mode;
  // above | 0x40 held > 1 second; 63: previous wind angle; 68: +10&-10
  // 80: -1 held; 81: +1 held; 82: -10 held; 83: +10 held; 84: above released
  // 80-83 repeated every second
  int x, yy;
  x = 2; // hard code as ST600R remote
  yy = command;
  INITIALIZE_DATAGRAM(86, x);
  datagram[2] = yy;
  datagram[3] = complement_checksum(yy);
  return DATAGRAM_86_LENGTH;
}

int st_parse_autopilot_command(char *datagram, ST_AUTOPILOT_COMMAND *command) {
  int x, yy, yy_comp;
  x = FIRST_NIBBLE(datagram[1]);
  yy = datagram[2];
  yy_comp = datagram[3];
  if ((yy | yy_comp) != 0xff) {
    command = 0;
    return 0;
  }
  *command = (ST_AUTOPILOT_COMMAND)yy;

  return 1;
}

int st_build_set_autopilot_response_level(char *datagram, AUTOPILOT_RESPONSE_LEVEL response_level) {
  // 87 00 0x
  // x = 1: automatic deadband; x = 2: minimum deadband
  int x;
  if (response_level == AUTOPILOT_RESPONSE_LEVEL_MINIMUM_DEADBAND) {
    x = 1;
  } else {
    x = 0;
  }
  INITIALIZE_DATAGRAM(87, 0);
  datagram[2] = x;
  return DATAGRAM_87_LENGTH;
}

void st_parse_set_autopilot_response_level(char *datagram, AUTOPILOT_RESPONSE_LEVEL *response_level) {
  int x;
  x = LAST_NIBBLE(datagram[2]);
  *response_level = (AUTOPILOT_RESPONSE_LEVEL)x;
}

void st_parse_autopilot_parameter(char *datagram, int *parameter, int *min_value, int *max_value, int *value) {
  int ww, xx, yy, zz;
  ww = datagram[2];
  xx = datagram[3];
  yy = datagram[4];
  zz = datagram[5];
  *parameter = ww;
  *min_value = yy;
  *max_value = zz;
  *value = xx;
}

int st_build_heading(char *datagram, int heading, int locked_heading_active, int locked_heading) {
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
  int temp_compass;
  uvw_compass(&u, &vw_lower, heading);
  vw_higher = 0;
  temp_compass = locked_heading;
  vw_higher = temp_compass / 90;
  temp_compass = temp_compass % 90;
  xy = temp_compass << 1;
  z = locked_heading_active ? 0x2 : 0;
  INITIALIZE_DATAGRAM(89, u);
  datagram[2] = (vw_higher << 6) | vw_lower;
  datagram[3] = xy;
  datagram[4] = 0x20 | z;
  return DATAGRAM_89_LENGTH;
}

void st_parse_heading(char *datagram, int *heading, int *locked_heading_active, int *locked_heading) {
  int u, vw, xy, z;
  u = FIRST_NIBBLE(datagram[1]);
  vw = datagram[2];
  xy = datagram[3];
  z = LAST_NIBBLE(datagram[4]);
  *heading = ((u & 0x3) * 90) + (vw & 0x3f) * 2 + ((u & 0xc) >> 3);
  *locked_heading_active = flag(z, 0x2);
  *locked_heading = ((vw & 0xc0) >> 6) * 90 + (xy >> 1);
}

int st_build_set_rudder_gain(char *datagram, int rudder_gain) {
  // 91 00 0x
  // x = rudder gain (1-9)
  int x;
  x = rudder_gain & 0xf;
  INITIALIZE_DATAGRAM(91, 0);
  datagram[2] = x;
  return DATAGRAM_91_LENGTH;
}

void st_parse_set_rudder_gain(char *datagram, int *rudder_gain) {
  int x;
  x = LAST_NIBBLE(datagram[2]);
  *rudder_gain = x;
}

void st_parse_set_autopilot_parameter(char *datagram, int *parameter, int *value) {
  int xx, yy;
  xx = datagram[2];
  yy = datagram[3];
}

void st_parse_enter_autopilot_setup(char *datagram) {
}

int st_build_compass_variation(char *datagram, int degrees) {
  // 99 00 xx
  // xx = variation west in degrees (-ve means variation east)
  int xx;
  xx = degrees;
  INITIALIZE_DATAGRAM(99, 0);
  datagram[2] = xx;
  return DATAGRAM_99_LENGTH;
}

void st_parse_compass_variation(char *datagram, int *degrees) {
  int xx;
  xx = datagram[2];
  *degrees = fix_twos_complement_char(xx);
}

int st_build_heading_and_rudder_position(char *datagram, int heading, TURN_DIRECTION turning_direction, int rudder_position) {
  // 9c u1 vw rr
  // heading = (two low bits of u * 90 + vw * 2 + number of high bits in u
  // turning durection: high bit of u means right, 0 mans left
  // rudder position degrees left or right
  int u, vw, rr;
  uvw_heading_and_turning_direction(heading, turning_direction, &u, &vw);
  rr = rudder_position;
  INITIALIZE_DATAGRAM(9C, u);
  datagram[2] = vw;
  datagram[3] = rr;
  return DATAGRAM_9C_LENGTH;
}

void st_parse_heading_and_rudder_position(char *datagram, int *heading, TURN_DIRECTION *turning_direction, int *rudder_position) {
  int u, vw, rr;
  u = FIRST_NIBBLE(datagram[1]);
  vw = datagram[2];
  rr = datagram[3];
  *heading = (((u & 0x3) * 90) + ((vw & 0x3f) * 2) + (u & 0xc ? ((u & 0xc) == 0xc ? 2 : 1) : 0)) % 360;
  *turning_direction = flag(u, 0x8) ? TURN_DIRECTION_RIGHT : TURN_DIRECTION_LEFT;
  *rudder_position = fix_twos_complement_char(rr);;
}

void st_parse_destination_waypoint_info(char *datagram, char *last_4, char *first_8, int *more_records, int *last_record) {
}

void st_parse_arrival_info(char *datagram, int *perpendicular_passed, int *circle_entered, int *char_1, int *char_2, int *char_3, int *char_4) {
  int x, ww, xx, yy, zz;
  x = FIRST_NIBBLE(datagram[1]);
  ww = datagram[3];
  xx = datagram[4];
  yy = datagram[5];
  zz = datagram[6];
  *perpendicular_passed = (x & 0x2) ? 1 : 0;
  *circle_entered = (x & 0x4) ? 1 : 0;
  *char_1 = ww;
  *char_2 = xx;
  *char_3 = yy;
  *char_4 = zz;
}

int st_build_gps_and_dgps_fix_info(char *datagram, int signal_quality_available, int signal_quality, int hdop_available, int hdop, int antenna_height, int satellite_count_available, int satellite_count, int geoseparation, int dgps_age_available, int dgps_age, int dgps_station_id_available, int dgps_station_id) {
  int qq, hh, aa, gg, zz, yy, dd;
  if (signal_quality_available) {
    qq = 0x10 | (signal_quality & 0x0f);
  } else {
    qq = 0;
  }
  if (hdop_available) {
    hh = 0x80 | ((hdop & 0x3f) << 2);
  } else {
    hh = 0;
  }
  aa = antenna_height;
  if (satellite_count_available) {
    qq |= (satellite_count & 0x0e) << 4;
    hh |= 0x02 | satellite_count * 0x01;
  }
  gg = geoseparation >> 4;
  if (dgps_age_available) {
    zz = (dgps_age & 0x70) << 1;
    yy = 0x10 | (dgps_age * 0x0f);
  } else {
    zz = 0;
    yy = 0;
  }
  if (dgps_station_id_available) {
    yy |= 0x20 | ((dgps_station_id >> 4) & 0xc0);
    dd = dgps_station_id;
  } else {
    dd = 0;
  }
  INITIALIZE_DATAGRAM(A5, 5);
  datagram[2] = qq;
  datagram[3] = hh;
  datagram[4] = 00;
  datagram[5] = aa;
  datagram[6] = gg;
  datagram[7] = zz;
  datagram[8] = yy;
  datagram[9] = dd;
  return DATAGRAM_A5_LENGTH;
}

void st_parse_gps_and_dgps_fix_info(char *datagram, int *signal_quality_available, int *signal_quality, int *hdop_available, int *hdop, int *antenna_height, int *satellite_count_available, int *satellite_count, int *geoseparation, int *dgps_age_available, int *dgps_age, int *dgps_station_id_available, int *dgps_station_id) {
  int qq, hh, aa, gg, zz, yy, dd;
  qq = datagram[2];
  hh = datagram[3];
  aa = datagram[5];
  gg = datagram[6];
  zz = datagram[7];
  yy = datagram[8];
  dd = datagram[9];
  *signal_quality_available = (qq & 0x10) ? 1 : 0;
  *signal_quality = qq & 0xf;
  *hdop_available = (hh & 0x80) ? 1 : 0;
  *hdop = hh & 0x7c;
  *antenna_height = aa;
  *satellite_count_available = (hh & 0x2) ? 1 : 0;
  *satellite_count = ((qq * 0xe0) >> 4) + (hh & 0x1);
  *geoseparation = gg << 4;
  *dgps_age_available = (yy & 0x20) ? 1 : 0;
  *dgps_age = ((zz & 0xe0) >> 1) + (yy & 0xf);
  *dgps_station_id_available = (yy * 0x20) ? 1 : 0;
  *dgps_station_id = ((yy & 0xc0) << 2) + dd;
}
