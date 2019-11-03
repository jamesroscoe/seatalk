#include "logger.h"
#include "timeout.h"
#include "boat_status.h"
#include "settings.h"

#define STATUS_VARIABLE(NAME) status_ ## NAME
#define STATUS_EXPIRY_TIME(NAME) status_##NAME##_expiry
#define VALID_STATUS(NAME) timeout_still_valid(STATUS_EXPIRY_TIME(NAME))
#define SET_STATUS_EXPIRY(NAME, VALUE) STATUS_EXPIRY_TIME(NAME) = VALUE
#define VALIDATE_STATUS(NAME) restart_timeout(&STATUS_EXPIRY_TIME(NAME), STATUS_TIME_TO_LIVE)
#define INVALIDATE_STATUS(NAME) invalidate_timeout(&STATUS_EXPIRY_TIME(NAME))
#define DEFINE_STATUS_EXPIRY(NAME) timeout STATUS_EXPIRY_TIME(NAME) = TIMEOUT_DEFAULT

#define DEFINE_RAW_STATUS(NAME, TYPE) TYPE STATUS_VARIABLE(NAME);\

#define DEFINE_READ_ONLY_STATUS(NAME, TYPE) DEFINE_RAW_STATUS(NAME, TYPE)\
  DEFINE_STATUS_EXPIRY(NAME);\
int get_##NAME(TYPE *NAME) {\
  if (VALID_STATUS(NAME)) {\
    *NAME = STATUS_VARIABLE(NAME);\
    return 0;\
  }\
  return 1;\
}

#define DEFINE_STATUS(NAME, TYPE) DEFINE_READ_ONLY_STATUS(NAME, TYPE)\
void set_##NAME(TYPE NAME) {\
  STATUS_VARIABLE(NAME) = NAME;\
  VALIDATE_STATUS(NAME);\
}

// status properties are designed to be read from the SeaTalk bus but not written out
DEFINE_STATUS(active_alarms, int)
DEFINE_STATUS(depth_below_transducer_in_feet_times_10, int)
//DEFINE_STATUS(distance_units, enum DISTANCE_UNITS)
DEFINE_READ_ONLY_STATUS(engine_single, struct ENGINE_STATUS)
DEFINE_READ_ONLY_STATUS(engine_port, struct ENGINE_STATUS)
DEFINE_READ_ONLY_STATUS(engine_starboard, struct ENGINE_STATUS)
DEFINE_STATUS(apparent_wind_angle, int)
DEFINE_STATUS(apparent_wind_speed_in_knots_times_10, int)
DEFINE_STATUS(heading, int)
DEFINE_STATUS(heading_reference, enum ANGLE_REFERENCE)
DEFINE_STATUS(turn_direction, enum TURN_DIRECTION)
DEFINE_STATUS(water_speed_in_knots_times_100, int)
DEFINE_STATUS(average_water_speed_in_knots_times_100, int)
DEFINE_STATUS(rudder_position_in_degrees_right, int)
DEFINE_STATUS(course_over_ground, int)
DEFINE_STATUS(course_over_ground_reference, enum ANGLE_REFERENCE)
DEFINE_STATUS(speed_over_ground_in_knots_times_100, int)
DEFINE_STATUS(trip_mileage_in_nautical_miles_times_100, int)
DEFINE_STATUS(total_mileage_in_nautical_miles_times_10, int)
DEFINE_STATUS(water_temperature_in_degrees_celsius_times_10, int)
//DEFINE_STATUS(lamp_intensity, int)
DEFINE_RAW_STATUS(position, struct POSITION)
DEFINE_STATUS_EXPIRY(position_latitude);
DEFINE_STATUS_EXPIRY(position_longitude);
DEFINE_STATUS(compass_variation, int);
DEFINE_STATUS(gps_fix_quality, struct GPS_FIX_QUALITY);
DEFINE_STATUS(navigation, struct NAVIGATION_STATUS);
DEFINE_RAW_STATUS(gmt_date_and_time, struct DATE_AND_TIME)
DEFINE_STATUS_EXPIRY(gmt_date);
DEFINE_STATUS_EXPIRY(gmt_time);
DEFINE_READ_ONLY_STATUS(autopilot, struct AUTOPILOT_STATUS);

void initialize_status(void) {
  INVALIDATE_STATUS(active_alarms);
  INVALIDATE_STATUS(depth_below_transducer_in_feet_times_10);
  INVALIDATE_STATUS(engine_single);
  INVALIDATE_STATUS(engine_port);
  INVALIDATE_STATUS(engine_starboard);
  INVALIDATE_STATUS(apparent_wind_angle);
  INVALIDATE_STATUS(apparent_wind_speed_in_knots_times_10);
  INVALIDATE_STATUS(heading);
  INVALIDATE_STATUS(heading_reference);
  INVALIDATE_STATUS(turn_direction);
  INVALIDATE_STATUS(water_speed_in_knots_times_100);
  INVALIDATE_STATUS(average_water_speed_in_knots_times_100);
  INVALIDATE_STATUS(rudder_position_in_degrees_right);
  INVALIDATE_STATUS(course_over_ground);
  INVALIDATE_STATUS(course_over_ground_reference);
  INVALIDATE_STATUS(speed_over_ground_in_knots_times_100);
  INVALIDATE_STATUS(trip_mileage_in_nautical_miles_times_100);
  INVALIDATE_STATUS(total_mileage_in_nautical_miles_times_10);
  INVALIDATE_STATUS(water_temperature_in_degrees_celsius_times_10);
//  INVALIDATE_STATUS(lamp_intensity);
  INVALIDATE_STATUS(position_latitude);
  INVALIDATE_STATUS(position_longitude);
  INVALIDATE_STATUS(compass_variation);
  INVALIDATE_STATUS(gps_fix_quality);
  INVALIDATE_STATUS(navigation);
  INVALIDATE_STATUS(gmt_date);
  INVALIDATE_STATUS(gmt_time);
  INVALIDATE_STATUS(autopilot);
}

