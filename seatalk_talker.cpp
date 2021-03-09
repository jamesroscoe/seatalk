#include "seatalk_protocol.h"
#include "seatalk_datagram.h"
#include "seatalk_transport_layer.h"
#include "logger.h"

#ifdef USE_DEFAULT_READY_TO_SEND
void seatalk_ready_to_send(int seatalk_port) {
//  do nothing
}
#endif

int st_send_datagram(int seatalk_port, int length, char *datagram) {
  return seatalk_queue_datagram(seatalk_port, length, datagram);
}

int st_send_depth_below_transducer(int seatalk_port, int depth_in_feet_times_10, int display_in_metres, int active_alarms, int transducer_defective) {
  char datagram[MAX_DATAGRAM_LENGTH];
  return seatalk_queue_datagram(seatalk_port, st_build_depth_below_transducer(datagram, depth_in_feet_times_10, display_in_metres, active_alarms, transducer_defective), datagram);
}

int st_send_engine_rpm_and_pitch(int seatalk_port, ENGINE_ID engine_id, int rpm, int pitch_percent) {
  char datagram[MAX_DATAGRAM_LENGTH];
  return seatalk_queue_datagram(seatalk_port, st_build_engine_rpm_and_pitch(datagram, engine_id, rpm, pitch_percent), datagram);
}

int st_send_apparent_wind_angle(int seatalk_port, int degrees_right_times_2) {
  char datagram[MAX_DATAGRAM_LENGTH];
  return seatalk_queue_datagram(seatalk_port, st_build_apparent_wind_angle(datagram, degrees_right_times_2), datagram);
}

int st_send_apparent_wind_speed(int seatalk_port, int knots_times_10, int display_in_metric) {
  char datagram[MAX_DATAGRAM_LENGTH];
  return seatalk_queue_datagram(seatalk_port, st_build_apparent_wind_speed(datagram, knots_times_10, display_in_metric), datagram);
}

int st_send_water_speed(int seatalk_port, int knots_times_10) {
  char datagram[MAX_DATAGRAM_LENGTH];
  return seatalk_queue_datagram(seatalk_port, st_build_water_speed(datagram, knots_times_10), datagram);
}

int st_send_trip_mileage(int seatalk_port, int nmiles_times_100) {
  char datagram[MAX_DATAGRAM_LENGTH];
  return seatalk_queue_datagram(seatalk_port, st_build_trip_mileage(datagram, nmiles_times_100), datagram);
}

int st_send_total_mileage(int seatalk_port, int nmiles_times_10) {
  char datagram[MAX_DATAGRAM_LENGTH];
  return seatalk_queue_datagram(seatalk_port, st_build_total_mileage(datagram, nmiles_times_10), datagram);
}

int st_send_water_temperature(int seatalk_port, int degrees_fahrenheit, int transducer_defective) {
  char datagram[MAX_DATAGRAM_LENGTH];
  return seatalk_queue_datagram(seatalk_port, st_build_water_temperature(datagram, degrees_fahrenheit, transducer_defective), datagram);
}

int st_send_speed_distance_units(int seatalk_port, DISTANCE_UNITS distance_units) {
  char datagram[MAX_DATAGRAM_LENGTH];
  return seatalk_queue_datagram(seatalk_port, st_build_speed_distance_units(datagram, distance_units), datagram);
}

int st_send_total_and_trip_mileage(int seatalk_port, int total_nm_times_10, int trip_nm_times_100) {
  char datagram[MAX_DATAGRAM_LENGTH];
  return seatalk_queue_datagram(seatalk_port, st_build_total_and_trip_mileage(datagram, total_nm_times_10, trip_nm_times_100), datagram);
}

int st_send_average_water_speed(int seatalk_port, int knots_1_times_100, int knots_2_times_100, int speed_1_from_sensor, int speed_2_is_average, int average_is_stopped, int display_in_statute_miles) {
  char datagram[MAX_DATAGRAM_LENGTH];
  return seatalk_queue_datagram(seatalk_port, st_build_average_water_speed(datagram, knots_1_times_100, knots_2_times_100, speed_1_from_sensor, speed_2_is_average, average_is_stopped, display_in_statute_miles), datagram);
}

