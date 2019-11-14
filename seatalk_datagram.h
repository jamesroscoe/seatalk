#define AUTOPILOT_DISPLAY_OFF 0x01
#define AUTOPILOT_DISPLAY_400G 0x02
#define AUTOPILOT_DISPLAY_NO_DATA 0x8
#define AUTOPILOT_DISPLAY_LARGE_XTE 0x10
#define AUTOPILOT_DISPLAY_AUTO_REL 0x80

#define AUTOPILOT_TYPE_400G 0x08
#define AUTOPILOT_TYPE_150G 0x05

typedef enum { TIMER_MODE_COUNT_UP, TIMER_MODE_COUNT_DOWN, TIMER_MODE_COUNT_UP_AND_START, TIMER_MODE_COUNT_DOWN_AND_START } TIMER_MODE;
typedef enum { COURSE_COMPUTER_FAILURE_TYPE_NONE, COURSE_COMPUTER_FAILURE_TYPE_AUTO_RELEASE_ERROR, COURSE_COMPUTER_FAILURE_TYPE_DRIVE_STOPPED } COURSE_COMPUTER_FAILURE_TYPE;
typedef enum { ST_AUTOPILOT_MODE_STANDBY = 0x0, ST_AUTOPILOT_MODE_AUTO = 0x2, ST_AUTOPILOT_MODE_VANE = 0x4, ST_AUTOPILOT_MODE_TRACK = 0x8 } ST_AUTOPILOT_MODE;
typedef enum {
  ST_AUTOPILOT_COMMAND_AUTO = 0x01,
  ST_AUTOPILOT_COMMAND_STANDBY = 0x02,
  ST_AUTOPILOT_COMMAND_TRACK = 0x03,
  ST_AUTOPILOT_COMMAND_DISP = 0x04,
  ST_AUTOPILOT_COMMAND_TURN_LEFT_1 = 0x05,
  ST_AUTOPILOT_COMMAND_TURN_LEFT_10 = 0x06,
  ST_AUTOPILOT_COMMAND_TURN_RIGHT_1 = 0x07,
  ST_AUTOPILOT_COMMAND_TURN_RIGHT_10 = 0x08,
  ST_AUTOPILOT_COMMAND_DECREASE_GAIN = 0x09,
  ST_AUTOPILOT_COMMAND_INCREASE_GAIN = 0x0a,
  ST_AUTOPILOT_COMMAND_TACK_LEFT = 0x21,
  ST_AUTOPILOT_COMMAND_TACK_RIGHT = 0x22,
  ST_AUTOPILOT_COMMAND_WIND_MODE = 0x23,
  ST_AUTOPILOT_COMMAND_TRACK_MODE = 0x28,
  ST_AUTOPILOT_COMMAND_TOGGLE_RESPONSE_LEVEL = 0x2e,
  ST_AUTOPILOT_COMMAND_RETURN_TO_COURSE = 0x41,
  ST_AUTOPILOT_COMMAND_ENTER_COMPASS_CALIBRATION_MODE = 0x42,
  ST_AUTOPILOT_COMMAND_PRESS_TRACK_LONGER = 0x43,
  ST_AUTOPILOT_COMMAND_PRESS_DISP_LONGER = 0x44,
  ST_AUTOPILOT_COMMAND_PRESS_LEFT_1_LONGER = 0x45,
  ST_AUTOPILOT_COMMAND_PRESS_LEFT_10_LONGER = 0x46,
  ST_AUTOPILOT_COMMAND_PRESS_RIGHT_1_LONGER = 0x47,
  ST_AUTOPILOT_COMMAND_PRESS_RIGHT_10_LONGER = 0x48,
  ST_AUTOPILOT_COMMAND_RETURN_TO_WIND_ANGLE = 0x63,
  ST_AUTOPILOT_COMMAND_PRESS_LEFT_10_RIGHT_10_LONGER = 0x68,
  ST_AUTOPILOT_COMMAND_ENTER_RUDDER_GAIN_MODE = 0x6e,
  ST_AUTOPILOT_COMMAND_HOLD_LEFT_1 = 0x80,
  ST_AUTOPILOT_COMMAND_HOLD_LEFT_10 = 0x81,
  ST_AUTOPILOT_COMMAND_HOLD_RIGHT_1 = 0x82,
  ST_AUTOPILOT_COMMAND_HOLD_RIGHT_10 = 0x83,
  ST_AUTOPILOT_COMMAND_RELEASE_HELD_KEY = 0x84
} ST_AUTOPILOT_COMMAND;

