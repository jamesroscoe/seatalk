// called by seatalk_transport_layer.cpp to notify talker that the bus is clear
// to send a datagram. Talker should respond by calling seatalk_queue_datagram()
void seatalk_ready_to_send(int seatalk_port);

// send_* methods are implemented by seatalk_protocol.cpp to initiate sending the
// requested information. Note that these do not queue up a datagram. There is no
// buffering. Call one of these in response to int seatalk_port_ready_to_send(int seatalk_port)

int st_send_datagram(int seatalk_port, int length, char *datagram);
int st_send_depth_below_transducer(int seatalk_port, int depth_in_feet_times_10, int display_in_metres, int active_alarms, int transducer_defective);
int st_send_engine_rpm_and_pitch(int seatalk_port, ENGINE_ID engine_id, int rpm, int pitch_percent);
int st_send_apparent_wind_angle(int seatalk_port, int degrees_right_times_2);
int st_send_apparent_wind_speed(int seatalk_port, int knots_times_10, int display_in_metric);
int st_send_water_speed(int seatalk_port, int knots_times_10);
int st_send_trip_mileage(int seatalk_port, int nmiles_times_100);
int st_send_total_mileage(int seatalk_port, int nmiles_times_10);
int st_send_water_temperature(int seatalk_port, int degrees_fahrenheit, int transducer_defective);
//int st_send_speed_distance_units(int seatalk_port, DISTANCE_UNITS distance_units);
int st_send_total_and_trip_mileage(int seatalk_port, int total_nm_times_10, int trip_nm_times_100);
int st_send_average_water_speed(int seatalk_port, int knots_1_times_100, int knots_2_times_100, int speed_1_from_sensor, int speed_2_is_average, int average_is_stopped, int display_in_statute_miles);
int st_send_precise_water_temperature(int seatalk_port, int degrees_celsius_times_10);
int st_send_lamp_intensity(int seatalk_port, int intensity);
int st_send_cancel_mob(int seatalk_port);
int st_send_lat_position(int seatalk_port, LATITUDE_HEMISPHERE hemisphere, int degrees, int minutes_times_100);
int st_send_lon_position(int seatalk_port, LONGITUDE_HEMISPHERE hemisphere, int degrees, int minutes_times_100);
int st_send_speed_over_ground(int seatalk_port, int knots_times_10);
int st_send_course_over_ground(int seatalk_port, int true_degrees);
int st_send_gmt_time(int seatalk_port, int hours, int minutes, int seconds);
int st_send_date(int seatalk_port, int year, int month, int day);
int st_send_satellite_info(int seatalk_port, int satellite_count, int horizontal_dilution_of_position);
int st_send_lat_lon_position(int seatalk_port, LATITUDE_HEMISPHERE hemisphere_latitude, int degrees_lat, int minutes_lat_times_1000, LONGITUDE_HEMISPHERE hemisphere_longitude, int degrees_lon, int minutes_lon_times_1000);
int st_send_countdown_timer(int seatalk_port, int hours, int minutes, int seconds, TIMER_MODE mode);
int st_send_wind_alarm(int seatalk_port, int active_alarms);
int st_send_alarm_acknowledgement(int seatalk_port, int acknowledged_alarm);
int st_send_target_waypoint_name(int seatalk_port, int char1, int char2, int char3, int char4);
int st_send_course_computer_failure(int seatalk_port, COURSE_COMPUTER_FAILURE_TYPE failure_type);
int st_send_autopilot_status(int seatalk_port, int compass_heading, TURN_DIRECTION turning_direction, int target_heading, AUTOPILOT_MODE mode, int rudder_position, int alarms, int display_flags);
int st_send_waypoint_navigation(int seatalk_port, int cross_track_error_present, int cross_track_error_times_100, int waypoint_bearing_present, int waypoint_bearing, int bearing_is_magnetic, int waypoint_distance_present, int waypoint_distance_times_100, int direction_to_steer);
int st_send_autopilot_command(int seatalk_port, ST_AUTOPILOT_COMMAND command);
int st_send_set_autopilot_response_level(int seatalk_port, AUTOPILOT_RESPONSE_LEVEL response_level);
int st_send_heading(int seatalk_port, int heading, int locked_heading_active, int locked_heading);
int st_send_set_rudder_gain(int seatalk_port, int rudder_gain);
int st_send_compass_variation(int seatalk_port, int degrees);
int st_send_heading_and_rudder_position(int seatalk_port, int heading, TURN_DIRECTION turning_direction, int rudder_position);
int st_send_gps_and_dgps_fix_info(int seatalk_port, int signal_quality_available, int signal_quality, int hdop_available, int hdop, int antenna_height, int satellite_count_available, int satellite_count, int geoseparation, int dgps_age_available, int dgps_age, int dgps_status_id_available, int dgps_station_id);

// called by seatalk_transport.cpp to request next datagram
void seatalk_clear_to_send(int seatalk_port);
// called from application to kick start the transmitter in case it has gone idle
void st_wake_transmitter(int seatalk_port);