int st_send_precise_water_temperature(int seatalk_port, int degrees_celsius_times_10) {
  char datagram[MAX_DATAGRAM_LENGTH];
  return seatalk_queue_datagram(seatalk_port, st_build_precise_water_temperature(datagram, degrees_celsius_times_10), datagram);
}

int st_send_set_lamp_intensity(int seatalk_port, int intensity) {
  char datagram[MAX_DATAGRAM_LENGTH];
  return seatalk_queue_datagram(seatalk_port, st_build_set_lamp_intensity(datagram, intensity), datagram);
}

int st_send_cancel_mob(int seatalk_port) {
  char datagram[MAX_DATAGRAM_LENGTH];
  return seatalk_queue_datagram(seatalk_port, st_build_cancel_mob(datagram), datagram);
}

int st_send_lat_position(int seatalk_port, LATITUDE_HEMISPHERE hemisphere, int degrees, int minutes_times_100) {
  char datagram[MAX_DATAGRAM_LENGTH];
  return seatalk_queue_datagram(seatalk_port, st_build_lat_position(datagram, hemisphere, degrees, minutes_times_100), datagram);
}

int st_send_lon_position(int seatalk_port, LONGITUDE_HEMISPHERE hemisphere, int degrees, int minutes_times_100) {
  char datagram[MAX_DATAGRAM_LENGTH];
  return seatalk_queue_datagram(seatalk_port, st_build_lon_position(datagram, hemisphere, degrees, minutes_times_100), datagram);
}

int st_send_speed_over_ground(int seatalk_port, int knots_times_10) {
  char datagram[MAX_DATAGRAM_LENGTH];
  return seatalk_queue_datagram(seatalk_port, st_build_speed_over_ground(datagram, knots_times_10), datagram);
}

int st_send_course_over_ground(int seatalk_port, int true_degrees) {
  char datagram[MAX_DATAGRAM_LENGTH];
  return seatalk_queue_datagram(seatalk_port, st_build_course_over_ground(datagram, true_degrees), datagram);
}

int st_send_gmt_time(int seatalk_port, int hours, int minutes, int seconds) {
  char datagram[MAX_DATAGRAM_LENGTH];
  return seatalk_queue_datagram(seatalk_port, st_build_gmt_time(datagram, hours, minutes, seconds), datagram);
}

int st_send_date(int seatalk_port, int year, int month, int day) {
  char datagram[MAX_DATAGRAM_LENGTH];
  return seatalk_queue_datagram(seatalk_port, st_build_date(datagram, year, month, day), datagram);
}

int st_send_satellite_info(int seatalk_port, int satellite_count, int horizontal_dilution_of_position) {
  char datagram[MAX_DATAGRAM_LENGTH];
  return seatalk_queue_datagram(seatalk_port, st_build_satellite_info(datagram, satellite_count, horizontal_dilution_of_position), datagram);
}

int st_send_lat_lon_position(int seatalk_port, LATITUDE_HEMISPHERE hemisphere_latitude, int degrees_lat, int minutes_lat_times_1000, LONGITUDE_HEMISPHERE hemisphere_longitude, int degrees_lon, int minutes_lon_times_1000) {
  char datagram[MAX_DATAGRAM_LENGTH];
  return seatalk_queue_datagram(seatalk_port, st_build_lat_lon_position(datagram, hemisphere_latitude, degrees_lat, minutes_lat_times_1000, hemisphere_longitude, degrees_lon, minutes_lon_times_1000), datagram);
}

int st_send_set_countdown_timer(int seatalk_port, int hours, int minutes, int seconds, TIMER_MODE mode) {
  char datagram[MAX_DATAGRAM_LENGTH];
  return seatalk_queue_datagram(seatalk_port, st_build_set_countdown_timer(datagram, hours, minutes, seconds, mode), datagram);
}

int st_send_wind_alarm(int seatalk_port, int active_alarms) {
  char datagram[MAX_DATAGRAM_LENGTH];
  return seatalk_queue_datagram(seatalk_port, st_build_wind_alarm(datagram, active_alarms), datagram);
}

int st_send_alarm_acknowledgement(int seatalk_port, int acknowledged_alarm) {
  char datagram[MAX_DATAGRAM_LENGTH];
  return seatalk_queue_datagram(seatalk_port, st_build_alarm_acknowledgement(datagram, acknowledged_alarm), datagram);
}

