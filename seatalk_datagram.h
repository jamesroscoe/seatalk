#define MAX_DATAGRAM_LENGTH 18

#define ST_DATAGRAM_DEPTH_BELOW_TRANSDUCER 0x00
#define ST_DATAGRAM_EQUIPMENT_ID 0x01
#define ST_DATAGRAM_ENGINE_STATUS 0x05
#define ST_DATAGRAM_APPARENT_WIND_ANGLE 0x10
#define ST_DATAGRAM_APPARENT_WIND_SPEED 0x11
#define ST_DATAGRAM_WATER_SPEED 0x20
#define ST_DATAGRAM_TRIP_MILEAGE 0x21
#define ST_DATAGRAM_TOTAL_MILEAGE 0x22
#define ST_DATAGRAM_WATER_TEMPERATURE 0x23
#define ST_DATAGRAM_SPEED_DISTANCE_UNITS 0x24
#define ST_DATAGRAM_LOG 0x25
#define ST_DATAGRAM_WATER_SPEED_WITH_AVERAGE 0x26
#define ST_DATAGRAM_PRECISE_WATER_TEMPERATURE 0x27
#define ST_DATAGRAM_SET_LAMP_INTENSITY 0x30
#define ST_DATAGRAM_CANCEL_MOB 0x36
#define ST_DATAGRAM_CODELOCK_DATA 0x38
#define ST_DATAGRAM_LAT_POSITION 0x50
#define ST_DATAGRAM_LON_POSITION 0x51
#define ST_DATAGRAM_SPEED_OVER_GROUND 0x52
#define ST_DATAGRAM_COURSE_OVER_GROUND 0x53
#define ST_DATAGRAM_GMT_TIME 0x54
#define ST_DATAGRAM_TRACK_KEYSTROKE 0x55
#define ST_DATAGRAM_DATE 0x56
#define ST_DATAGRAM_SATELLITE_STATUS 0x57
#define ST_DATAGRAM_POSITION_FIX 0x58
#define ST_DATAGRAM_SET_COUNTDOWN_TIMER 0x59
#define ST_DATAGRAM_INITIALIZATION_BROADCAST 0x61
#define ST_DATAGRAM_DEPTH_UNITS 0x65
#define ST_DATAGRAM_WIND_ALARM 0x66
#define ST_DATAGRAM_ALARM_ACKNOWLEDGEMENT 0x68
#define ST_DATAGRAM_SECOND_EQUIPMENT_ID 0x6c
#define ST_DATAGRAM_MOB_KEYSTROKE 0x6e
#define ST_DATAGRAM_ST60_REMOTE_CONTROL_KEYSTROKE 0x70
#define ST_DATAGRAM_SET_LAMP_INTENSITY_2 0x80
#define ST_DATAGRAM_COURSE_COMPUTER_SETUP 0x81
#define ST_DATAGRAM_TARGET_WAYPOINT_NAME 0x82
#define ST_DATAGRAM_COURSE_COMPUTER_STARTUP_BROADCAST 0x83
#define ST_DATAGRAM_AUTOPILOT_STATUS 0x84
#define ST_DATAGRAM_WAYPOINT_NAVIGATION 0x85
#define ST_DATAGRAM_AUTOPILOT_COMMAND 0x86
#define ST_DATAGRAM_SET_AUTOPILOT_RESPONSE_LEVEL 0x87
#define ST_DATAGRAM_AUTOPILOT_PARAMETER 0x88
#define ST_DATAGRAM_HEADING 0x89
#define ST_DATAGRAM_DEVICE_INFORMATION 0x90
#define ST_DATAGRAM_SET_RUDDER_GAIN 0x91
#define ST_DATAGRAM_SET_AUTOPILOT_PARAMETER 0x92
#define ST_DATAGRAM_ENTER_AUTOPILOT_SETUP 0x93
#define ST_DATAGRAM_AUTOPILOT_STATUS_IN_SETUP_MODE 0x95
#define ST_DATAGRAM_COMPASS_VARIATION 0x99
#define ST_DATAGRAM_VERSION 0x9a
#define ST_DATAGRAM_HEADING_AND_RUDDER_POSITION 0x9c
#define ST_DATAGRAM_WAYPOINT 0x9e
#define ST_DATAGRAM_DESTINATION_WAYPOINT 0xa1
#define ST_DATAGRAM_WAYPOINT_ARRIVAL_NOTICE 0xa2
#define ST_DATAGRAM_REQUEST_DEVICE_IDENTIFICATION 0xa4
#define ST_DATAGRAM_GPS_AND_DGPS_FIX_INFO 0xa5
#define ST_DATAGRAM_PURPOSE_UNKNOWN 0xa7
#define ST_DATAGRAM_ALARM_NOTICE_1_FOR_GUARD_1_OR_GUARD_2 0xa8
#define ST_DATAGRAM_ALARM_NOTICE_2_FOR_GUARD_1_OR_GUARD_2 0xab

