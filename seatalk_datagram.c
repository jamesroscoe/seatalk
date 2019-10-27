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

int two_bytes(char *datagram, int index) {
  int byte1, byte2;
  byte1 = datagram[index];
  byte2 = datagram[index + 1];
  return (byte1 << 8) | byte2;
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

int flag(char flags, char mask) {
  return (flags & mask) ? 1 : 0;
}

int get_datagram_length(char second_byte) {
  return last_nibble(second_byte);
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

int alarm_is_active(int active_alarms, int alarm) {
  return (active_alarms & alarm) ? 0xffff : 0;
}

int build_depth_below_transducer(char *datagram, int depth_in_feet_times_10, int display_in_metres, int active_alarms, int transducer_defective) {
  // 00 02 yz xx xx
  // depth below transducer (in feet): xxxx/10
  // anchor alarm: y & 0x8
  // metric display: y & 4
  // deep water alarm: z & 2
  // shallow water alarm: z & 1
  int xxxx = depth_in_feet_times_10;
  int y = (alarm_is_active(active_alarms, ANCHOR_ALARM) & 0x8) | (display_in_metres ? 0x4 : 0x0);
  int z = (alarm_is_active(active_alarms, DEEP_WATER_ALARM) & 0x2) | (alarm_is_active(active_alarms, SHALLOW_WATER_ALARM) & 0x1) | (transducer_defective ? 0x4 : 0);;
  INITIALIZE_DATAGRAM(00, 0);
  datagram[2] = (y << 4) | z;
  datagram[3] = xxxx >> 8;
  datagram[4] = xxxx & 0xff;
  return DATAGRAM_00_LENGTH;
}

void parse_depth_below_transducer(char *datagram, int *depth_in_feet_times_10, int *display_units, int *active_alarms, int *transducer_defective) {
  int y, z, xxxx;
  y = first_nibble(datagram[2]);
  z = last_nibble(datagram[2]);
  xxxx = two_bytes(datagram, 3);
  *depth_in_feet_times_10 = xxxx;
  *display_units = flag(y, 0x4);
  *active_alarms = 0;
  *active_alarms |= flag(z, 0x1) ? SHALLOW_WATER_ALARM : 0;
  *active_alarms |= flag(z, 0x2) ? DEEP_WATER_ALARM : 0;
  *active_alarms |= flag(y, 0x8) ? ANCHOR_ALARM : 0;
  *transducer_defective = flag(z, 0x4);
}

void update_depth_below_transducer(char *datagram) {
  int depth_in_feet_times_10, display_in_metres, active_alarms, transducer_defective;
  parse_depth_below_transducer(datagram, &depth_in_feet_times_10, &display_in_metres, &active_alarms, &transducer_defective);
//  set_depth_status(depth_in_feet_times_10, display_in_metres, active_alarms, transducer_defective);
}

int build_engine_rpm_and_pitch(char *datagram, enum ENGINE_ID engine_id, int rpm, int pitch_percent) {
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

void parse_engine_rpm_and_pitch(char *datagram, enum ENGINE_ID *engine_id, int *rpm, int *pitch_percent) {
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

void update_engine_rpm_and_pitch(char *datagram) {
  enum ENGINE_ID engine_id;
  int rpm, pitch_percent;
  parse_engine_rpm_and_pitch(datagram, &engine_id, &rpm, &pitch_percent);
//  set_engine_rpm_and_pitch(engine_id, rpm, pitch_percent);
}

int build_apparent_wind_angle(char *datagram, int degrees_right_times_2) {
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

void parse_apparent_wind_angle(char *datagram, int *degrees_right_times_2) {
  int xxyy;
  xxyy = two_bytes(datagram, 2);
  *degrees_right_times_2 = fix_twos_complement_word(xxyy);
}

void update_apparent_wind_angle(char *datagram) {
  int degrees_right_times_2;
  parse_apparent_wind_angle(datagram, &degrees_right_times_2);
//  set_apparent_wind_angle(degrees_right_times_2);
}

int build_apparent_wind_speed(char *datagram, int knots_times_10, int display_in_metric) {
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

void parse_apparent_wind_speed(char *datagram, int *knots_times_10, int *display_in_metric) {
  int xx, y;
  xx = datagram[2];
  y = last_nibble(datagram[3]);
  *knots_times_10 = (xx & 0x7f) * 10 + y;
  *display_in_metric = flag(xx, 0x80);
}

void update_apparent_wind_speed(char *datagram) {
  int knots_times_10, display_in_metric;
  parse_apparent_wind_speed(datagram, &knots_times_10, &display_in_metric);
//  set_apparent_wind_speed(knots_times_10);
}

int build_water_speed(char *datagram, int knots_times_10) {
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

void parse_water_speed(char *datagram, int *knots_times_10) {
  int xxxx;
  xxxx = two_bytes(datagram, 2);
  *knots_times_10 = xxxx;
}

void update_water_speed(char *datagram) {
  int knots_times_10;
  parse_water_speed(datagram, &knots_times_10);
//  set_water_speed(knots_times_10);
}

int build_trip_mileage(char *datagram, int nmiles_times_100) {
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

void parse_trip_mileage(char *datagram, int *miles_times_100) {
  int xxxxx;
  xxxxx = (two_bytes(datagram, 2) << 4) | last_nibble(datagram[4]);
  *miles_times_100 = xxxxx;
}

void update_trip_mileage(char *datagram) {
  int miles_times_100;
  parse_trip_mileage(datagram, &miles_times_100);
//  set_trip_mileage(miles_times_100);
}

int build_total_mileage(char *datagram, int nmiles_times_10) {
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

void parse_total_mileage(char *datagram, int *miles_times_10) {
  int xxxx;
  xxxx = two_bytes(datagram, 2);
  *miles_times_10 = xxxx;
}

void update_total_mileage(char *datagram) {
  int miles_times_10;
  parse_total_mileage(datagram, &miles_times_10);
//  set_total_mileage(miles_times_10);
}

int build_water_temperature(char *datagram, int degrees_fahrenheit, int transducer_defective) {
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

void parse_water_temperature(char *datagram, int *degrees_fahrenheit, int *transducer_defective) {
  int xx, yy, z;
  xx = datagram[2];
  yy = datagram[3];
  z = first_nibble(datagram[1]);
  *degrees_fahrenheit = yy;;
  *transducer_defective = flag(z, 0x4);
}

void update_water_temperature(char *datagram) {
  int celsius, transducer_defective;
  parse_water_temperature(datagram, &celsius, &transducer_defective);
//  set_water_temperature(celsius);
}

int build_speed_distance_units(char *datagram, enum DISTANCE_UNITS distance_units) {
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

void parse_speed_distance_units(char *datagram, enum DISTANCE_UNITS *distance_units) {
  int xx;
  if (xx == 0) {
    *distance_units = DISTANCE_UNITS_NAUTICAL;
  } else if (xx == 6) {
    *distance_units = DISTANCE_UNITS_STATUTE;
  } else {
    *distance_units = DISTANCE_UNITS_METRIC;
  }
}

void update_speed_distance_units(char *datagram) {
  enum DISTANCE_UNITS distance_units;
  parse_speed_distance_units(datagram, &distance_units);
//  set_speed_distance_units(units);
}

int build_total_and_trip_mileage(char *datagram, int total_nm_times_10, int trip_nm_times_100) {
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

void parse_total_and_trip_mileage(char *datagram, int *total_mileage_times_10, int *trip_mileage_times_100) {
  int xx, yy, uu, vv, z, w;
  xx = datagram[2];
  yy = datagram[3];
  uu = datagram[4];
  vv = datagram[5];
  z = first_nibble(datagram[1]);
  w = last_nibble(datagram[6]);
  *total_mileage_times_10 = (z << 16) | (yy << 8) | xx;
  *trip_mileage_times_100 = (w << 16) | (vv << 8) | uu;
}

void update_total_and_trip_mileage(char *datagram) {
  int total_mileage_times_10, trip_mileage_times_100;
  parse_total_and_trip_mileage(datagram, &total_mileage_times_10, &trip_mileage_times_100);
//  set_total_and_trip_mileage(total_mileage_times_10, trip_mileage_times_100);
}

int build_average_water_speed(char *datagram, int knots_1_times_100, int knots_2_times_100, int speed_1_from_sensor, int speed_2_is_average, int average_is_stopped, int display_in_statute_miles) {
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

void parse_average_water_speed(char *datagram, int *knots_1_times_100, int *knots_2_times_100, int *speed_1_from_sensor, int *speed_2_is_average, int *average_is_stopped, int *display_in_statute_miles) {
  int xxxx, yyyy, d, e;
  xxxx = two_bytes(datagram, 2);
  yyyy = two_bytes(datagram, 4);
  d = first_nibble(datagram[6]);
  e = last_nibble(datagram[6]);
  *knots_1_times_100 = xxxx;
  *knots_2_times_100 = yyyy;
  *speed_1_from_sensor = flag(d, 0x4);
  *speed_2_is_average = flag(d, 0x8);
  *average_is_stopped = flag(e, 0x1);
  *display_in_statute_miles = flag(e, 0x2);
}

void update_average_water_speed(char *datagram) {
  int knots_1_times_100, knots_2_times_100, speed_1_from_sensor, speed_2_is_average, average_is_stopped, units;
  parse_average_water_speed(datagram, &knots_1_times_100, &knots_2_times_100, &speed_1_from_sensor, &speed_2_is_average, &average_is_stopped, &units);
//  set_average_water_speed(knots_1_times_100, knots_2_times_100, speed_1_from_sensor, speed_2_is_average, average_calculation_stopped, units);
}

int build_precise_water_temperature(char *datagram, int degrees_celsius_times_10) {
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

void parse_precise_water_temperature(char *datagram, int *degrees_celsius_times_10) {
  int xxxx;
  xxxx = two_bytes(datagram, 2);
  *degrees_celsius_times_10 = xxxx - 100;
}

void update_precise_water_temperature(char *datagram) {
  int celsius_times_10;
  parse_precise_water_temperature(datagram, &celsius_times_10);
//  set_precise_water_temperature(celsiurs_times_10);
}

int build_lamp_intensity(char *datagram, int intensity) {
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

void parse_lamp_intensity(char *datagram, int *level) {
  int x;
  x = last_nibble(datagram[2]);
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

void update_lamp_intensity(char *datagram) {
  int level;
  parse_lamp_intensity(datagram, &level);
//  set_lamp_intensity(level);
}

int build_cancel_mob(char *datagram) {
  // 36 00 01 no variation
  INITIALIZE_DATAGRAM(36, 0);
  datagram[2] = 1;
  return DATAGRAM_36_LENGTH;
}

void accept_cancel_mob(char *datagram) {
//  set_mob(0);
}

int build_lat_position(char *datagram, int degrees, int minutes_times_100) {
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

void parse_lat_position(char *datagram, int *degrees, int *minutes_times_100) {
  int xx, yyyy;
  xx = datagram[2];
  yyyy = two_bytes(datagram, 3);
  *degrees = (yyyy & 0x8000 ? -1 : 1) * xx;
  *minutes_times_100 = yyyy & 0x7fff;
}

void update_lat_position(char *datagram) {
  int degrees, minutes_times_100;
  parse_lat_position(datagram, &degrees, &minutes_times_100);
//  set_lat_position(degrees, minutes);
}

int build_lon_position(char *datagram, int degrees, int minutes_times_100) {
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

void parse_lon_position(char *datagram, int *degrees, int *minutes_times_100) {
  int xx, yyyy;
  xx = datagram[2];
  yyyy = two_bytes(datagram, 3);
  *degrees = (yyyy & 0x8000 ? -1 : 1) * xx;
  *minutes_times_100 = yyyy & 0x7fff;
}

void update_lon_position(char *datagram) {
  int degrees, minutes_times_100;
  parse_lon_position(datagram, &degrees, &minutes_times_100);
//  set_lon_position(degrees, minutes);
}

int build_speed_over_ground(char *datagram, int knots_times_10) {
  // 52 01 XX XX
  // XXXX/10 is speed over ground in kts
  int xxxx = knots_times_10;
  INITIALIZE_DATAGRAM(52, 0);
  datagram[2] = xxxx >> 8;
  datagram[3] = xxxx & 0xff;
  return DATAGRAM_52_LENGTH;
}

void parse_speed_over_ground(char *datagram, int *knots_times_10) {
  int xxxx;
  xxxx = two_bytes(datagram, 2);
  *knots_times_10 = xxxx;
}

void update_speed_over_ground(char *datagram) {
  int knots_times_10;
  parse_speed_over_ground(datagram, &knots_times_10);
//  set_speed_over_ground(knots_times_10);
}

int build_course_over_ground(char *datagram, int degrees) {
  // 53 U0 VW
  // magnetic course = (U & 0x3) * 90 + (VW & 0x3F) * 2 + (U & 0xC) >> 2
  int u, vw;
  uvw_compass(&u, &vw, degrees);
  INITIALIZE_DATAGRAM(53, u);
  datagram[2] = vw;
  return DATAGRAM_53_LENGTH;
}

void parse_course_over_ground(char *datagram, int *degrees) {
  int u, vw;
  u = first_nibble(datagram[1]);
  vw = datagram[2];
  *degrees = ((u & 0x3) * 90) + ((vw & 0x3f) * 2) + (u >> 3);
}

void update_course_over_ground(char *datagram) {
  int degrees;
  parse_course_over_ground(datagram, &degrees);
//  set_course_over_ground(degrees);
}

void parse_gmt_time(char *datagram, int *hours, int *minutes, int *seconds) {
  int t, rs, hh;
  t = first_nibble(datagram[1]);
  rs = datagram[2];
  hh = datagram[3];
  *hours = hh;
  *minutes = rs >> 2;
  *seconds = ((rs & 0x3) << 4) | t;
}

void update_gmt_time(char *datagram) {
  int hours, minutes, seconds;
  parse_gmt_time(datagram, &hours, &minutes, &seconds);
//  set_gmt_time(hours, minutes, seconds);
}

int parse_autopilot_remote_control_keystroke(char *datagram, int *button_1_value, int *button_2_value, int *pressed_longer, int *held_down, int *z101_remote, int *tiller_pilot) {
  int x, yy, yy_comp;
  x = first_nibble(datagram[1]);
  yy = datagram[2];
  yy_comp = datagram[3];
  *button_1_value = 0;
  *button_2_value = 0;
  *pressed_longer = 0;
  *held_down = 0;
  *z101_remote = 0;
  *tiller_pilot = 0;
  if (yy != !yy_comp) {
    return -1;
  }
  *pressed_longer = yy & 0x40;
  if (x == 1) {
    if ((yy & 0x20) == 0) {
      switch (yy & 0x0f) {
        case 0x05:
          *button_1_value = -1;
          break;
        case 0x06:
          *button_1_value = -10;
          break;
        case 0x07:
          *button_1_value = 1;
          break;
        case 0x08:
          *button_2_value = 10;
          break;
      }
    } else {
      switch (yy & 0x0f) {
        case 0x20:
          *button_1_value = -1;
          *button_2_value = 1;
          break;
        case 0x21:
          *button_1_value = -10;
          *button_2_value = -1;
          break;
        case 0x22:
          *button_1_value = 1;
          *button_2_value = 10;
          break;
        case 0x24:
        case 0x28:
          *button_1_value = -10;
          *button_2_value = 10;
          break;
      }
    }
    *z101_remote = 1;
    return 0;
  } else {
    if (yy & 0x80) {
      switch (yy) {
        case 0x80:
          *button_1_value = -1;
          break;
        case 0x81:
          *button_1_value = 1;
          break;
        case 0x82:
          *button_1_value = -10;
          break;
        case 0x83:
          *button_1_value = 10;
          break;
        case 0x84: // released
          *button_1_value = 0;
          break;
      }
      *held_down = 1;
    } else if ((yy & 0x20) == 0) {
      switch (yy & 0x0f) {
        case 0x01:
          *button_1_value = 101; // auto
          break;
        case 0x02:
          *button_1_value = 102; // standby
          break;
        case 0x03:
          *button_1_value = 103; // track
          break;
        case 0x04:
          *button_1_value = 104; // disp
          break;
        case 0x05:
          *button_1_value = -1;
          break;
        case 0x06:
          *button_1_value = -10;
          break;
        case 0x07:
          *button_1_value = 1;
          break;
        case 0x08:
          *button_1_value = 10;
          break;
      }
    } else {
      switch (yy & 0x0f) {
        case 0x01:
          *button_1_value = -10;
          *button_2_value = -1;
          break;
        case 0x02:
          *button_1_value = 1;
          *button_2_value = 10;
          break;
        case 0x03:
          *button_1_value = 101; // auto
          *button_2_value = 102; // standby
          break;
        case 0x08:
          *button_1_value = -10;
          *button_2_value = 10;
          break;
        case 0x0e:
          *button_1_value = -1;
          *button_2_value = 1;
          break;
      }
    }
    *tiller_pilot = x == 0;
    return 0;
  }
}

void accept_autopilot_remote_control_keystroke(char *datagram) {
//  int button_1_value, button_2_value, pressed_longer, held_down, z101_remote;
//  if (parse_autopilot_remote_control_keystroke(datagram, &button_1_value, &button_2_value, &pressed_longer, &held_down, &z101_remote) == 0) {
//    if (!(pressed_longer || held_down)) {
//      if (button_2_value) { // single key pressed
//        if (button_1_value < 100) { // direction command key pressed
//          change_autopilot_heading(button_1_value);
//        } else {
//          switch (button_1_value) {
//            case 101: // auto
//              set_autopilot_mode(
//      }
//      if (button_1_value && button_2_value) { // key combination
//      }
//    }
//  }
}

void parse_date(char *datagram, int *year, int *month, int *day) {
  int m, dd, yy;
  m = first_nibble(datagram[1]);
  dd = datagram[2];
  yy = datagram[3];
  *year = yy;
  *month = m;
  *day = dd;
}

void update_date(char *datagram) {
  int year, month, day;
  parse_date(datagram, &year, &month, &day);
//  set_date(year, month, day);
}

void parse_satellite_info(char *datagram, int *satellite_count, int *horizontal_dilution_of_position) {
  int s, dd;
  s = first_nibble(datagram[1]);
  dd = datagram[2];
  *satellite_count = s;
  *horizontal_dilution_of_position = dd;
}

void update_satellite_info(char *datagram) {
  int satellite_count, horizontal_dilution_of_position;
  parse_satellite_info(datagram, &satellite_count, &horizontal_dilution_of_position);
//  set_satellite_info(satellite_count, horizontal_dilution_of_position);
}

void parse_lat_lon_position(char *datagram, int *degrees_lat, int *minutes_lat_times_1000, int *degrees_lon, int *minutes_lon_times_1000) {
  int z, la, xxyy, lo, qqrr;
  z = first_nibble(datagram[1]);
  la = datagram[2];
  xxyy = two_bytes(datagram, 3);
  lo = datagram[5];
  qqrr = two_bytes(datagram, 6);
  *degrees_lat = (z & 0x1 ? -1 : 1) * la;
  *minutes_lat_times_1000 = xxyy;
  *degrees_lon = (z & 0x2 ? -1 : 1) * lo;
  *minutes_lon_times_1000 = qqrr;
}

void update_lat_lon_position(char *datagram) {
  int lat, minutes_lat_times_1000, lon, minutes_lon_times_1000;
  parse_lat_lon_position(datagram, &lat, &minutes_lat_times_1000, &lon, &minutes_lon_times_1000);
//  set_lat_lon_position(lat, minutes_lat_times_1000, lon, minutes_lon_times_1000);
}

void parse_set_countdown_timer(char *datagram, int *hours, int *minutes, int *seconds, int *mode) {
  int ss, mm, x, h;
  ss = datagram[2];
  mm = datagram[3];
  x = first_nibble(datagram[4]);
  h = last_nibble(datagram[4]);
  *hours = h;
  *minutes = mm & 0x7f;
  *seconds = ss;
  if (x == 0) {
    if (mm & 0x80) {
      *mode = 1;
    } else {
      *mode = 0;
    }
  } else if (x == 4) {
    *mode = 2;
  } else if (x == 8) {
    *mode = 3;
  }
}

void accept_set_countdown_timer(char *datagram) {
  int hours, minutes, seconds, mode;
  parse_set_countdown_timer(datagram, &hours, &minutes, &seconds, &mode);
//  set_countdown_timer(hours, minutes, seconds, mode);
}

void parse_wind_alarm(char *datagram, int *true_wind, int *angle_low, int *angle_high, int *speed_low, int *speed_high) {
  int x, y;
  x = first_nibble(datagram[2]);
  y = last_nibble(datagram[2]);
  *true_wind = x != 0;
  *angle_low = ((x | y) & 0x8) != 0;
  *angle_high = ((x | y) & 0x4) != 0;
  *speed_low = ((x | y) & 0x2) != 0;
  *speed_high = ((x | y) & 0x1) != 0;
}

void update_wind_alarm(char *datagram) {
  int true_wind, angle_low, angle_high, speed_low, speed_high;
  parse_wind_alarm(datagram, &true_wind, &angle_low, &angle_high, &speed_low, &speed_high);
//  set_wind_alarm(true_wind, angle_low, angle_high, speed_low, speed_high);
}

void parse_alarm_acknowledgement(char *datagram, int *shallow_water, int *deep_water, int *anchor, int *true_wind_high, int *true_wind_low, int *true_wind_angle_high, int *true_wind_angle_low, int *apparent_wind_high, int *apparent_wind_low, int *apparent_wind_angle_high, int *apparent_wind_angle_low) {
  int x;
  int y;
  int all;
  x = first_nibble(datagram[1]);
  y = datagram[2];
  all = y == 0x15;
  *shallow_water = all;
  *deep_water = all;
  *anchor = all;
  *true_wind_high = all;
  *true_wind_low = all;
  *true_wind_angle_high = all;
  *true_wind_angle_low = all;
  *apparent_wind_high = all;
  *apparent_wind_low = all;
  *apparent_wind_angle_high = all;
  *apparent_wind_angle_low = all;
  if (!all) {
    switch (x) {
      case 1:
        *shallow_water = 1;
        break;
      case 2:
        *deep_water = 1;
        break;
      case 3:
        *anchor = 1;
        break;
      case 4:
        *true_wind_high = 1;
        break;
      case 5:
        *true_wind_low = 1;
        break;
      case 6:
        *true_wind_angle_high = 1;
        break;
      case 7:
        *true_wind_angle_low = 1;
        break;
      case 8:
        *apparent_wind_high = 1;
        break;
      case 9:
        *apparent_wind_low = 1;
        break;
      case 10:
        *apparent_wind_angle_high = 1;
        break;
      case 11:
        *apparent_wind_angle_low = 1;
        break;
    }
  }
}

void accept_alarm_acknowledgement(char *datagram) {
  int shallow_water, deep_water, anchor, true_wind_high, true_wind_low, true_wind_angle_high, true_wind_angle_low, apparent_wind_high, apparent_wind_low, apparent_wind_angle_high, apparent_wind_angle_low;
  parse_alarm_acknowledgement(datagram, &shallow_water, &deep_water, &anchor, &true_wind_high, &true_wind_low, &true_wind_angle_high, &true_wind_angle_low, &apparent_wind_high, &apparent_wind_low, &apparent_wind_angle_high, &apparent_wind_angle_low);
//  cancel_alarm(shallow_water, deep_water, anchor, true_wind_high, true_wind_low, true_wind_angle_high, true_wind_angle_low, apparent_wind_high, apparent_wind_low, apparent_wind_angle_high, apparent_wind_angle_low);
}

void accept_set_mob(char *datagram) {
//  set_mob(1);
}

void parse_maxview_keystroke(char *datagram, int *key_1, int *key_2, int *held_longer) {
//  int x, y;
//  x = first_nibble(datagram[2]);
//  y = last_nibble(datagram[2]);
//  *key_1 = 0;
//  *key_2 = 0;
//  *held_longer = (x & 0x4) ? 1 : 0;
//  if (x & 0x2 == 0) { // single keypress
//    switch (y) {
//      case 1:
//        *key_1 = 
}

void accept_maxview_keystroke(char *datagram) {
}

int parse_target_waypoint_name(char *datagram, int *char_1, int *char_2, int *char_3, int *char_4) {
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
  *char_1 = 0x30 + xx & 0x3f;
  *char_2 = 0x30 + (yy & 0xf) << 2 + (xx & 0xc0) >> 6;
  *char_3 = 0x30 + (zz & 0x3) << 4 + (yy & 0xf0) >> 4;
  *char_4 = 0x30 + (zz & 0xfc) >> 2;
  return 0;
}

void update_target_waypoint_name(char *datagram) {
  int char_1, char_2, char_3, char_4;
  char s[5];
  if (parse_target_waypoint_name(datagram, &char_1, &char_2, &char_3, &char_4) == 0) {
    s[0] = char_1;
    s[1] = char_2;
    s[2] = char_3;
    s[3] = char_4;
    s[4] = 0;
//    set_target_waypoint_name(s);
  }
}

void parse_course_computer_failure(char *datagram, int *failure_type) {
  int xx;
  xx = datagram[2];
  switch (xx) {
    case 0:
      *failure_type = 0;
      break;
    case 1:
      *failure_type = 1;
      break;
    case 2:
      *failure_type = 2;
      break;
  }
}

void update_course_computer_failure(char *datagram) {
  int failure_type;
  parse_course_computer_failure(datagram, &failure_type);
//  set_course_computer_failure_type(failure_type);
}

void parse_autopilot_status(char *datagram, int *compass_heading, int *turning_direction, int *target_heading, int *mode, int *off_course_alarm, int *wind_shift_alarm, int *rudder_position, int *heading_display_off, int *no_data, int *large_xte, int *auto_rel) {
  char u, vw, xy, z, m, rr, ss, tt;

  // printf("parse_autopilot_status\n");
  u = first_nibble(datagram[1]);
  vw = datagram[2];
  xy = datagram[3];
  z = last_nibble(datagram[4]);
  m = last_nibble(datagram[5]);
  rr = datagram[6];
  ss = datagram[7];
  tt = datagram[8];

  *compass_heading = ((u & 0x03) * 90) + ((vw & 0x3f) * 2) + ((u & 0x0c) ? (((u & 0x0c) == 0x0c) ? 2 : 1) : 0);
  *turning_direction = (u & 0x08) ? 1 : -1;
  *target_heading = (((vw & 0xc0) >> 5) * 90) + (xy / 2);
  if ((z & 0x02) == 0) {
    *mode = 0;
  } else if (z & 0x02) {
    *mode = 1;
  } else if (z & 0x04) {
    *mode = 2;
  } else {
    *mode = 3;
  }
  *off_course_alarm = flag(m, 0x04);
  *wind_shift_alarm = flag(m, 0x08);
  if (rr & 0xf0) {
    *rudder_position = -(!rr + 1);
  } else {
    *rudder_position = rr;
  }
  *heading_display_off = flag(ss, 0x01);
  *no_data = flag(ss, 0x08);
  *large_xte = flag(ss, 0x10);
  *auto_rel = flag(ss, 0x80);
}

void update_autopilot_status(char *datagram) {
  int compass_heading, turning_direction, target_heading, mode, off_course_alarm, wind_shift_alarm, rudder_position, heading_display_off, no_data, large_xte, auto_rel;
  parse_autopilot_status(datagram, &compass_heading, &turning_direction, &target_heading, &mode, &off_course_alarm, &wind_shift_alarm, &rudder_position, &heading_display_off, &no_data, &large_xte, &auto_rel);
//  set_autopilot_status(compass_heading, turning_direction, target_heading, mode, off_course_alarm, wind_shift_alarm, rudder_position, heading_display_off, no_data, large_xte, auto_rel);
}

int parse_waypoint_navigation(char *datagram, int *cross_track_times_100, int *waypoint_bearing, int *waypoint_distance, int *waypoint_distance_factor, int *direction_to_steer, int *xte_present, int *waypoint_bearing_present, int *waypoint_distance_present, int *large_xte) {
  int x, xx, v, u, z, w, zz, yf, yf_comp;
  int y, f;
  x = first_nibble(datagram[1]);
  xx = datagram[2];
  v = first_nibble(datagram[3]);
  u = last_nibble(datagram[3]);
  z = first_nibble(datagram[4]);
  w = last_nibble(datagram[4]);
  zz = datagram[5];
  yf = datagram[6];
  yf_comp = datagram[8];
  if (yf != !yf_comp) {
    return -1;
  }
  y = first_nibble(yf);
  f = last_nibble(yf);
  *cross_track_times_100 = x << 8 + xx;
  *waypoint_bearing = ((u & 0x3) * 90) + ((v << 4 + w) > 1);
  *waypoint_distance_factor = (y & 0x1) ? 100 : 10;
  *waypoint_distance = z << 8 + zz;
  *direction_to_steer = (y & 4) ? 1 : -1;
  *xte_present = (f & 0x1) ? 1 : 0;
  *waypoint_bearing_present = (f & 0x2) ? 1 : 0;
  *waypoint_distance_present = (f & 0x4) ? 1 : 0;
  *large_xte = (f & 0x8) ? 1 : 0;
  return 0;
}

void update_waypoint_navigation(char *datagram) {
  int cross_track_times_100, waypoint_bearing, waypoint_distance, waypoint_distance_factor, direction_to_steer, xte_present, waypoint_bearing_present, waypoint_distance_present, large_xte;
  if (parse_waypoint_navigation(datagram, &cross_track_times_100, &waypoint_bearing, &waypoint_distance, &waypoint_distance_factor, &direction_to_steer, &xte_present, &waypoint_bearing_present, &waypoint_distance_present, &large_xte) == 0) {
  }
  if (waypoint_distance_factor = 10) {
    waypoint_distance = waypoint_distance * 10;
  }
//  set_waypoint_navigation(waypoint_bearing, waypoint_distance, direction_to_steer, xte_present, waypoint_bearing_present, waypoint_distance_present, large_xte);
}

void parse_set_autopilot_response_level(char *datagram, int *response_level) {
  int x;
  x = last_nibble(datagram[2]);
  *response_level = x;
}

void accept_set_autopilot_response_level(char *datagram) {
  int response_level;
  parse_set_autopilot_response_level(datagram, &response_level);
//  set_autopilot_response_level(response_level);
}

void parse_autopilot_parameter(char *datagram, int *parameter, int *min_value, int *max_value, int *value) {
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

void update_autopilot_parameter(char *datagram) {
  int parameter, min_value, max_value, value;
  parse_autopilot_parameter(datagram, &parameter, &min_value, &max_value, &value);
//  set_autopilot_parameter(parameter, min_value, max_value, value);
}

void parse_heading(char *datagram, int *heading, int *locked_heading_active, int *locked_heading) {
  int u, vw, xy, z;
  u = first_nibble(datagram[1]);
  vw = datagram[2];
  xy = datagram[3];
  z = last_nibble(datagram[4]);
  *heading = ((u & 0x3) * 90) + (vw & 0x3f) * 2 + (u & 0xc) >> 3;
  *locked_heading_active = (z & 0x2) != 0;
  *locked_heading = ((vw * 0xc0) >> 6) * 90 + xy >> 1;
}

void update_heading(char *datagram) {
  int heading, locked_heading_active, locked_heading;
  parse_heading(datagram, &heading, &locked_heading_active, &locked_heading);
//  set_heading(heading, locked_heading_active, locked_heading);
}

void parse_set_rudder_gain(char *datagram, int *rudder_gain) {
  int x;
  x = last_nibble(datagram[2]);
}

void accept_set_rudder_gain(char *datagram) {
  int rudder_gain;
  parse_set_rudder_gain(datagram, &rudder_gain);
//  set_rudder_gain(rudder_gain);
}

void parse_set_autopilot_parameter(char *datagram, int *parameter, int *value) {
  int xx, yy;
  xx = datagram[2];
  yy = datagram[3];
}

void accept_set_autopilot_parameter(char *datagram) {
  int parameter, value;
  parse_set_autopilot_parameter(datagram, &parameter, &value);
}

void parse_enter_autopilot_setup(char *datagram) {
}

void accept_enter_autopilot_setup(char *datagram) {
  parse_enter_autopilot_setup(datagram);
}

void parse_compass_variation(char *datagram, int *compass_variation) {
  int xx;
  xx = datagram[2];
  if (xx & 0x80) { // -ve means variation East
    *compass_variation = -1 * (!xx + 1);
  } else {
    *compass_variation = xx;
  }
}

void update_compass_variation(char *datagram) {
  int compass_variation;
  parse_compass_variation(datagram, &compass_variation);
//  set_compass_variation(compass_variation);
}

void parse_compass_heading_and_rudder_position(char *datagram, int *heading, int *rudder_position) {
  int u, vw, rr;
  u = first_nibble(datagram[1]);
  vw = datagram[2];
  rr = datagram[3];
  *heading = ((u & 0x3) * 90) + ((vw & 0x3f) * 2) + (u & 0xc ? (u & 0xc == 0xc ? 2 : 1) : 0);
  if (rr & 0x8) {
    *rudder_position = -1 * (!rr + 1);
  } else {
    *rudder_position = rr;
  }
}

void update_compass_heading_and_rudder_position(char *datagram) {
  int compass_heading, rudder_position;
  parse_compass_heading_and_rudder_position(datagram, &compass_heading, &rudder_position);
//  set_compass_heading_and_rudder_position(compass_heading, rudder_position);
}

void parse_destination_waypoint_info(char *datagram, char *last_4, char *first_8, int *more_records, int *last_record) {
}

void update_destination_waypoint_info(char *datagram) {
}

void parse_arrival_info(char *datagram, int *perpendicular_passed, int *circle_entered, int *char_1, int *char_2, int *char_3, int *char_4) {
  int x, ww, xx, yy, zz;
  x = first_nibble(datagram[1]);
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

void update_arrival_info(char *datagram) {
  int perpendicular_passed, circle_entered, char_1, char_2, char_3, char_4;
  char name[5];
  parse_arrival_info(datagram, &perpendicular_passed, &circle_entered, &char_1, &char_2, &char_3, &char_4);
  name[0] = char_1;
  name[1] = char_2;
  name[2] = char_3;
  name[3] = char_4;
  name[4] = 0;
//  set_arrival_info(perpendicular_passed, circle_entered, name);
}

void parse_gps_and_dgps_fix_info(char *datagram, int *signal_quality_available, int *signal_quality, int *hdop_available, int *hdop, int *antenna_height, int *satellite_count_available, int *satellite_count, int *geoseparation, int *dgps_age_available, int *dgps_age, int *dgps_station_id_available, int *dgps_station_id) {
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
  *satellite_count = (qq * 0xe0) >> 4 + (hh & 0x1);
  *geoseparation = gg << 4;
  *dgps_age_available = (yy & 0x20) ? 1 : 0;
  *dgps_age = (zz & 0xe0) >> 1 + (yy & 0xf);
  *dgps_station_id_available = (yy * 0x20) ? 1 : 0;
  *dgps_station_id = ((yy & 0xc0) << 2) + dd;
}

void update_gps_and_dgps_fix_info(char *datagram) {
  int signal_quality_available, signal_quality, hdop_available, hdop, antenna_height, satellite_count_available, satellite_count, geoseparation, dgps_age_available, dgps_age, dgps_station_id_available, dgps_station_id;
  parse_gps_and_dgps_fix_info(datagram, &signal_quality_available, &signal_quality, &hdop_available, &hdop, &antenna_height, &satellite_count_available, &satellite_count, &geoseparation, &dgps_age_available, &dgps_age, &dgps_station_id_available, &dgps_station_id);
//  set_gps_and_dgps_fix_info(signal_quality_available, signal_quality, hdop_available, hdop, antenna_height, satellite_count_available, satellite_count, geoseparation, dgps_age_available, dgps_age, dgps_station_id_available, dgps_station_id);
}

// handle datagram
void handle_seatalk_datagram(char *datagram) {
  // printf("parse_seatalk_datagram: %x", datagram[0]);
  switch (datagram[0]) {
    case 0x00: // depth below transducer
      update_depth_below_transducer(datagram);
      break;
    case 0x01: // equipment ID
      break;
    case 0x05: // engine RPM and blade pitch
      update_engine_rpm_and_pitch(datagram);
      break;
    case 0x10: // apparent wind angle
      update_apparent_wind_angle(datagram);
      break;
    case 0x11: // apparent wind speed
      update_apparent_wind_speed(datagram);
      break;
    case 0x20: // water speed xxxx/10 kt
      update_water_speed(datagram);
      break;
    case 0x21: // trip mileage xxxxx/100 nm
      update_trip_mileage(datagram);
      break;
    case 0x22: // trip mileage xxxx/10 nm
      update_total_mileage(datagram);
      break;
    case 0x23: // water temperature
      update_water_temperature(datagram);
      break;
    case 0x24: // display units for mileage & speed
      update_speed_distance_units(datagram);
      break;
    case 0x25: // total & trip log
      update_total_and_trip_mileage(datagram);
      break;
    case 0x26: // water speed with average
      update_average_water_speed(datagram);
      break;
    case 0x27: // water teperature (xxxx-100)/10 degrees C break;
      update_precise_water_temperature(datagram);
      break;
    case 0x30: // set lamp intensity
      update_lamp_intensity(datagram);
      break;
    case 0x36: // cancel MOB
      accept_cancel_mob(datagram);
      break;
    case 0x38: // codelock data
      break;
    case 0x50: // lat position
      update_lat_position(datagram);
      break;
    case 0x51: // lon position
      update_lon_position(datagram);
      break;
    case 0x52: // speed over ground
      update_speed_over_ground(datagram);
      break;
    case 0x53: // course over ground
      update_course_over_ground(datagram);
      break;
    case 0x54: // gmt time
      update_gmt_time(datagram);
      break;
    case 0x55: // TRACK keystroke on GPS unit
      accept_autopilot_remote_control_keystroke(datagram);
      break;
    case 0x56: // date
      update_date(datagram);
      break;
    case 0x57: // satellite status
      update_satellite_info(datagram);
      break;
    case 0x58: // lat/lon
      update_lat_lon_position(datagram);
      break;
    case 0x59: // set countdown timer
      accept_set_countdown_timer(datagram);
      break;
    case 0x61: // e-80 unit initialization broadcast
      break;
    case 0x65: // select fathom or feed depth display units
      break;
    case 0x66: // wind alarm
      update_wind_alarm(datagram);
      break;
    case 0x68: // alarm acknowledgement keystroke
      accept_alarm_acknowledgement(datagram);
      break;
    case 0x6c: // second equipment id message
      break;
    case 0x6e: // MOB keystroke
      accept_set_mob(datagram);
      break;
    case 0x70: // ST60 remote control keystroke
      accept_maxview_keystroke(datagram);
      break;
    case 0x80: // set lamp intensity
      update_lamp_intensity(datagram); // same as 0x30
      break;
    case 0x81: // set by course computer during setup when going past user cal
      break;
    case 0x82: // target waypoint name
      break;
    case 0x83: // course computer powered up or cleared failure condition
      break;
    case 0x84: // autopilot status broadcast
      update_autopilot_status(datagram);
      break;
    case 0x85: // navigation to waypoint status
      update_waypoint_navigation(datagram);
      break;
    case 0x86: // autopilot command
      accept_autopilot_remote_control_keystroke(datagram);
      break;
    case 0x87: // set autopilot response level
      accept_set_autopilot_response_level(datagram);
      break;
    case 0x88: // autopilot parameter
      update_autopilot_parameter(datagram);
      break;
    case 0x89: // steering compass course
      update_heading(datagram);
      break;
    case 0x90: // device identification
      break;
    case 0x91: // set rudder gain
      accept_set_rudder_gain(datagram);
      break;
    case 0x92: // set autopilot parameter
      accept_set_autopilot_parameter(datagram);
      break;
    case 0x93: // enter autopilot setup
      accept_enter_autopilot_setup(datagram);
      break;
    case 0x95: // replaces 84 while autpilot is in value setting mode
      break;
    case 0x99: // compass variation sent by st40 or autopilot
      update_compass_variation(datagram);
      break;
    case 0x9a: // version string
      break;
    case 0x9c: // compass heading and rudder position
      update_compass_heading_and_rudder_position(datagram);
      break;
    case 0x9e: // waypoint definition
      break;
    case 0xa1: // destination waypoint info
      break;
    case 0xa2: // waypoint arrival notice
      update_arrival_info(datagram);
      break;
    case 0xa4: // broadcast query to identify all devices on the bus
      break;
    case 0xa5: // GPS and DGPS info
      update_gps_and_dgps_fix_info(datagram);
      break;
    case 0xa7: // unknown
      break;
    case 0xa8: // alarm notice for guard #1 or guard #2
      break;
    case 0xab: // alarm notice for guard #1 or guard #2
      break;
  }
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
