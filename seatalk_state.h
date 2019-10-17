#define TIME_TO_LIVE 5
#define HEADING_SIZE 4
#define FLAG_SIZE 4
#define MODE_SIZE 10
#define ANGLE_SIZE 5

typedef struct AUTOPILOT_STATUS {
  int compass_heading;
  char turning_direction[MODE_SIZE];
  int target_heading;
  char mode[MODE_SIZE];
  int off_course_alarm;
  int wind_shift_alarm;
  int rudder_position;
  int heading_display_off;
  int no_data;
  int large_xte;
  int auto_rel;
};

int seatalk_command_pending(void);
int seatalk_sensor_pending(void);
int get_pending_datagram(char *datagram);
void update_seatalk_state(char *datagram);
void set_compass_sensor(int value);
void clear_compass_sensor(void);

int get_autopilot_status(struct AUTOPILOT_STATUS *new_ap_status);
void set_autopilot_status(int compass_heading, int turning_direction, int target_heading, int mode, int off_course_alarm, int wind_shift_alarm, int rudder_position, int heading_display_off, int no_data, int large_xte, int auto_rel);
