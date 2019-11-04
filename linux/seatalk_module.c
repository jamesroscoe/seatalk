#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sysfs.h>
#include "../seatalk_hardware_layer.h"
#include "../boat_status.h"

// sysfs
struct kobject *seatalk_kobj_ref;
struct kobject *sensors_kobj_ref;
struct kobject *status_kobj_ref;
struct kobject *autopilot_status_kobj_ref;
struct kobject *alarms_kobj_ref;

// sensors
#define SENSOR(NAME) static ssize_t sysfs_set_ ## NAME ## _sensor(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count);\
struct kobj_attribute NAME ## _sensor_attribute = __ATTR(NAME, 0220, NULL, sysfs_set_ ## NAME ## _sensor)
#define IMPLEMENT_SENSOR(NAME) static ssize_t sysfs_set_ ## NAME ## _sensor(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)

//SENSOR(compass);

// status
#define STATUS(PARENT, NAME) static ssize_t sysfs_get_ ## PARENT ## _ ## NAME(struct kobject *kobj, struct kobj_attribute *attr, char *buf);\
struct kobj_attribute PARENT ## _ ## NAME ## _attribute = __ATTR(NAME, 0440, sysfs_get_ ## PARENT ## _ ## NAME, NULL)
#define IMPLEMENT_STATUS(PARENT, NAME) static ssize_t sysfs_get_ ## PARENT ## _ ## NAME(struct kobject *kobj, struct kobj_attribute *attr, char *buf)

// autopilot status
#define IMPLEMENT_AUTOPILOT_STATUS(PARENT, NAME) IMPLEMENT_STATUS(PARENT, NAME) {\
  struct AUTOPILOT_STATUS ap_status;\
  if (get_autopilot_status(&ap_status)) {\
    return 0;\
  }

//STATUS(autopilot, compass_heading);
//STATUS(autopilot, turning_direction);
//STATUS(autopilot, target_heading);
//STATUS(autopilot, mode);
//STATUS(alarms, autopilot_off_course);
//STATUS(alarms, autopilot_wind_shift);
//STATUS(autopilot, rudder_position);
//STATUS(autopilot, heading_display_off);
//STATUS(autopilot, no_data);
//STATUS(autopilot, large_xte);
//STATUS(autopilot, auto_rel);

// utility
int flag_display(char *buf, int value) {
  return sprintf(buf, value ? "on" : "off");
}

// implement sysfs get/set functions
//IMPLEMENT_SENSOR(compass) {
//  int heading;
//  char *end_pointer;
//  pr_info("set_compass_sensor: %s", buf);
//  heading = simple_strtol(buf, &end_pointer, 10);
//  set_compass_sensor(heading);
//  return count;
//}

//IMPLEMENT_AUTOPILOT_STATUS(autopilot, compass_heading)
//  return sprintf(buf, "%d", ap_status.compass_heading);
//}

//IMPLEMENT_AUTOPILOT_STATUS(autopilot, turning_direction)
//  return sprintf(buf, "%s", ap_status.turning_direction);
//}

//IMPLEMENT_AUTOPILOT_STATUS(autopilot, target_heading)
//  return sprintf(buf, "%d", ap_status.target_heading);
//}

//IMPLEMENT_AUTOPILOT_STATUS(autopilot, mode)
//  return sprintf(buf, "%s", ap_status.mode);
//}

//IMPLEMENT_AUTOPILOT_STATUS(alarms, autopilot_off_course)
//  return flag_display(buf, ap_status.off_course_alarm);
//}

//IMPLEMENT_AUTOPILOT_STATUS(alarms, autopilot_wind_shift)
//  return flag_display(buf, ap_status.wind_shift_alarm);
//}

//IMPLEMENT_AUTOPILOT_STATUS(autopilot, rudder_position)
//  return sprintf(buf, "%d", ap_status.rudder_position);
//}

//IMPLEMENT_AUTOPILOT_STATUS(autopilot, heading_display_off)
//  return flag_display(buf, ap_status.heading_display_off);
//}

//IMPLEMENT_AUTOPILOT_STATUS(autopilot, no_data)
//  return flag_display(buf, ap_status.no_data);
//}

//IMPLEMENT_AUTOPILOT_STATUS(autopilot, large_xte)
//  return flag_display(buf, ap_status.large_xte);
//}

//IMPLEMENT_AUTOPILOT_STATUS(autopilot, auto_rel)
//  return flag_display(buf, ap_status.auto_rel);
//}

void state_updated(void) {
  pr_info("New data on SeaTalk bus");
}

static int init_sysfs(void) {
  seatalk_kobj_ref = kobject_create_and_add("seatalk", NULL);
  sensors_kobj_ref = kobject_create_and_add("sensors", seatalk_kobj_ref);
  status_kobj_ref = kobject_create_and_add("status", seatalk_kobj_ref);
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
  kobject_put(seatalk_kobj_ref);
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
  kobject_put(seatalk_kobj_ref);
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
