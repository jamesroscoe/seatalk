#ifdef TESTING
  #include <stdio.h>
  #include <string.h>
  #include <stdlib.h>
  #include <time.h>
#else
  #include <linux/module.h>
  #include <linux/time.h>
  #include "linux/seatalk_module.h"
#endif
#include "seatalk_state.h"
#include "seatalk_transport_layer.h"
#include "seatalk_datagram.h"

#define SENSOR_VARIABLE(NAME) sensor_ ## NAME
#define STATUS_VARIABLE(NAME) status_ ## NAME
#define SENSOR_EXPIRY_TIME(NAME) sensor_##NAME##_expiry
#define STATUS_EXPIRY_TIME(NAME) status_##NAME##_expiry
#define VALID_SENSOR(NAME) (get_time() <= SENSOR_EXPIRY_TIME(NAME))
#define VALID_STATUS(NAME) (get_time() <= STATUS_EXPIRY_TIME(NAME))
#define SET_SENSOR_EXPIRY(NAME, VALUE) SENSOR_EXPIRY_TIME(NAME) = VALUE
#define SET_STATUS_EXPIRY(NAME, VALUE) STATUS_EXPIRY_TIME(NAME) = VALUE
#define VALIDATE_SENSOR(NAME) SET_SENSOR_EXPIRY(NAME, get_time() + TIME_TO_LIVE)
#define VALIDATE_STATUS(NAME) SET_STATUS_EXPIRY(NAME, get_time() + TIME_TO_LIVE)
#define SET_SENSOR(NAME, VALUE) VALIDATE_SENSOR(NAME);\
  SENSOR_VARIABLE(NAME) = VALUE;\
  initiate_seatalk_transmitter()
#define INVALIDATE_SENSOR(NAME) SET_SENSOR_EXPIRY(NAME, 0)
#define INVALIDATE_STATUS(NAME) SET_STATUS_EXPIRY(NAME, 0)
#define DEFINE_SENSOR_EXPIRY(NAME) time_t SENSOR_EXPIRY_TIME(NAME) = 0
#define DEFINE_STATUS_EXPIRY(NAME) time_t STATUS_EXPIRY_TIME(NAME) = 0
#define DEFINE_SENSOR(NAME, TYPE) TYPE SENSOR_VARIABLE(NAME);\
  DEFINE_SENSOR_EXPIRY(NAME)
#define DEFINE_STATUS(NAME, TYPE) TYPE STATUS_VARIABLE(NAME);\
  DEFINE_STATUS_EXPIRY(NAME)

time_t get_time(void) {
  #ifdef TESTING
    time_t now;
    time(&now);
    return now;
  #else
    struct timespec now = current_kernel_time();
    return now.tv_sec;
  #endif
}

DEFINE_SENSOR(compass, int);
DEFINE_STATUS(autopilot, struct AUTOPILOT_STATUS);

//typedef enum COMMAND {
//};

//enum COMMAND pending_command;

int seatalk_sensor_pending(void) {
  return VALID_SENSOR(compass);
}

int build_command_datagram(char *datagram) {
//  switch (pending_command) {
//  }
  return 0;
}

int build_sensor_datagram(char *datagram) {
  if (VALID_SENSOR(compass)) {
    return build_compass_datagram(datagram, SENSOR_VARIABLE(compass));
  } else {
    return 0;
  }
}

int get_pending_datagram(char *datagram) {
  int length;
  if (length = build_command_datagram(datagram)) {
    return length;
  } else if (length = build_sensor_datagram(datagram)) {
    return length;
  } else {
    return 0;
  }
}

void update_seatalk_state(char *datagram) {
//  printf("update_seatalk_state\n");
//  int i;
//  pr_info("update_seatalk_state: ");
//  for (i = 0; i < (datagram[1] & 0x0f) + 3; i++) {
//    pr_info("%x ", datagram[i]);
//  }
//  pr_info("\n");
  handle_seatalk_datagram(datagram);
//  state_updated();
}

int copy_string(char *dest, char *source, int max_length) {
  #ifdef TESTING
    strcpy(dest, source);
    return strlen(dest);
  #else
    return strscpy(dest, source, max_length);
  #endif
}

void clear_string(char *s) {
  s[0] = 0;
}

int flag_value(int flag) {
  return flag ? 1 : 0;
}

