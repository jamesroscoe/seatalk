#ifdef TESTING
  void usleep_range(int min, int max) {
  }
#else
  #include <linux/delay.h>
#endif

#include "boat_status.h"
#include "seatalk_datagram.h"
#include "seatalk_command.h"
#include "seatalk_protocol.h"

#define MAX_COMMAND_DELAY 3 // seconds
#define SLEEP_DURATION 100 // milliseconds
#define MAX_COMMAND_LOOPS MAX_COMMAND_DELAY * 1000 / SLEEP_DURATION

char command_datagram[256];
int command_datagram_position = 0;
int command_datagram_bytes_remaining = 0;

int seatalk_command_pending(void) {
  return command_datagram_bytes_remaining > 0;
}

int get_command_datagram(char *datagram) {
  int i;
  for (i = 0; i < command_datagram_bytes_remaining; i++) {
    datagram[i] = command_datagram[i];
  }
  return command_datagram_bytes_remaining;
}

int send_command(int length) {
  int count = 0;
  command_datagram_position = 0;
  command_datagram_bytes_remaining = length;
  wake_transmitter();
  while (command_datagram_bytes_remaining && (count++ <= MAX_COMMAND_LOOPS)) {
    usleep_range(SLEEP_DURATION, SLEEP_DURATION * 2);
  }
  return command_datagram_bytes_remaining == 0;
}

int set_lamp_intensity(int level) {
  return send_command(build_lamp_intensity(command_datagram, level));
}

//void cancel_mob(void) {
//  send_command(build_cancel_mob(command_datagram));
//}

//void track_keystroke_from_gps(void) {
//  send_command(build_track_keystroke_from_gps(command_datagram));
//}

//void set_countdown_timer(int hours, minutes, int seconds, int mode) {
//  send_command(build_countdown_timer(command_datagram, hours, minutes, seconds, mode));
//}

//void acknowledge_alarm(int alarm_type) {
//  send_command(build_alarm_acknowledgement(command_datagram, alarm_type));
//}

//void mob(void) {
//  send_command(build_mob_waypoint(command_datagram));
//  send_command(build_mob(command_datagram));
//}

//void maxview_remote_keypress(ST_AUTOPILOT_COMMAND command) {
//  send_command(build_autopilot_remote_keystroke(command_datagram, command));
//}

//void pass_user_cal() {
//  send_command(build_pass_user_cal());
//  send_command(build_pass_user_cal2());
//}

int autopilot_remote_keystroke(ST_AUTOPILOT_COMMAND command) {
  return send_command(build_autopilot_command(command_datagram, command));
}

int set_autopilot_response_level(AUTOPILOT_RESPONSE_LEVEL level) {
  return send_command(build_set_autopilot_response_level(command_datagram, level));
}

//void set_autopilot_rudder_gain(int level) {
//  send_command(build_set_rudder_gain(command_datagram, level));
//}

//void set_autopilot_parameter(int parameter_number, int value) {
//  send_command(build_set_autopilot_parameter(command_datagram, parameter_number, value));
//}

