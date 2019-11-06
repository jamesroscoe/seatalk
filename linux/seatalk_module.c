#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sysfs.h>
#include "../seatalk_hardware_layer.h"
#include "../boat_status.h"

// sysfs
struct kobject *boat_kobj_ref;
struct kobject *sensors_kobj_ref;
struct kobject *commands_kobj_ref;
struct kobject *status_kobj_ref;
struct kobject *alarms_kobj_ref;
struct kobject *engine_single_kobj_ref;
struct kobject *engine_port_kobj_ref;
struct kobject *engine_startboard_kobj_ref;
struct kobject *position_kobj_ref;
struct kobject *fix_quality_kobj_ref;
struct kobject *navigation_kobj_ref;
struct kobject *gmt_date_and_time_kobj_ref;
struct kobject *autopilot_kobj_ref;

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
#define SENSOR(NAME) static ssize_t sysfs_set_ ## NAME ## _sensor(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count);\
struct kobj_attribute NAME ## _sensor_attribute = __ATTR(NAME, 0220, NULL, sysfs_set_ ## NAME ## _sensor)
#define IMPLEMENT_SENSOR(NAME) static ssize_t sysfs_set_ ## NAME ## _sensor(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)

//SENSOR(compass);

// implement sysfs get/set functions
//IMPLEMENT_SENSOR(compass) {
//  int heading;
//  char *end_pointer;
//  pr_info("set_compass_sensor: %s", buf);
//  heading = simple_strtol(buf, &end_pointer, 10);
//  set_compass_sensor(heading);
//  return count;
//}

void state_updated(void) {
  pr_info("New data on SeaTalk bus");
}