int get_datagram_length(char second_byte);

int build_depth_below_transducer(char *datagram, int depth_in_feet_times_10, int display_in_metres, int active_alarms, int transducer_defective);
void parse_depth_below_transducer(char *datagram, int *depth_in_feet_times_10, int *display_units, int *active_alarms, int *transducer_defective);
int build_engine_rpm_and_pitch(char *datagram, ENGINE_ID engine_id, int rpm, int pitch_percent);
void parse_engine_rpm_and_pitch(char *datagram, ENGINE_ID *engine_id, int *rpm, int *pitch_percent);
int build_apparent_wind_angle(char *datagram, int degrees_right_times_2);
void parse_apparent_wind_angle(char *datagram, int *degrees_right_times_2);
int build_apparent_wind_speed(char *datagram, int knots_times_10, int display_in_metric);
void parse_apparent_wind_speed(char *datagram, int *knots_times_10, int *display_in_metric);
int build_water_speed(char *datagram, int knots_times_10);
void parse_water_speed(char *datagram, int *knots_times_10);
int build_trip_mileage(char *datagram, int nmiles_times_100);
void parse_trip_mileage(char *datagram, int *miles_times_100);
int build_total_mileage(char *datagram, int nmiles_times_10);
void parse_total_mileage(char *datagram, int *miles_times_10);
int build_water_temperature(char *datagram, int degrees_fahrenheit, int transducer_defective);
void parse_water_temperature(char *datagram, int *degrees_fahrenheit, int *transducer_defective);
int build_speed_distance_units(char *datagram, DISTANCE_UNITS distance_units);
void parse_speed_distance_units(char *datagram, DISTANCE_UNITS *distance_units);
int build_total_and_trip_mileage(char *datagram, int total_nm_times_10, int trip_nm_times_100);
void parse_total_and_trip_mileage(char *datagram, int *total_mileage_times_10, int *trip_mileage_times_100);
int build_average_water_speed(char *datagram, int knots_1_times_100, int knots_2_times_100, int speed_1_from_sensor, int speed_2_is_average, int average_is_stopped, int display_in_statute_miles);
void parse_average_water_speed(char *datagram, int *knots_1_times_100, int *knots_2_times_100, int *speed_1_from_sensor, int *speed_2_is_average, int *average_is_stopped, int *display_in_statute_miles);
int build_precise_water_temperature(char *datagram, int degrees_celsius_times_10);
void parse_precise_water_temperature(char *datagram, int *degrees_celsius_times_10);
int build_lamp_intensity(char *datagram, int intensity);
void parse_lamp_intensity(char *datagram, int *level);
int build_cancel_mob(char *datagram);
int build_lat_position(char *datagram, LATITUDE_HEMISPHERE hemisphere, int degrees, int minutes_times_100);
void parse_lat_position(char *datagram, LATITUDE_HEMISPHERE *hemisphere, int *degrees, int *minutes_times_100);
int build_lon_position(char *datagram, LONGITUDE_HEMISPHERE hemisphere, int degrees, int minutes_times_100);
void parse_lon_position(char *datagram, LONGITUDE_HEMISPHERE *hemisphere, int *degrees, int *minutes_times_100);
int build_speed_over_ground(char *datagram, int knots_times_10);
void parse_speed_over_ground(char *datagram, int *knots_times_10);
int build_course_over_ground(char *datagram, int true_degrees);
void parse_course_over_ground(char *datagram, int *true_degrees);
int build_gmt_time(char *datagram, int hours, int minutes, int seconds);
void parse_gmt_time(char *datagram, int *hours, int *minutes, int *seconds);
int build_date(char *datagram, int year, int month, int day);
void parse_date(char *datagram, int *year, int *month, int *day);
int build_satellite_info(char *datagram, int satellite_count, int horizontal_dilution_of_position);
void parse_satellite_info(char *datagram, int *satellite_count, int *horizontal_dilution_of_position);
int build_lat_lon_position(char *datagram, LATITUDE_HEMISPHERE hemisphere_latitude, int degrees_lat, int minutes_lat_times_1000, LONGITUDE_HEMISPHERE hemisphere_longitude, int degrees_lon, int minutes_lon_times_1000);
void parse_lat_lon_position(char *datagram, LATITUDE_HEMISPHERE *hemisphere_latitude, int *degrees_lat, int *minutes_lat_times_1000, LONGITUDE_HEMISPHERE *hemisphere_longitude, int *degrees_lon, int *minutes_lon_times_1000);
int build_countdown_timer(char *datagram, int hours, int minutes, int seconds, TIMER_MODE mode);
void parse_countdown_timer(char *datagram, int *hours, int *minutes, int *seconds, TIMER_MODE *mode);
int build_wind_alarm(char *datagram, int active_alarms);
void parse_wind_alarm(char *datagram, int *active_alarms);
int build_alarm_acknowledgement(char *datagram, int acknowledged_alarm);
void parse_alarm_acknowledgement(char *datagram, int *acknowledged_alarm);
void parse_maxview_keystroke(char *datagram, int *key_1, int *key_2, int *held_longer);
void accept_maxview_keystroke(char *datagram);
int build_target_waypoint_name(char *datagram, int char1, int char2, int char3, int char4);
int parse_target_waypoint_name(char *datagram, int *char_1, int *char_2, int *char_3, int *char_4);
int build_course_computer_failure(char *datagram, COURSE_COMPUTER_FAILURE_TYPE failure_type);
void parse_course_computer_failure(char *datagram, COURSE_COMPUTER_FAILURE_TYPE *failure_type);
int build_autopilot_status(char *datagram, int compass_heading, TURN_DIRECTION turning_direction, int target_heading, AUTOPILOT_MODE mode, int wrudder_position, int alarms, int display_flags);
void parse_autopilot_status(char *datagram, int *compass_heading, TURN_DIRECTION *turning_direction, int *target_heading, AUTOPILOT_MODE *mode, int *rudder_position, int *alarms, int *display_flags);
int build_waypoint_navigation(char *datagram, int cross_track_error_present, int cross_track_error_times_100, int waypoint_bearing_present, int waypoint_bearing, int bearing_is_magnetic, int waypoint_distance_present, int waypoint_distance_times_100, int direction_to_steer);
int parse_waypoint_navigation(char *datagram, int *cross_track_error_present, int *cross_track_error_times_100, int *waypoint_bearing_present, int *waypoint_bearing, int *bearing_is_magnetic, int *waypoint_distance_present, int *waypoint_distance_times_100, int *direction_to_steer);
int build_autopilot_command(char *datagram, ST_AUTOPILOT_COMMAND command);
int parse_autopilot_command(char *datagram, ST_AUTOPILOT_COMMAND *command);
int build_set_autopilot_response_level(char *datagram, AUTOPILOT_RESPONSE_LEVEL response_level);
void parse_set_autopilot_response_level(char *datagram, AUTOPILOT_RESPONSE_LEVEL *response_level);
void parse_autopilot_parameter(char *datagram, int *parameter, int *min_value, int *max_value, int *value);
int build_heading(char *datagram, int heading, int locked_heading_active, int locked_heading);
void parse_heading(char *datagram, int *heading, int *locked_heading_active, int *locked_heading);
int build_set_rudder_gain(char *datagram, int rudder_gain);
void parse_set_rudder_gain(char *datagram, int *rudder_gain);
void parse_set_autopilot_parameter(char *datagram, int *parameter, int *value);
void parse_enter_autopilot_setup(char *datagram);
int build_compass_variation(char *datagram, int degrees);
void parse_compass_variation(char *datagram, int *degrees);
int build_heading_and_rudder_position(char *datagram, int heading, TURN_DIRECTION turning_direction, int rudder_position);
void parse_heading_and_rudder_position(char *datagram, int *heading, TURN_DIRECTION *turning_direction, int *rudder_position);
void parse_destination_waypoint_info(char *datagram, char *last_4, char *first_8, int *more_records, int *last_record);
void parse_arrival_info(char *datagram, int *perpendicular_passed, int *circle_entered, int *char_1, int *char_2, int *char_3, int *char_4);
int build_gps_and_dgps_fix_info(char *datagram, int signal_quality_available, int signal_quality, int hdop_available, int hdop, int antenna_height, int satellite_count_available, int satellite_count, int geoseparation, int dgps_age_available, int dgps_age, int dgps_status_id_available, int dgps_station_id);
void parse_gps_and_dgps_fix_info(char *datagram, int *signal_quality_available, int *signal_quality, int *hdop_available, int *hdop, int *antenna_height, int *satellite_count_available, int *satellite_count, int *geoseparation, int *dgps_age_available, int *dgps_age, int *dgps_station_id_available, int *dgps_station_id);

