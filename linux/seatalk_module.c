#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sysfs.h>
#include "../seatalk_hardware_layer.h"
#include "../boat_status.h"
#include "../boat_sensor.h"
#include "../seatalk_datagram.h"
#include "../seatalk_command.h"

// utility
int flag_display(char *buf, int value) {
  return sprintf(buf, value ? "on" : "off");
}

int convert_int_to_string(char *buf, int value) {
  return sprintf(buf, "%d", value);
}

int convert_TURN_DIRECTION_to_string(char *buf, TURN_DIRECTION value) {
  switch (value) {
    case TURN_DIRECTION_LEFT:
      return sprintf(buf, "left");
      break;
    default:
      return sprintf(buf, "right");
  }
}

int convert_LATITUDE_HEMISPHERE_to_string(char *buf, LATITUDE_HEMISPHERE hemisphere) {
  switch (hemisphere) {
    case LATITUDE_HEMISPHERE_NORTH:
      return sprintf(buf, "N");
      break;
    default:
      return sprintf(buf, "S");
  }
}

int convert_LONGITUDE_HEMISPHERE_to_string(char *buf, LONGITUDE_HEMISPHERE hemisphere) {
  switch (hemisphere) {
    case LONGITUDE_HEMISPHERE_WEST:
      return sprintf(buf, "W");
      break;
    default:
      return sprintf(buf, "E");
  }
}

int convert_ANGLE_REFERENCE_to_string(char *buf, ANGLE_REFERENCE angle_reference) {
  switch (angle_reference) {
    case ANGLE_REFERENCE_BOAT:
      return sprintf(buf, "right");
      break;
    case ANGLE_REFERENCE_TRUE:
      return sprintf(buf, "true");
      break;
    default:
      return sprintf(buf, "magnetic");
  }
}

int convert_AUTOPILOT_MODE_to_string(char *buf, AUTOPILOT_MODE mode) {
  switch (mode) {
    case AUTOPILOT_MODE_AUTO:
      return sprintf(buf, "auto");
      break;
    case AUTOPILOT_MODE_VANE:
      return sprintf(buf, "vane");
      break;
    case AUTOPILOT_MODE_TRACK:
      return sprintf(buf, "track");
      break;
    default:
      return sprintf(buf, "standby");
  }
}

int convert_string_to_int(int *value_from_string, const char *buf, int count) {
  if (sscanf(buf, "%d", value_from_string) == 1) {
    return count;
  }
  return 0;
}

// status
#define STATUS(PARENT, NAME) static ssize_t sysfs_get_ ## PARENT ## _ ## NAME(struct kobject *kobj, struct kobj_attribute *attr, char *buf);\
struct kobj_attribute PARENT ## _ ## NAME ## _attribute = __ATTR(NAME, 0440, sysfs_get_ ## PARENT ## _ ## NAME, NULL)

#define RAW_IMPLEMENT_STATUS(PARENT, NAME) static ssize_t sysfs_get_ ## PARENT ## _ ## NAME(struct kobject *kobj, struct kobj_attribute *attr, char *buf)

#define IMPLEMENT_STATUS(PARENT, NAME, TYPE, BOAT_STATUS_FUNCTION) RAW_IMPLEMENT_STATUS(PARENT, NAME) {\
  TYPE value;\
  if (BOAT_STATUS_FUNCTION(&value) == 0) {\
    return convert_##TYPE##_to_string(buf, value);\
  }\
  return 0;\
}

#define IMPLEMENT_BOAT_STATUS(NAME, TYPE) IMPLEMENT_STATUS(status, NAME, TYPE, get_##NAME)
#define RAW_BOAT_STATUS(NAME) STATUS(status, NAME)

#define BOAT_STATUS(NAME, TYPE) RAW_BOAT_STATUS(NAME);\
IMPLEMENT_BOAT_STATUS(NAME, TYPE)