#define DATAGRAM_ID(DATAGRAM) DATAGRAM[0]
#define FIRST_NIBBLE(BYTE) (BYTE & 0xf0) >> 4
#define LAST_NIBBLE(BYTE) (BYTE & 0x0f)
#define GET_DATAGRAM_LENGTH(SECOND_BYTE) LAST_NIBBLE(SECOND_BYTE)

int st_build_depth_below_transducer(char *datagram, int depth_in_feet_times_10, int display_in_metres, int active_alarms, int transducer_defective);
void st_parse_depth_below_transducer(char *datagram, int *depth_in_feet_times_10, int *display_units, int *active_alarms, int *transducer_defective);
int st_build_engine_rpm_and_pitch(char *datagram, ENGINE_ID engine_id, int rpm, int pitch_percent);
void st_parse_engine_rpm_and_pitch(char *datagram, ENGINE_ID *engine_id, int *rpm, int *pitch_percent);
int st_build_apparent_wind_angle(char *datagram, int degrees_right_times_2);
void st_parse_apparent_wind_angle(char *datagram, int *degrees_right_times_2);
int st_build_apparent_wind_speed(char *datagram, int knots_times_10, int display_in_metric);
void st_parse_apparent_wind_speed(char *datagram, int *knots_times_10, int *display_in_metric);
int st_build_water_speed(char *datagram, int knots_times_10);
void st_parse_water_speed(char *datagram, int *knots_times_10);
int st_build_trip_mileage(char *datagram, int nmiles_times_100);
void st_parse_trip_mileage(char *datagram, int *miles_times_100);
int st_build_total_mileage(char *datagram, int nmiles_times_10);
void st_parse_total_mileage(char *datagram, int *miles_times_10);
int st_build_water_temperature(char *datagram, int degrees_fahrenheit, int transducer_defective);
void st_parse_water_temperature(char *datagram, int *degrees_fahrenheit, int *transducer_defective);
int st_build_speed_distance_units(char *datagram, DISTANCE_UNITS distance_units);
void st_parse_speed_distance_units(char *datagram, DISTANCE_UNITS *distance_units);
int st_build_total_and_trip_mileage(char *datagram, int total_nm_times_10, int trip_nm_times_100);
void st_parse_total_and_trip_mileage(char *datagram, int *total_mileage_times_10, int *trip_mileage_times_100);
int st_build_average_water_speed(char *datagram, int knots_1_times_100, int knots_2_times_100, int speed_1_from_sensor, int speed_2_is_average, int average_is_stopped, int display_in_statute_miles);
void st_parse_average_water_speed(char *datagram, int *knots_1_times_100, int *knots_2_times_100, int *speed_1_from_sensor, int *speed_2_is_average, int *average_is_stopped, int *display_in_statute_miles);
int st_build_precise_water_temperature(char *datagram, int degrees_celsius_times_10);
void st_parse_precise_water_temperature(char *datagram, int *degrees_celsius_times_10);
int st_build_set_lamp_intensity(char *datagram, int intensity);
void st_parse_set_lamp_intensity(char *datagram, int *level);
int st_build_cancel_mob(char *datagram);
void st_parse_cancel_mob(char *datagram);
int st_build_lat_position(char *datagram, LATITUDE_HEMISPHERE hemisphere, int degrees, int minutes_times_100);
void st_parse_lat_position(char *datagram, LATITUDE_HEMISPHERE *hemisphere, int *degrees, int *minutes_times_100);
int st_build_lon_position(char *datagram, LONGITUDE_HEMISPHERE hemisphere, int degrees, int minutes_times_100);
void st_parse_lon_position(char *datagram, LONGITUDE_HEMISPHERE *hemisphere, int *degrees, int *minutes_times_100);
int st_build_speed_over_ground(char *datagram, int knots_times_10);
void st_parse_speed_over_ground(char *datagram, int *knots_times_10);
int st_build_course_over_ground(char *datagram, int true_degrees);
void st_parse_course_over_ground(char *datagram, int *true_degrees);
int st_build_gmt_time(char *datagram, int hours, int minutes, int seconds);
void st_parse_gmt_time(char *datagram, int *hours, int *minutes, int *seconds);
int st_build_date(char *datagram, int year, int month, int day);
void st_parse_date(char *datagram, int *year, int *month, int *day);
int st_build_satellite_info(char *datagram, int satellite_count, int horizontal_dilution_of_position);
void st_parse_satellite_info(char *datagram, int *satellite_count, int *horizontal_dilution_of_position);
int st_build_lat_lon_position(char *datagram, LATITUDE_HEMISPHERE hemisphere_latitude, int degrees_lat, int minutes_lat_times_1000, LONGITUDE_HEMISPHERE hemisphere_longitude, int degrees_lon, int minutes_lon_times_1000);
void st_parse_lat_lon_position(char *datagram, LATITUDE_HEMISPHERE *hemisphere_latitude, int *degrees_lat, int *minutes_lat_times_1000, LONGITUDE_HEMISPHERE *hemisphere_longitude, int *degrees_lon, int *minutes_lon_times_1000);
int st_build_set_countdown_timer(char *datagram, int hours, int minutes, int seconds, TIMER_MODE mode);
void st_parse_set_countdown_timer(char *datagram, int *hours, int *minutes, int *seconds, TIMER_MODE *mode);
int st_build_wind_alarm(char *datagram, int active_alarms);
void st_parse_wind_alarm(char *datagram, int *active_alarms);
int st_build_alarm_acknowledgement(char *datagram, int acknowledged_alarm);
void st_parse_alarm_acknowledgement(char *datagram, int *acknowledged_alarm);
void st_parse_maxview_keystroke(char *datagram, int *key_1, int *key_2, int *held_longer);
int st_build_target_waypoint_name(char *datagram, int char1, int char2, int char3, int char4);
int st_parse_target_waypoint_name(char *datagram, int *char_1, int *char_2, int *char_3, int *char_4);
int st_build_course_computer_failure(char *datagram, COURSE_COMPUTER_FAILURE_TYPE failure_type);
void st_parse_course_computer_failure(char *datagram, COURSE_COMPUTER_FAILURE_TYPE *failure_type);
int st_build_autopilot_status(char *datagram, int compass_heading, TURN_DIRECTION turning_direction, int target_heading, AUTOPILOT_MODE mode, int rudder_position, int alarms, int display_flags);
void st_parse_autopilot_status(char *datagram, int *compass_heading, TURN_DIRECTION *turning_direction, int *target_heading, AUTOPILOT_MODE *mode, int *rudder_position, int *alarms, int *display_flags);
int st_build_waypoint_navigation(char *datagram, int cross_track_error_present, int cross_track_error_times_100, int waypoint_bearing_present, int waypoint_bearing, int bearing_is_magnetic, int waypoint_distance_present, int waypoint_distance_times_100, int direction_to_steer);
int st_parse_waypoint_navigation(char *datagram, int *cross_track_error_present, int *cross_track_error_times_100, int *waypoint_bearing_present, int *waypoint_bearing, int *bearing_is_magnetic, int *waypoint_distance_present, int *waypoint_distance_times_100, int *direction_to_steer);
int st_build_autopilot_command(char *datagram, ST_AUTOPILOT_COMMAND command);
int st_parse_autopilot_command(char *datagram, ST_AUTOPILOT_COMMAND *command);
int st_build_set_autopilot_response_level(char *datagram, AUTOPILOT_RESPONSE_LEVEL response_level);
void st_parse_set_autopilot_response_level(char *datagram, AUTOPILOT_RESPONSE_LEVEL *response_level);
int st_build_autopilot_parameter(char *datagram, int parameter, int min_value, int max_value, int value);
void st_parse_autopilot_parameter(char *datagram, int *parameter, int *min_value, int *max_value, int *value);
int st_build_heading(char *datagram, int heading, int locked_heading_active, int locked_heading);
void st_parse_heading(char *datagram, int *heading, int *locked_heading_active, int *locked_heading);
int st_build_set_rudder_gain(char *datagram, int rudder_gain);
void st_parse_set_rudder_gain(char *datagram, int *rudder_gain);
int st_build_set_autopilot_parameter(char *datagram, int parameter, int value);
void st_parse_set_autopilot_parameter(char *datagram, int *parameter, int *value);
void st_parse_enter_autopilot_setup(char *datagram);
int st_build_compass_variation(char *datagram, int degrees);
void st_parse_compass_variation(char *datagram, int *degrees);
int st_build_heading_and_rudder_position(char *datagram, int heading, TURN_DIRECTION turning_direction, int rudder_position);
void st_parse_heading_and_rudder_position(char *datagram, int *heading, TURN_DIRECTION *turning_direction, int *rudder_position);
int st_build_destination_waypoint_info(char *datagram, char last_4, char first_8, int more_records, int last_record);
void st_parse_destination_waypoint_info(char *datagram, char *last_4, char *first_8, int *more_records, int *last_record);
int st_build_arrival_info(char *datagram, int perpendicular_passed, int circle_entered, int char_1, int char2, int char_3, int char_4);
void st_parse_arrival_info(char *datagram, int *perpendicular_passed, int *circle_entered, int *char_1, int *char_2, int *char_3, int *char_4);
int st_build_gps_and_dgps_fix_info(char *datagram, int signal_quality_available, int signal_quality, int hdop_available, int hdop, int antenna_height, int satellite_count_available, int satellite_count, int geoseparation, int dgps_age_available, int dgps_age, int dgps_status_id_available, int dgps_station_id);
void st_parse_gps_and_dgps_fix_info(char *datagram, int *signal_quality_available, int *signal_quality, int *hdop_available, int *hdop, int *antenna_height, int *satellite_count_available, int *satellite_count, int *geoseparation, int *dgps_age_available, int *dgps_age, int *dgps_station_id_available, int *dgps_station_id);
