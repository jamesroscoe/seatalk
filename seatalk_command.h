#include "seatalk_datagram.h"

int seatalk_command_pending(void);
void send_command(int length);
void set_lamp_intensity(int level);
void cancel_mob(void);
void track_keystroke_from_gps(void);
void set_countdown_timer(int minutes, int seconds, int hours, int mode);
void acknowledge_alarm(int alarm_type);
void mob(void);
void maxview_remote_keypress(int key);
void pass_user_cal(void);
void autopilot_remote_keystroke(int keystroke_type);
void set_autopilot_response_level(int level);
void set_autopilot_rudder_gain(int level);
void set_autopilot_parameter(int parameter_number, int value);