static int init_sysfs(void) {
  boat_kobj_ref = kobject_create_and_add("boat", NULL);
  sensors_kobj_ref = kobject_create_and_add("sensors", boat_kobj_ref);
  status_kobj_ref = kobject_create_and_add("status", boat_kobj_ref);
//  autopilot_status_kobj_ref = kobject_create_and_add("autopilot", status_kobj_ref);
//  alarms_kobj_ref = kobject_create_and_add("alarms", seatalk_kobj_ref);
//  if (sysfs_create_file(sensors_kobj_ref, &compass_sensor_attribute.attr)) {
//    pr_info("Unable to create sysfs file for compass sensor");
//    goto clean_compass_sensor;
//  }
//  if (sysfs_create_file(autopilot_status_kobj_ref, &autopilot_compass_heading_attribute.attr)) {
//    pr_info("Unable to create sysfs file for autopilot compass heading");
//    goto clean_autopilot_compass_heading;
//  }
//  if (sysfs_create_file(autopilot_status_kobj_ref, &autopilot_turning_direction_attribute.attr)) {
//    pr_info("Unable to create sysfs file for autopilot turning direction");
//    goto clean_autopilot_turning_direction;
//  }
//  if (sysfs_create_file(autopilot_status_kobj_ref, &autopilot_target_heading_attribute.attr)) {
//    pr_info("Unable to create sysfs file for autopilot target heading");
//    goto clean_autopilot_target_heading;
//  }
//  if (sysfs_create_file(autopilot_status_kobj_ref, &autopilot_mode_attribute.attr)) {
//    pr_info("Unable to create sysfs file for autopilot mode");
//    goto clean_autopilot_mode;
//  }
//  if (sysfs_create_file(alarms_kobj_ref, &alarms_autopilot_off_course_attribute.attr)) {
//    pr_info("Unable to create sysfs file for autopilot off course alarm");
//    goto clean_autopilot_off_course_alarm;
//  }
//  if (sysfs_create_file(alarms_kobj_ref, &alarms_autopilot_wind_shift_attribute.attr)) {
//    pr_info("Unable to create sysfs file for autopilot wind shift alarm");
//    goto clean_autopilot_wind_shift_alarm;
//  }
//  if (sysfs_create_file(autopilot_status_kobj_ref, &autopilot_rudder_position_attribute.attr)) {
//    pr_info("Unable to create sysfs file for autopilot rudder position");
//    goto clean_autopilot_rudder_position;
//  }
//  if (sysfs_create_file(autopilot_status_kobj_ref, &autopilot_heading_display_off_attribute.attr)) {
//    pr_info("Unable to create sysfs file for autopilot heading display off");
//    goto clean_autopilot_heading_display_off;
//  }
//  if (sysfs_create_file(autopilot_status_kobj_ref, &autopilot_no_data_attribute.attr)) {
//    pr_info("Unable to create sysfs file for autopilot no data");
//    goto clean_autopilot_no_data;
//  }
//  if (sysfs_create_file(autopilot_status_kobj_ref, &autopilot_large_xte_attribute.attr)) {
//    pr_info("Unable to create sysfs file for autopilot large xte");
//    goto clean_autopilot_large_xte;
//  }
//  if (sysfs_create_file(autopilot_status_kobj_ref, &autopilot_auto_rel_attribute.attr)) {
//    pr_info("Unable to create sysfs file for autopilot auto rel");
//    goto clean_autopilot_auto_rel;
//  }
  return 0;

//clean_autopilot_auto_rel:
//  sysfs_remove_file(autopilot_status_kobj_ref, &autopilot_auto_rel_attribute.attr);

//clean_autopilot_large_xte:
//  sysfs_remove_file(autopilot_status_kobj_ref, &autopilot_large_xte_attribute.attr);

//clean_autopilot_no_data:
//  sysfs_remove_file(autopilot_status_kobj_ref, &autopilot_no_data_attribute.attr);

//clean_autopilot_heading_display_off:
//  sysfs_remove_file(autopilot_status_kobj_ref, &autopilot_heading_display_off_attribute.attr);

//clean_autopilot_rudder_position:
//  sysfs_remove_file(autopilot_status_kobj_ref, &autopilot_rudder_position_attribute.attr);

//clean_autopilot_wind_shift_alarm:
//  sysfs_remove_file(alarms_kobj_ref, &alarms_autopilot_wind_shift_attribute.attr);

//clean_autopilot_off_course_alarm:
//  sysfs_remove_file(alarms_kobj_ref, &alarms_autopilot_off_course_attribute.attr);

//clean_autopilot_mode:
//  sysfs_remove_file(autopilot_status_kobj_ref, &autopilot_mode_attribute.attr);

//clean_autopilot_target_heading:
//  sysfs_remove_file(autopilot_status_kobj_ref, &autopilot_target_heading_attribute.attr);

//clean_autopilot_turning_direction:
//  sysfs_remove_file(autopilot_status_kobj_ref, &autopilot_turning_direction_attribute.attr);

//clean_autopilot_compass_heading:
//  sysfs_remove_file(autopilot_status_kobj_ref, &autopilot_compass_heading_attribute.attr);

//clean_compass_sensor:
//  sysfs_remove_file(sensors_kobj_ref, &compass_sensor_attribute.attr);
//  kobject_put(alarms_kobj_ref);
//  kobject_put(autopilot_status_kobj_ref);
  kobject_put(status_kobj_ref);
  kobject_put(sensors_kobj_ref);
  kobject_put(boat_kobj_ref);
  return 1;
}

static void exit_sysfs(void) {
//  sysfs_remove_file(autopilot_status_kobj_ref, &autopilot_auto_rel_attribute.attr);
//  sysfs_remove_file(autopilot_status_kobj_ref, &autopilot_large_xte_attribute.attr);
//  sysfs_remove_file(autopilot_status_kobj_ref, &autopilot_no_data_attribute.attr);
//  sysfs_remove_file(autopilot_status_kobj_ref, &autopilot_heading_display_off_attribute.attr);
//  sysfs_remove_file(autopilot_status_kobj_ref, &autopilot_rudder_position_attribute.attr);
//  sysfs_remove_file(alarms_kobj_ref, &alarms_autopilot_wind_shift_attribute.attr);
//  sysfs_remove_file(alarms_kobj_ref, &alarms_autopilot_off_course_attribute.attr);
//  sysfs_remove_file(autopilot_status_kobj_ref, &autopilot_mode_attribute.attr);
//  sysfs_remove_file(autopilot_status_kobj_ref, &autopilot_target_heading_attribute.attr);
//  sysfs_remove_file(autopilot_status_kobj_ref, &autopilot_turning_direction_attribute.attr);
//  sysfs_remove_file(autopilot_status_kobj_ref, &autopilot_compass_heading_attribute.attr);
//  sysfs_remove_file(sensors_kobj_ref, &compass_sensor_attribute.attr);
//  kobject_put(alarms_kobj_ref);
//  kobject_put(autopilot_status_kobj_ref);
  kobject_put(status_kobj_ref);
  kobject_put(sensors_kobj_ref);
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