int itoa(char* buf, int val)
{
    const unsigned int radix = 10;

    char* p;
    unsigned int a;        //every digit
    int len;
    char* b;            //start of the digit char
    char temp;
    unsigned int u;

    p = buf;

    if (val < 0)
    {
        *p++ = '-';
        val = 0 - val;
    }
    u = (unsigned int)val;

    b = p;

    do
    {
        a = u % radix;
        u /= radix;

        *p++ = a + '0';

    } while (u > 0);

    len = (int)(p - buf);

    *p-- = 0;

    //swap
    do
    {
        temp = *p;
        *p = *b;
        *b = temp;
        --p;
        ++b;

    } while (b < p);

    return len;
}

void set_compass_sensor(int value) {
//  printf("set_ap_heading(%d) at %d\n", value, get_time());
  if ((value >= 0) && (value <= 359)) {
    SET_SENSOR(compass, value);
//    printf("compass: %d, expires: %d\n", compass, EXPIRY_TIME(compass));
//    printf("VALID(compass): %d\n", VALID(compass));
  } else {
    INVALIDATE_SENSOR(compass);
  }
}

int get_autopilot_status(struct AUTOPILOT_STATUS *new_ap_status) {
  if (!VALID_STATUS(autopilot)) {
    return -1;
  }
  *new_ap_status = STATUS_VARIABLE(autopilot);
  // printf("(ap_status) compass: %s, turn: %s, target: %s, mode: %s\n", ap_status.compass_heading, ap_status.turning_direction, ap_status.target_heading, ap_status.mode);
//  new_ap_status->compass_heading = ap_status.compass_heading;
//  copy_string(new_ap_status->turning_direction, ap_status.turning_direction, MODE_SIZE);
//  new_ap_status->target_heading = ap_status.target_heading;
//  copy_string(new_ap_status->mode, ap_status.mode, MODE_SIZE);
//  new_ap_status->off_course_alarm = ap_status.off_course_alarm;
//  new_ap_status->wind_shift_alarm = ap_status.wind_shift_alarm
//  new_ap_status->rudder_position = ap_status.rudder_position
//  new_ap_status->heading_display_off = ap_status.heading_display_off;
//  new_ap_status->no_data = ap_status.no_data;
//  new_ap_status->large_xte = ap_status.large_xte;
//  new_ap_status->auto_rel = ap_status.auto_rel;
  return 0;
}

void set_autopilot_status(int compass_heading, int turning_direction, int target_heading, int mode, int off_course_alarm, int wind_shift_alarm, int rudder_position, int heading_display_off, int no_data, int large_xte, int auto_rel) {
  struct AUTOPILOT_STATUS ap_status;
//  printf("set_ap_status(%d, %d, %d, %d)\n", compass_heading, turning_direction, target_heading, mode);
  ap_status.compass_heading = compass_heading;
  copy_string(ap_status.turning_direction, (turning_direction > 0) ? "right" : (turning_direction < 0) ? "left" : "none", MODE_SIZE);
  ap_status.target_heading = target_heading;
  switch (mode) {
    case 0:
      copy_string(ap_status.mode, "standby", MODE_SIZE);
      break;
    case 1:
      copy_string(ap_status.mode, "auto", MODE_SIZE);
      break;
    case 2:
      copy_string(STATUS_VARIABLE(autopilot).mode, "vane", MODE_SIZE);
      break;
    case 3:
      copy_string(STATUS_VARIABLE(autopilot).mode, "track", MODE_SIZE);
      break;
  }
  ap_status.off_course_alarm = off_course_alarm;
  ap_status.wind_shift_alarm = wind_shift_alarm;
  ap_status.rudder_position, rudder_position;
  ap_status.heading_display_off = heading_display_off;
  ap_status.no_data = no_data;
  ap_status.large_xte = large_xte;
  ap_status.auto_rel = auto_rel;
  STATUS_VARIABLE(autopilot) = ap_status;
  VALIDATE_STATUS(autopilot);
}

/*
void clear_ap_status(void) {
  clear_string(ap_status.compass_heading);
  clear_string(ap_status.turning_direction);
  clear_string(ap_status.target_heading);
  clear_string(ap_status.mode);
  clear_string(ap_status.off_course_alarm);
  clear_string(ap_status.wind_shift_alarm);
  clear_string(ap_status.rudder_position);
  clear_string(ap_status.heading_display_off);
  clear_string(ap_status.no_data);
  clear_string(ap_status.large_xte);
  clear_string(ap_status.auto_rel);
}
*/