int st_send_target_waypoint_name(int seatalk_port, int char1, int char2, int char3, int char4) {
  char datagram[MAX_DATAGRAM_LENGTH];
  return seatalk_queue_datagram(seatalk_port, st_build_target_waypoint_name(datagram, char1, char2, char3, char4), datagram);
}

int st_send_course_computer_failure(int seatalk_port, COURSE_COMPUTER_FAILURE_TYPE failure_type) {
  char datagram[MAX_DATAGRAM_LENGTH];
  return seatalk_queue_datagram(seatalk_port, st_build_course_computer_failure(datagram, failure_type), datagram);
}

int st_send_autopilot_status(int seatalk_port, int compass_heading, TURN_DIRECTION turning_direction, int target_heading, AUTOPILOT_MODE mode, int rudder_position, int alarms, int display_flags) {
  char datagram[MAX_DATAGRAM_LENGTH];
  return seatalk_queue_datagram(seatalk_port, st_build_autopilot_status(datagram, compass_heading, turning_direction, target_heading, mode, rudder_position, alarms, display_flags), datagram);
}

int st_send_waypoint_navigation(int seatalk_port, int cross_track_error_present, int cross_track_error_times_100, int waypoint_bearing_present, int waypoint_bearing, int bearing_is_magnetic, int waypoint_distance_present, int waypoint_distance_times_100, int direction_to_steer) {
  char datagram[MAX_DATAGRAM_LENGTH];
  return seatalk_queue_datagram(seatalk_port, st_build_waypoint_navigation(datagram, cross_track_error_present, cross_track_error_times_100, waypoint_bearing_present, waypoint_bearing, bearing_is_magnetic, waypoint_distance_present, waypoint_distance_times_100, direction_to_steer), datagram);
}

int st_send_autopilot_command(int seatalk_port, ST_AUTOPILOT_COMMAND command) {
  char datagram[MAX_DATAGRAM_LENGTH];
  return seatalk_queue_datagram(seatalk_port, st_build_autopilot_command(datagram, command), datagram);
}

int st_send_set_autopilot_response_level(int seatalk_port, AUTOPILOT_RESPONSE_LEVEL response_level) {
  char datagram[MAX_DATAGRAM_LENGTH];
  return seatalk_queue_datagram(seatalk_port, st_build_set_autopilot_response_level(datagram, response_level), datagram);
}

int st_send_heading(int seatalk_port, int heading, int locked_heading_active, int locked_heading) {
  char datagram[MAX_DATAGRAM_LENGTH];
  return seatalk_queue_datagram(seatalk_port, st_build_heading(datagram, heading, locked_heading_active, locked_heading), datagram);
}

int st_send_set_rudder_gain(int seatalk_port, int rudder_gain) {
  char datagram[MAX_DATAGRAM_LENGTH];
  return seatalk_queue_datagram(seatalk_port, st_build_set_rudder_gain(datagram, rudder_gain), datagram);
}

int st_send_compass_variation(int seatalk_port, int degrees) {
  char datagram[MAX_DATAGRAM_LENGTH];
  return seatalk_queue_datagram(seatalk_port, st_build_compass_variation(datagram, 0), datagram);
}

int st_send_heading_and_rudder_position(int seatalk_port, int heading, TURN_DIRECTION turning_direction, int rudder_position) {
  char datagram[MAX_DATAGRAM_LENGTH];
  return seatalk_queue_datagram(seatalk_port, st_build_heading_and_rudder_position(datagram, heading, turning_direction, rudder_position), datagram);
}

int st_send_gps_and_dgps_fix_info(int seatalk_port, int signal_quality_available, int signal_quality, int hdop_available, int hdop, int antenna_height, int satellite_count_available, int satellite_count, int geoseparation, int dgps_age_available, int dgps_age, int dgps_status_id_available, int dgps_station_id) {
  char datagram[MAX_DATAGRAM_LENGTH];
  return seatalk_queue_datagram(seatalk_port, st_build_gps_and_dgps_fix_info(datagram, signal_quality_available, signal_quality, hdop_available, hdop, antenna_height, satellite_count_available, satellite_count, geoseparation, dgps_age_available, dgps_age, dgps_status_id_available, dgps_station_id), datagram);
}

void st_wake_transmitter(int seatalk_port) {
  seatalk_wake_transmitter(seatalk_port);
}
