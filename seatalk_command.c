#ifdef TESTING
  void usleep_range(int min, int max) {
  }
#else
  #include <linux/delay.h>
#endif

#include "boat_status.h"
#include "seatalk_command.h"
#include "seatalk_datagram.h"

#define MAX_COMMAND_DELAY 3 // seconds
#define SLEEP_DURATION 100 // milliseconds
#define MAX_COMMAND_LOOPS MAX_COMMAND_DELAY / 100

char command_datagram[256];
int command_datagram_position = 0;
int command_datagram_bytes_remaining = 0;

int seatalk_command_pending(void) {
  return command_datagram_bytes_remaining > 0;
}

void send_command(int length) {
  int count = 0;
  command_datagram_position = 0;
  command_datagram_bytes_remaining = length;
  while (command_datagram_bytes_remaining || (count++ >= MAX_COMMAND_LOOPS)) {
    usleep_range(SLEEP_DURATION, SLEEP_DURATION * 2);
  }
}

void set_lamp_intensity(int level) {
//  send_command(build_set_lamp_intensity_datagram(datagram, level));
//  send_command(build_set_lamp_intensity2_datagram(datagram, level));
}

void cancel_mob(void) {
//  send_command(build_set_lamp_intensity_datagram(datagram));
}

void track_keystroke_from_gps(void) {
//  send_command(build_track_keystroke_from_gps_datagram(datagram));
}

void set_countdown_timer(int minutes, int seconds, int hours, int mode) {
//  send_command(build_set_countdown_timer_datagram(minutes, seconds, hours, mode));
}

void acknowledge_alarm(int alarm_type) {
//  send_command(build_acknowledge_alarm_datagram(alarm_type));
}

void mob(void) {
//  send_command(build_mob_waypoint_datagram());
//  send_command(build_mob_datagram());
}

void maxview_remote_keypress(int key) {
//  send_command(build_maxview_keypress_datagram(key));
}

void pass_user_cal() {
//  send_command(build_pass_user_cal_datagram());
//  send_command(build_pass_user_cal2_datagram());
}

void autopilot_remote_keystroke(int keystroke_type) {
//  send_command(build_autopilot_remote_keystroke_datagram(keystroke_type));
}

void set_autopilot_response_level(int level) {
//  send_command(build_set_autopilot_response_level_datagram(level));
}

void set_autopilot_rudder_gain(int level) {
//  send_command(build_set_autopilot_rudder_gain_datagram(level));
}

void set_autopilot_parameter(int parameter_number, int value) {
//  send_command(build_set_autopilot_parameter_datagram(parameter_number, value));
}