#define ALARM_STATUS(NAME, ALARM_NAME) STATUS(alarms, NAME);\
RAW_IMPLEMENT_STATUS(alarms, NAME) {\
  int active_alarms;\
  get_active_alarms(&active_alarms);\
  return flag_display(buf, active_alarms & ALARM_##ALARM_NAME);\
}

#define ENGINE_STATUS(ENGINE_ID, NAME, ENGINE_ID_NAME) STATUS(engine_ ## ENGINE_ID, NAME);\
RAW_IMPLEMENT_STATUS(engine_##ENGINE_ID, NAME) {\
  ENGINE_STATUS engine_status;\
  if (get_engine_status(ENGINE_ID_##ENGINE_ID_NAME, &engine_status) == 0) {\
    return convert_int_to_string(buf, engine_status.NAME);\
  }\
  return 0;\
}

#define ENGINE_STATUSES(NAME) ENGINE_STATUS(single, NAME, SINGLE);\
ENGINE_STATUS(port, NAME, PORT);\
ENGINE_STATUS(starboard, NAME, STARBOARD)

#define POSITION_STATUS(NAME, TYPE) STATUS(position, NAME);\
RAW_IMPLEMENT_STATUS(position, NAME) {\
  POSITION position;\
  if (get_position(&position) == 0) {\
    return convert_##TYPE##_to_string(buf, position.NAME);\
  }\
  return 0;\
}

#define FIX_QUALITY(NAME) STATUS(fix_quality, NAME);\
RAW_IMPLEMENT_STATUS(fix_quality, NAME) {\
  GPS_FIX_QUALITY fix_quality;\
  if (get_gps_fix_quality(&fix_quality) == 0) {\
    return convert_int_to_string(buf, fix_quality.NAME);\
  }\
  return 0;\
}

#define RAW_NAVIGATION_STATUS(NAME) STATUS(navigation, NAME);

#define NAVIGATION_STATUS(NAME, TYPE) STATUS(navigation, NAME);\
RAW_IMPLEMENT_STATUS(navigation, NAME) {\
  NAVIGATION_STATUS navigation_status;\
  if (get_navigation(&navigation_status) == 0) {\
    return convert_##TYPE##_to_string(buf, navigation_status.NAME);\
  }\
  return 0;\
}

#define STRING_NAVIGATION_STATUS(NAME) RAW_NAVIGATION_STATUS(NAME);\
RAW_IMPLEMENT_STATUS(navigation, NAME) {\
  NAVIGATION_STATUS navigation_status;\
  if (get_navigation(&navigation_status) == 0) {\
    return sprintf(buf, "%s", navigation_status.NAME);\
  }\
  return 0;\
}

#define NAVIGATION_POSITION_STATUS(NAME, TYPE) RAW_NAVIGATION_STATUS(waypoint_position_##NAME);\
RAW_IMPLEMENT_STATUS(navigation, waypoint_position_##NAME) {\
  NAVIGATION_STATUS navigation_status;\
  if (get_navigation(&navigation_status) == 0) {\
    return convert_##TYPE##_to_string(buf, navigation_status.waypoint_position.NAME);\
  }\
  return 0;\
}

#define DATE_AND_TIME_STATUS(NAME) STATUS(gmt_date_and_time, NAME);\
RAW_IMPLEMENT_STATUS(gmt_date_and_time, NAME) {\
  DATE_AND_TIME date_and_time;\
  if (get_gmt_date_and_time(&date_and_time) == 0) {\
    return convert_int_to_string(buf, date_and_time.NAME);\
  }\
  return 0;\
}

#define AUTOPILOT_STATUS(NAME, TYPE) STATUS(autopilot, NAME);\
RAW_IMPLEMENT_STATUS(autopilot, NAME) {\
  AUTOPILOT_STATUS autopilot_status;\
  if (get_autopilot(&autopilot_status) == 0) {\
    return convert_##TYPE##_to_string(buf, autopilot_status.NAME);\
  }\
  return 0;\
}

BOAT_STATUS(depth_below_transducer_in_feet_times_10, int);
//BOAT_STATUS(distance_units, DISPLAY_UNIT);
BOAT_STATUS(apparent_wind_angle, int);
BOAT_STATUS(apparent_wind_speed_in_knots_times_10, int);
BOAT_STATUS(turn_direction, TURN_DIRECTION);
BOAT_STATUS(heading, int);
BOAT_STATUS(water_speed_in_knots_times_100, int);
BOAT_STATUS(average_water_speed_in_knots_times_100, int);
BOAT_STATUS(rudder_position_in_degrees_right, int);
BOAT_STATUS(course_over_ground, int);
BOAT_STATUS(speed_over_ground_in_knots_times_100, int);
BOAT_STATUS(trip_mileage_in_nautical_miles_times_100, int);
BOAT_STATUS(total_mileage_in_nautical_miles_times_10, int);
BOAT_STATUS(water_temperature_in_degrees_celsius_times_10, int);
BOAT_STATUS(compass_variation_in_degrees_west, int);

ALARM_STATUS(shallow_water, SHALLOW_WATER);
ALARM_STATUS(deep_water, DEEP_WATER);
ALARM_STATUS(anchor, ANCHOR);
ALARM_STATUS(apparent_wind_angle_low, APPARENT_WIND_ANGLE_LOW);
ALARM_STATUS(apparent_wind_angle_high, APPARENT_WIND_ANGLE_HIGH);
ALARM_STATUS(apparent_wind_speed_low, APPARENT_WIND_SPEED_LOW);
ALARM_STATUS(apparent_wind_speed_high, APPARENT_WIND_SPEED_HIGH);
ALARM_STATUS(true_wind_angle_low, TRUE_WIND_ANGLE_LOW);
ALARM_STATUS(true_wind_angle_high, TRUE_WIND_ANGLE_HIGH);
ALARM_STATUS(true_wind_speed_low, TRUE_WIND_SPEED_LOW);
ALARM_STATUS(true_wind_speed_high, TRUE_WIND_SPEED_HIGH);
ALARM_STATUS(autopilot_off_course, AUTOPILOT_OFF_COURSE);
ALARM_STATUS(autopilot_wind_shift, AUTOPILOT_WIND_SHIFT);

ENGINE_STATUSES(rpm);
ENGINE_STATUSES(pitch_percent);

POSITION_STATUS(hemisphere_latitude, LATITUDE_HEMISPHERE);
POSITION_STATUS(degrees_latitude, int);
POSITION_STATUS(minutes_latitude_times_1000, int);
POSITION_STATUS(hemisphere_longitude, LONGITUDE_HEMISPHERE);
POSITION_STATUS(degrees_longitude, int);
POSITION_STATUS(minutes_longitude_times_1000, int);

FIX_QUALITY(signal_quality_valid);
FIX_QUALITY(signal_quality);
FIX_QUALITY(position_error_valid);
FIX_QUALITY(position_error);
FIX_QUALITY(antenna_height);
FIX_QUALITY(satellite_count_valid);
FIX_QUALITY(satellite_count);
FIX_QUALITY(geoseparation);
FIX_QUALITY(dgps_age_valid);
FIX_QUALITY(dgps_age);
FIX_QUALITY(dgps_station_id_valid);
FIX_QUALITY(dgps_station_id);

STRING_NAVIGATION_STATUS(waypoint_name_last_4);
NAVIGATION_POSITION_STATUS(hemisphere_latitude, LATITUDE_HEMISPHERE);
NAVIGATION_POSITION_STATUS(degrees_latitude, int);
NAVIGATION_POSITION_STATUS(minutes_latitude_times_1000, int);
NAVIGATION_POSITION_STATUS(hemisphere_longitude, LONGITUDE_HEMISPHERE);
NAVIGATION_POSITION_STATUS(degrees_longitude, int);
NAVIGATION_POSITION_STATUS(minutes_longitude_times_1000, int);
NAVIGATION_STATUS(waypoint_bearing, int);
NAVIGATION_STATUS(waypoint_bearing_reference, ANGLE_REFERENCE);
NAVIGATION_STATUS(waypoint_range_in_nautical_miles_times_100, int);
NAVIGATION_STATUS(cross_track_error_in_nautical_miles_times_100, int);

DATE_AND_TIME_STATUS(year);
DATE_AND_TIME_STATUS(month);
DATE_AND_TIME_STATUS(day);
DATE_AND_TIME_STATUS(hour);
DATE_AND_TIME_STATUS(minute);
DATE_AND_TIME_STATUS(second);

AUTOPILOT_STATUS(target_heading, int);
AUTOPILOT_STATUS(mode, AUTOPILOT_MODE);

// sensors
#define SENSOR(PARENT, NAME) static ssize_t sysfs_set_ ## PARENT ## _ ## NAME ## _sensor(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count);\
struct kobj_attribute PARENT ## _ ## NAME ## _attribute = __ATTR(NAME, 0220, NULL, sysfs_set_ ## PARENT ## _ ## NAME ## _sensor)
#define RAW_IMPLEMENT_SENSOR(PARENT, NAME) static ssize_t sysfs_set_ ## PARENT ## _ ## NAME ## _sensor(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)

#define BOAT_SENSOR(NAME, TYPE) SENSOR(sensors, NAME);\
RAW_IMPLEMENT_SENSOR(sensors, NAME) {\
  int result;\
  TYPE value;\
  if ((result = convert_string_to_##TYPE(&value, buf, count)) == count) {\
    update_##NAME##_sensor(value);\
  }\
  return result;\
}

BOAT_SENSOR(heading, int);
BOAT_SENSOR(water_speed_in_knots_times_100, int);
BOAT_SENSOR(apparent_wind_angle, int);
BOAT_SENSOR(apparent_wind_speed_in_knots_times_10, int);
BOAT_SENSOR(depth_below_transducer_in_feet_times_10, int);
BOAT_SENSOR(course_over_ground, int);
BOAT_SENSOR(speed_over_ground_in_knots_times_100, int);
BOAT_SENSOR(water_temperature_in_degrees_celsius_times_10, int);
BOAT_SENSOR(rudder_position_in_degrees_right, int);

// commands

#define COMMAND(PARENT, NAME) static ssize_t sysfs_execute_ ## PARENT ## _ ## NAME ## _command(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count);\
struct kobj_attribute PARENT ## _ ## NAME ## _attribute = __ATTR(NAME, 0220, NULL, sysfs_execute_ ## PARENT ## _ ## NAME ## _command);\
static ssize_t sysfs_execute_ ## PARENT ## _ ## NAME ## _command(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)

#define COMMAND_IS(NAME, ID_NAME) if (strcmp(buf, #NAME) == 0) {\
  command = ST_AUTOPILOT_COMMAND_ ## ID_NAME;\
}

COMMAND(commands, autopilot) {
  ST_AUTOPILOT_COMMAND command;
  COMMAND_IS(auto, AUTO)
  else COMMAND_IS(standby, STANDBY)
  else COMMAND_IS(track, TRACK)
  else COMMAND_IS(disp, DISP)
  else COMMAND_IS(turn_left_1, TURN_LEFT_1)
  else COMMAND_IS(turn_left_10, TURN_LEFT_10)
  else COMMAND_IS(turn_right_1, TURN_RIGHT_1)
  else COMMAND_IS(turn_right_10, TURN_RIGHT_10)
  else COMMAND_IS(decrease_gain, DECREASE_GAIN)
  else COMMAND_IS(increase_gain, INCREASE_GAIN)
  else COMMAND_IS(tack_left, TACK_LEFT)
  else COMMAND_IS(tack_right, TACK_RIGHT)
  else COMMAND_IS(wind_mode, WIND_MODE)
  else COMMAND_IS(track_mode, TRACK_MODE)
  else COMMAND_IS(toggle_response_level, TOGGLE_RESPONSE_LEVEL)
  else COMMAND_IS(return_to_course, RETURN_TO_COURSE)
  else COMMAND_IS(enter_compass_calibration_mode, ENTER_COMPASS_CALIBRATION_MODE)
  else COMMAND_IS(press_track_longer, PRESS_TRACK_LONGER)
  else COMMAND_IS(press_disp_longer, PRESS_DISP_LONGER)
  else COMMAND_IS(press_left_1_longer, PRESS_LEFT_1_LONGER)
  else COMMAND_IS(press_left_10_longer, PRESS_LEFT_10_LONGER)
  else COMMAND_IS(press_right_1_longer, PRESS_RIGHT_1_LONGER)
  else COMMAND_IS(press_right_10_longer, PRESS_RIGHT_10_LONGER)
  else COMMAND_IS(return_to_wind_angle, RETURN_TO_WIND_ANGLE)
  else COMMAND_IS(press_left_10_right_10_longer, PRESS_LEFT_10_RIGHT_10_LONGER)
  else COMMAND_IS(enter_rudder_gain_mode, ENTER_RUDDER_GAIN_MODE)
  else COMMAND_IS(hold_left_1, HOLD_LEFT_1)
  else COMMAND_IS(hold_left_10, HOLD_LEFT_10)
  else COMMAND_IS(hold_right_1, HOLD_RIGHT_1)
  else COMMAND_IS(hold_right_10, HOLD_RIGHT_10)
  else COMMAND_IS(release_held_key, RELEASE_HELD_KEY)
  else {
    return 0;
  }
  return autopilot_remote_keystroke(command);
}

//void state_updated(void) {
//  pr_info("New data on SeaTalk bus");
//}

// sysfs
#define DIRECTORY(PARENT, NAME) struct kobject *PARENT ## _ ## NAME ## _kobj_ref\

#define ROOT_DIRECTORY() DIRECTORY(boat, sensors);\
DIRECTORY(boat, commands);\
DIRECTORY(boat, status)

#define STATUS_DIRECTORY() DIRECTORY(boat_status, alarms);\
DIRECTORY(boat_status, engine_single);\
DIRECTORY(boat_status, engine_port);\
DIRECTORY(boat_status, engine_starboard);\
DIRECTORY(boat_status, position);\
DIRECTORY(boat_status, fix_quality);\
DIRECTORY(boat_status, navigation);\
DIRECTORY(boat_status_navigation, waypoint_position);\
DIRECTORY(boat_status, gmt_date_and_time);\
DIRECTORY(boat_status, autopilot)

#define CREATE_VDIR(PARENT, NAME) PARENT ## _ ## NAME ## _kobj_ref = kobject_create_and_add(#NAME, PARENT ## _kobj_ref)
#define CLEAN_VDIR(PARENT, NAME) kobject_put(PARENT ## _ ## NAME ## _kobj_ref)
#define VFILE(STATUS_SENSOR_COMMAND, PARENT, NAME) if (sysfs_create_file(STATUS_SENSOR_COMMAND ## _ ## PARENT ## _kobj_ref, &PARENT ## _ ## NAME ## _attribute.attr)) {\
  pr_info("Unable to create sysfs file for " #PARENT "_" #NAME);\
  goto clean_ ## PARENT ## _ ## NAME;\
}

#define STATUS_VFILE(NAME) VFILE(boat, status, NAME)
#define ALARM_VFILE(NAME) VFILE(boat_status, alarms, NAME)
#define ENGINE_VFILE(NAME) VFILE(boat_status, engine_single, NAME);\
VFILE(boat_status, engine_port, NAME);\
VFILE(boat_status, engine_starboard, NAME)

#define POSITION_VFILE(NAME) VFILE(boat_status, position, NAME)
#define FIX_QUALITY_VFILE(NAME) VFILE(boat_status, fix_quality, NAME)
#define NAVIGATION_VFILE(NAME) VFILE(boat_status, navigation, NAME)
#define NAVIGATION_POSITION_VFILE(NAME) VFILE(boat_status, navigation_waypoint_position, NAME)
#define GMT_DATE_AND_TIME_VFILE(NAME) VFILE(boat_status, gmt_date_and_time, NAME)
#define AUTOPILOT_VFILE(NAME) VFILE(boat_status, autopilot, NAME)
#define SENSOR_VFILE(NAME) VFILE(boat, sensors, NAME)
#define COMMAND_VFILE(NAME) VFILE(boat, commands, NAME)

#define RAW_CLEAN_VFILE(STATUS_SENSOR_COMMAND, PARENT, NAME) sysfs_remove_file(STATUS_SENSOR_COMMAND ## _ ## PARENT ## _kobj_ref, &PARENT ## _ ## NAME ##_attribute.attr)
#define CLEAN_VFILE(STATUS_SENSOR_COMMAND, PARENT, NAME) clean_ ## PARENT ## _ ## NAME:\
  RAW_CLEAN_VFILE(STATUS_SENSOR_COMMAND, PARENT, NAME)

#define CLEAN_STATUS_VFILE(NAME) CLEAN_VFILE(boat, status, NAME)
#define CLEAN_ALARM_VFILE(NAME) CLEAN_VFILE(boat_status, alarms, NAME)
#define CLEAN_ENGINE_VFILE(NAME) CLEAN_VFILE(boat_status, engine_starboard, NAME);\
CLEAN_VFILE(boat_status, engine_port, NAME);\
CLEAN_VFILE(boat_status, engine_single, NAME)

#define CLEAN_POSITION_VFILE(NAME) CLEAN_VFILE(boat_status, position, NAME)
#define CLEAN_FIX_QUALITY_VFILE(NAME) CLEAN_VFILE(boat_status, fix_quality, NAME)
#define CLEAN_NAVIGATION_VFILE(NAME) CLEAN_VFILE(boat_status, navigation, NAME)
#define CLEAN_NAVIGATION_POSITION_VFILE(NAME) CLEAN_VFILE(boat_status, navigation_waypoint_position, NAME)
#define CLEAN_GMT_DATE_AND_TIME_VFILE(NAME) CLEAN_VFILE(boat_status, gmt_date_and_time, NAME)
#define CLEAN_AUTOPILOT_VFILE(NAME) CLEAN_VFILE(boat_status, autopilot, NAME)
#define CLEAN_SENSOR_VFILE(NAME) CLEAN_VFILE(boat, sensors, NAME)
#define CLEAN_COMMAND_VFILE(NAME) CLEAN_VFILE(boat, commands, NAME)

struct kobject *boat_kobj_ref;

ROOT_DIRECTORY();
STATUS_DIRECTORY();

#define VFILES() STATUS_VFILE(depth_below_transducer_in_feet_times_10);\
  STATUS_VFILE(apparent_wind_angle);\
  STATUS_VFILE(apparent_wind_speed_in_knots_times_10);\
  STATUS_VFILE(turn_direction);\
  STATUS_VFILE(heading);\
  STATUS_VFILE(water_speed_in_knots_times_100);\
  STATUS_VFILE(average_water_speed_in_knots_times_100);\
  STATUS_VFILE(rudder_position_in_degrees_right);\
  STATUS_VFILE(course_over_ground);\
  STATUS_VFILE(speed_over_ground_in_knots_times_100);\
  STATUS_VFILE(trip_mileage_in_nautical_miles_times_100);\
  STATUS_VFILE(total_mileage_in_nautical_miles_times_10);\
  STATUS_VFILE(water_temperature_in_degrees_celsius_times_10);\
  STATUS_VFILE(compass_variation_in_degrees_west);\
\
  ALARM_VFILE(shallow_water);\
  ALARM_VFILE(deep_water);\
  ALARM_VFILE(anchor);\
  ALARM_VFILE(apparent_wind_angle_low);\
  ALARM_VFILE(apparent_wind_angle_high);\
  ALARM_VFILE(apparent_wind_speed_low);\
  ALARM_VFILE(apparent_wind_speed_high);\
  ALARM_VFILE(true_wind_angle_low);\
  ALARM_VFILE(true_wind_angle_high);\
  ALARM_VFILE(true_wind_speed_low);\
  ALARM_VFILE(true_wind_speed_high);\
  ALARM_VFILE(autopilot_off_course);\
  ALARM_VFILE(autopilot_wind_shift);\
\
  ENGINE_VFILE(rpm);\
  ENGINE_VFILE(pitch_percent);\
\
  POSITION_VFILE(hemisphere_latitude);\
  POSITION_VFILE(degrees_latitude);\
  POSITION_VFILE(minutes_latitude_times_1000);\
  POSITION_VFILE(hemisphere_longitude);\
  POSITION_VFILE(degrees_longitude);\
  POSITION_VFILE(minutes_longitude_times_1000);\
\
  FIX_QUALITY_VFILE(signal_quality);\
  FIX_QUALITY_VFILE(position_error);\
  FIX_QUALITY_VFILE(antenna_height);\
  FIX_QUALITY_VFILE(satellite_count);\
  FIX_QUALITY_VFILE(geoseparation);\
  FIX_QUALITY_VFILE(dgps_age);\
  FIX_QUALITY_VFILE(dgps_station_id);\
\
  NAVIGATION_VFILE(waypoint_name_last_4);\
  NAVIGATION_POSITION_VFILE(hemisphere_latitude);\
  NAVIGATION_POSITION_VFILE(degrees_latitude);\
  NAVIGATION_POSITION_VFILE(minutes_latitude_times_1000);\
  NAVIGATION_POSITION_VFILE(hemisphere_longitude);\
  NAVIGATION_POSITION_VFILE(degrees_longitude);\
  NAVIGATION_POSITION_VFILE(minutes_longitude_times_1000);\
  NAVIGATION_VFILE(waypoint_bearing);\
  NAVIGATION_VFILE(waypoint_bearing_reference);\
  NAVIGATION_VFILE(waypoint_range_in_nautical_miles_times_100);\
  NAVIGATION_VFILE(cross_track_error_in_nautical_miles_times_100);\
\
  GMT_DATE_AND_TIME_VFILE(year);\
  GMT_DATE_AND_TIME_VFILE(month);\
  GMT_DATE_AND_TIME_VFILE(day);\
  GMT_DATE_AND_TIME_VFILE(hour);\
  GMT_DATE_AND_TIME_VFILE(minute);\
  GMT_DATE_AND_TIME_VFILE(second);\
\
  AUTOPILOT_VFILE(target_heading);\
  AUTOPILOT_VFILE(mode);\
\
  SENSOR_VFILE(heading);\
  SENSOR_VFILE(water_speed_in_knots_times_100);\
  SENSOR_VFILE(apparent_wind_angle);\
  SENSOR_VFILE(apparent_wind_speed_in_knots_times_10);\
  SENSOR_VFILE(depth_below_transducer_in_feet_times_10);\
  SENSOR_VFILE(course_over_ground);\
  SENSOR_VFILE(speed_over_ground_in_knots_times_100);\
  SENSOR_VFILE(water_temperature_in_degrees_celsius_times_10);\
  SENSOR_VFILE(rudder_position_in_degrees_right);\
\
  COMMAND_VFILE(autopilot)

#undef DIRECTORY
#define DIRECTORY(PARENT, NAME) PARENT ## _ ## NAME ## _kobj_ref = kobject_create_and_add(#NAME, PARENT ## _kobj_ref)

static int init_sysfs(void) {
  boat_kobj_ref = kobject_create_and_add("seatalk", NULL);
  ROOT_DIRECTORY();
  STATUS_DIRECTORY();
  VFILES();

  return 0;

  CLEAN_COMMAND_VFILE(autopilot);

  CLEAN_SENSOR_VFILE(rudder_position_in_degrees_right);
  CLEAN_SENSOR_VFILE(water_temperature_in_degrees_celsius_times_10);
  CLEAN_SENSOR_VFILE(speed_over_ground_in_knots_times_100);
  CLEAN_SENSOR_VFILE(course_over_ground);
  CLEAN_SENSOR_VFILE(depth_below_transducer_in_feet_times_10);
  CLEAN_SENSOR_VFILE(apparent_wind_speed_in_knots_times_10);
  CLEAN_SENSOR_VFILE(apparent_wind_angle);
  CLEAN_SENSOR_VFILE(water_speed_in_knots_times_100);
  CLEAN_SENSOR_VFILE(heading);

  CLEAN_AUTOPILOT_VFILE(mode);
  CLEAN_AUTOPILOT_VFILE(target_heading);

  CLEAN_GMT_DATE_AND_TIME_VFILE(second);
  CLEAN_GMT_DATE_AND_TIME_VFILE(minute);
  CLEAN_GMT_DATE_AND_TIME_VFILE(hour);
  CLEAN_GMT_DATE_AND_TIME_VFILE(day);
  CLEAN_GMT_DATE_AND_TIME_VFILE(month);
  CLEAN_GMT_DATE_AND_TIME_VFILE(year);

  CLEAN_NAVIGATION_VFILE(cross_track_error_in_nautical_miles_times_100);
  CLEAN_NAVIGATION_VFILE(waypoint_range_in_nautical_miles_times_100);
  CLEAN_NAVIGATION_VFILE(waypoint_bearing_reference);
  CLEAN_NAVIGATION_VFILE(waypoint_bearing);
  CLEAN_NAVIGATION_POSITION_VFILE(minutes_longitude_times_1000);
  CLEAN_NAVIGATION_POSITION_VFILE(degrees_longitude);
  CLEAN_NAVIGATION_POSITION_VFILE(hemisphere_longitude);
  CLEAN_NAVIGATION_POSITION_VFILE(minutes_latitude_times_1000);
  CLEAN_NAVIGATION_POSITION_VFILE(degrees_latitude);
  CLEAN_NAVIGATION_POSITION_VFILE(hemisphere_latitude);
  CLEAN_NAVIGATION_VFILE(waypoint_name_last_4);

  CLEAN_FIX_QUALITY_VFILE(dgps_station_id);
  CLEAN_FIX_QUALITY_VFILE(dgps_age);
  CLEAN_FIX_QUALITY_VFILE(geoseparation);
  CLEAN_FIX_QUALITY_VFILE(satellite_count);
  CLEAN_FIX_QUALITY_VFILE(antenna_height);
  CLEAN_FIX_QUALITY_VFILE(position_error);
  CLEAN_FIX_QUALITY_VFILE(signal_quality);

  CLEAN_POSITION_VFILE(minutes_longitude_times_1000);
  CLEAN_POSITION_VFILE(degrees_longitude);
  CLEAN_POSITION_VFILE(hemisphere_longitude);
  CLEAN_POSITION_VFILE(minutes_latitude_times_1000);
  CLEAN_POSITION_VFILE(degrees_latitude);
  CLEAN_POSITION_VFILE(hemisphere_latitude);

  CLEAN_ENGINE_VFILE(pitch_percent);
  CLEAN_ENGINE_VFILE(rpm);

  CLEAN_ALARM_VFILE(autopilot_wind_shift);
  CLEAN_ALARM_VFILE(autopilot_off_course);
  CLEAN_ALARM_VFILE(true_wind_speed_high);
  CLEAN_ALARM_VFILE(true_wind_speed_low);
  CLEAN_ALARM_VFILE(true_wind_angle_high);
  CLEAN_ALARM_VFILE(true_wind_angle_low);
  CLEAN_ALARM_VFILE(apparent_wind_speed_high);
  CLEAN_ALARM_VFILE(apparent_wind_speed_low);
  CLEAN_ALARM_VFILE(apparent_wind_angle_high);
  CLEAN_ALARM_VFILE(apparent_wind_angle_low);
  CLEAN_ALARM_VFILE(anchor);
  CLEAN_ALARM_VFILE(deep_water);
  CLEAN_ALARM_VFILE(shallow_water);

  CLEAN_STATUS_VFILE(compass_variation_in_degrees_west);
  CLEAN_STATUS_VFILE(water_temperature_in_degrees_celsius_times_10);
  CLEAN_STATUS_VFILE(total_mileage_in_nautical_miles_times_10);
  CLEAN_STATUS_VFILE(trip_mileage_in_nautical_miles_times_100);
  CLEAN_STATUS_VFILE(speed_over_ground_in_knots_times_100);
  CLEAN_STATUS_VFILE(course_over_ground);
  CLEAN_STATUS_VFILE(rudder_position_in_degrees_right);
  CLEAN_STATUS_VFILE(average_water_speed_in_knots_times_100);
  CLEAN_STATUS_VFILE(water_speed_in_knots_times_100);
  CLEAN_STATUS_VFILE(heading);
  CLEAN_STATUS_VFILE(turn_direction);
  CLEAN_STATUS_VFILE(apparent_wind_speed_in_knots_times_10);
  CLEAN_STATUS_VFILE(apparent_wind_angle);
//  CLEAN_STATUS_VFILE(distance_units);
  CLEAN_STATUS_VFILE(depth_below_transducer_in_feet_times_10);

  CLEAN_VDIR(boat_status, autopilot);
  CLEAN_VDIR(boat_status, gmt_date_and_time);
  CLEAN_VDIR(boat_status_navigation, waypoint_position);
  CLEAN_VDIR(boat_status, navigation);
  CLEAN_VDIR(boat_status, fix_quality);
  CLEAN_VDIR(boat_status, position);
  CLEAN_VDIR(boat_status, engine_starboard);
  CLEAN_VDIR(boat_status, engine_port);
  CLEAN_VDIR(boat_status, engine_single);
  CLEAN_VDIR(boat_status, alarms);
  CLEAN_VDIR(boat, status);
  CLEAN_VDIR(boat, commands);
  CLEAN_VDIR(boat, sensors);
  kobject_put(boat_kobj_ref);
  return 1;
}

static void exit_sysfs(void) {

#undef VFILE
#define VFILE(STATUS_SENSOR_COMMAND, PARENT, NAME) RAW_CLEAN_VFILE(STATUS_SENSOR_COMMAND, PARENT, NAME)

  VFILES();

#undef DIRECTORY
#define DIRECTORY(PARENT, NAME) kobject_put(PARENT ## _ ## NAME ## _kobj_ref)

  STATUS_DIRECTORY();
  ROOT_DIRECTORY();
  kobject_put(boat_kobj_ref);
}

static int __init init_seatalk_module(void) {
  if (init_sysfs()) {
    pr_info("Unable to initialize sysfs. Exiting.");
    goto cleanup;
  }
  if (init_seatalk_hardware_signal()) {
    pr_info("Unable to initialize SeaTalk communications layer. Exiting.");
    goto cleanup;
  }
  if (init_seatalk_hardware_irq()) {
    pr_info("Unable to initialize Seatalk IRQ. Exiting");
    goto cleanup_irq;
  }
  pr_info("Seatalk module initialization complete.");
  return 0;

cleanup_irq:
  exit_seatalk_hardware_signal();

cleanup:
  return -1;
}

void __exit exit_seatalk_module(void) {
  pr_info("Exiting Seatalk module");
  exit_seatalk_hardware_irq();
  exit_seatalk_hardware_signal();
  exit_sysfs();
  pr_info("Seatalk module cleanup complete.");
}

module_init(init_seatalk_module);
module_exit(exit_seatalk_module);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("James Roscoe <james.roscoe@me.com>");
MODULE_DESCRIPTION("Open-source SeaTalk (Raymarine trademark) communicator. Thanks to Thomas Knauf for sharing the protocol and hardware details at his website, http://www.thomasknauf.de/seatalk.htm");
MODULE_VERSION("1.0");