//int copy_string(char *dest, char *source, int max_length) {
//  #ifdef TESTING
//    strcpy(dest, source);
//    return strlen(dest);
//  #else
//    return strscpy(dest, source, max_length);
//  #endif
//}

//void clear_string(char *s) {
//  s[0] = 0;
//}

//int flag_value(int flag) {
//  return flag ? 1 : 0;
//}

//int itoa(char* buf, int val)
//{
//    const unsigned int radix = 10;
//
//    char* p;
//    unsigned int a;        //every digit
//    int len;
//    char* b;            //start of the digit char
//    char temp;
//    unsigned int u;
//
//    p = buf;
//
//    if (val < 0)
//    {
//        *p++ = '-';
//        val = 0 - val;
//    }
//    u = (unsigned int)val;
//
//    b = p;
//
//    do
//    {
//        a = u % radix;
//        u /= radix;
//
//        *p++ = a + '0';
//
//    } while (u > 0);
//
//    len = (int)(p - buf);
//
//    *p-- = 0;
//
//    //swap
//    do
//    {
//        temp = *p;
//        *p = *b;
//        *b = temp;
//        --p;
//        ++b;
//
//    } while (b < p);
//
//    return len;
//}

int get_engine_status(enum ENGINE_ID engine_id, struct ENGINE_STATUS *engine_status) {
  switch (engine_id) {
    case ENGINE_ID_SINGLE:
      return get_engine_single(engine_status);
      break;
    case ENGINE_ID_PORT:
      return get_engine_port(engine_status);
      break;
    case ENGINE_ID_STARBOARD:
      return get_engine_starboard(engine_status);
      break;
  }
  return -1;
}

void set_engine_rpm(enum ENGINE_ID engine_id, int rpm) {
  switch (engine_id) {
    case ENGINE_ID_SINGLE:
      STATUS_VARIABLE(engine_single).rpm = rpm;
      VALIDATE_STATUS(engine_single);
      break;
    case ENGINE_ID_PORT:
      STATUS_VARIABLE(engine_port).rpm = rpm;
      VALIDATE_STATUS(engine_port);
      break;
    case ENGINE_ID_STARBOARD:
      STATUS_VARIABLE(engine_starboard).rpm = rpm;
      VALIDATE_STATUS(engine_starboard);
      break;
  }
}

void set_engine_pitch_percent(enum ENGINE_ID engine_id, int pitch_percent) {
  switch (engine_id) {
    case ENGINE_ID_SINGLE:
      STATUS_VARIABLE(engine_single).pitch_percent = pitch_percent;
      VALIDATE_STATUS(engine_single);
      break;
    case ENGINE_ID_PORT:
      STATUS_VARIABLE(engine_port).pitch_percent = pitch_percent;
      VALIDATE_STATUS(engine_port);
      break;
    case ENGINE_ID_STARBOARD:
      STATUS_VARIABLE(engine_starboard).pitch_percent = pitch_percent;
      VALIDATE_STATUS(engine_starboard);
      break;
  }
}

int get_position(struct POSITION *position) {
  if (VALID_STATUS(position_latitude) && VALID_STATUS(position_longitude)) {
    *position = STATUS_VARIABLE(position);
    return 0;
  }
  return -1;
}

void set_position_latitude(int hemisphere, int degrees, int minutes_times_1000) {
  STATUS_VARIABLE(position).hemisphere_latitude = hemisphere;
  STATUS_VARIABLE(position).degrees_latitude = degrees;
  STATUS_VARIABLE(position).minutes_latitude_times_1000 = minutes_times_1000;
  VALIDATE_STATUS(position_longitude);
}

void set_position_longitude(int hemisphere, int degrees, int minutes_times_1000) {
  STATUS_VARIABLE(position).hemisphere_longitude = hemisphere;
  STATUS_VARIABLE(position).degrees_longitude = degrees;
  STATUS_VARIABLE(position).minutes_longitude_times_1000 = minutes_times_1000;
  VALIDATE_STATUS(position_longitude);
}

int get_gmt_date_and_time(struct DATE_AND_TIME *date_and_time) {
  if (VALID_STATUS(gmt_date) && VALID_STATUS(gmt_time)) {
    *date_and_time = STATUS_VARIABLE(gmt_date_and_time);
    return 0;
  }
  return -1;
}

void set_gmt_date(int year, int month, int day) {
  STATUS_VARIABLE(gmt_date_and_time).year = year;
  STATUS_VARIABLE(gmt_date_and_time).month = month;
  STATUS_VARIABLE(gmt_date_and_time).day = day;
  VALIDATE_STATUS(gmt_date);
}

void set_gmt_time(int hour, int minute, int second) {
  STATUS_VARIABLE(gmt_date_and_time).hour = hour;
  STATUS_VARIABLE(gmt_date_and_time).minute = minute;
  STATUS_VARIABLE(gmt_date_and_time).second = second;
  VALIDATE_STATUS(gmt_time);
}

void set_autopilot_status(int target_heading, enum AUTOPILOT_MODE mode, enum AUTOPILOT_RESPONSE_LEVEL response_level) {
  STATUS_VARIABLE(autopilot).target_heading = target_heading;
  STATUS_VARIABLE(autopilot).mode = mode;
  STATUS_VARIABLE(autopilot).response_level = response_level;
  VALIDATE_STATUS(autopilot);
}
