#include "seatalk_protocol.h"

#ifdef USE_DEFAULT_LISTENER
#include "seatalk_consumer.h"
#include "seatalk_datagram.h"

void st_handle_depth_below_transducer(int seatalk_port, char *datagram) {
  int depth_in_feet_times_10, display_in_metres, active_alarms, transducer_defective;
  int current_active_alarms;
  st_parse_depth_below_transducer(datagram, &depth_in_feet_times_10, &display_in_metres, &active_alarms, &transducer_defective);
  st_received_depth_below_transducer(seatalk_port, depth_in_feet_times_10, display_in_metres, active_alarms, transducer_defective);
}

void st_handle_engine_rpm_and_pitch(int seatalk_port, char *datagram) {
  ENGINE_ID engine_id;
  int rpm, pitch_percent;
  st_parse_engine_rpm_and_pitch(datagram, &engine_id, &rpm, &pitch_percent);
  st_received_engine_rpm_and_pitch(seatalk_port, engine_id, rpm, pitch_percent);
}

void st_handle_apparent_wind_angle(int seatalk_port, char *datagram) {
  int degrees_right_times_2;
  st_parse_apparent_wind_angle(datagram, &degrees_right_times_2);
  st_received_apparent_wind_angle(seatalk_port, degrees_right_times_2);
}

void st_handle_apparent_wind_speed(int seatalk_port, char *datagram) {
  int knots_times_10, display_in_metric;
  st_parse_apparent_wind_speed(datagram, &knots_times_10, &display_in_metric);
  st_received_apparent_wind_speed(seatalk_port, knots_times_10, display_in_metric);
}

void st_handle_water_speed(int seatalk_port, char *datagram) {
  int knots_times_10;
  st_parse_water_speed(datagram, &knots_times_10);
  st_received_water_speed(seatalk_port, knots_times_10);
}

void st_handle_trip_mileage(int seatalk_port, char *datagram) {
  int nmiles_times_100;
  st_parse_trip_mileage(datagram, &nmiles_times_100);
  st_received_trip_mileage(seatalk_port, nmiles_times_100);
}

void st_handle_total_mileage(int seatalk_port, char *datagram) {
  int miles_times_10;
  st_parse_total_mileage(datagram, &miles_times_10);
  st_received_total_mileage(seatalk_port, miles_times_10);
}

void st_handle_water_temperature(int seatalk_port, char *datagram) {
  int fahrenheit, transducer_defective;
  st_parse_water_temperature(datagram, &fahrenheit, &transducer_defective);
  st_received_water_temperature(seatalk_port, fahrenheit, transducer_defective);
}

void st_handle_speed_distance_units(int seatalk_port, char *datagram) {
  DISTANCE_UNITS distance_units;
  st_parse_speed_distance_units(datagram, &distance_units);
  st_received_speed_distance_units(seatalk_port, distance_units);
}

void st_handle_total_and_trip_mileage(int seatalk_port, char *datagram) {
  int total_mileage_times_10, trip_mileage_times_100;
  st_parse_total_and_trip_mileage(datagram, &total_mileage_times_10, &trip_mileage_times_100);
  st_received_total_and_trip_mileage(seatalk_port, total_mileage_times_10, trip_mileage_times_100);
}

void st_handle_average_water_speed(int seatalk_port, char *datagram) {
  int knots_1_times_100, knots_2_times_100, speed_1_from_sensor, speed_2_is_average, average_is_stopped, units;
  st_parse_average_water_speed(datagram, &knots_1_times_100, &knots_2_times_100, &speed_1_from_sensor, &speed_2_is_average, &average_is_stopped, &units);
  // this just assumes speed_2 is average
  st_received_average_water_speed(seatalk_port, knots_1_times_100, knots_2_times_100, speed_1_from_sensor, speed_2_is_average, average_is_stopped, units);
}

void st_handle_precise_water_temperature(int seatalk_port, char *datagram) {
  int celsius_times_10;
  st_parse_precise_water_temperature(datagram, &celsius_times_10);
  st_received_precise_water_temperature(seatalk_port, celsius_times_10);
}

void st_handle_set_lamp_intensity(int seatalk_port, char *datagram) {
  int level;
  st_parse_set_lamp_intensity(datagram, &level);
  st_received_set_lamp_intensity(seatalk_port, level);
}

void st_handle_cancel_mob(int seatalk_port, char *datagram) {
//  set_mob(seatalk_port, 0);
}

void st_handle_lat_position(int seatalk_port, char *datagram) {
  int degrees, minutes_times_100;
  LATITUDE_HEMISPHERE hemisphere;
  st_parse_lat_position(datagram, &hemisphere, &degrees, &minutes_times_100);
  st_received_lat_position(seatalk_port, hemisphere, degrees, minutes_times_100);
}

void st_handle_lon_position(int seatalk_port, char *datagram) {
  int degrees, minutes_times_100;
  LONGITUDE_HEMISPHERE hemisphere;
  st_parse_lon_position(datagram, &hemisphere, &degrees, &minutes_times_100);
  st_received_lon_position(seatalk_port, hemisphere, degrees, minutes_times_100);
}

void st_handle_speed_over_ground(int seatalk_port, char *datagram) {
  int knots_times_10;
  st_parse_speed_over_ground(datagram, &knots_times_10);
  st_received_speed_over_ground(seatalk_port, knots_times_10);
}

void st_handle_course_over_ground(int seatalk_port, char *datagram) {
  int true_degrees;
  st_parse_course_over_ground(datagram, &true_degrees);
  st_received_course_over_ground(seatalk_port, true_degrees);
}

void st_handle_gmt_time(int seatalk_port, char *datagram) {
  int hours, minutes, seconds;
  st_parse_gmt_time(datagram, &hours, &minutes, &seconds);
  st_received_gmt_time(seatalk_port, hours, minutes, seconds);
}

void st_handle_date(int seatalk_port, char *datagram) {
  int year, month, day;
  st_parse_date(datagram, &year, &month, &day);
  st_received_date(seatalk_port, year, month, day);
}

void st_handle_satellite_info(int seatalk_port, char *datagram) {
  int satellite_count, horizontal_dilution_of_position;
  st_parse_satellite_info(datagram, &satellite_count, &horizontal_dilution_of_position);
  st_received_satellite_info(seatalk_port, satellite_count, horizontal_dilution_of_position);
}

void st_handle_lat_lon_position(int seatalk_port, char *datagram) {
  int lat, minutes_lat_times_1000, lon, minutes_lon_times_1000;
  LATITUDE_HEMISPHERE lat_hemisphere;
  LONGITUDE_HEMISPHERE lon_hemisphere;
  st_parse_lat_lon_position(datagram, &lat_hemisphere, &lat, &minutes_lat_times_1000, &lon_hemisphere, &lon, &minutes_lon_times_1000);
  st_received_lat_lon_position(seatalk_port, lat_hemisphere, lat, minutes_lat_times_1000, lon_hemisphere, lon, minutes_lon_times_1000);
}

void st_handle_set_countdown_timer(int seatalk_port, char *datagram) {
  int hours, minutes, seconds;
  TIMER_MODE mode;
  st_parse_set_countdown_timer(datagram, &hours, &minutes, &seconds, &mode);
  st_received_set_countdown_timer(seatalk_port, hours, minutes, seconds, mode);
}

void st_handle_wind_alarm(int seatalk_port, char *datagram) {
  int active_alarms;
  st_parse_wind_alarm(datagram, &active_alarms);
  st_received_wind_alarm(seatalk_port, active_alarms);
}

void st_handle_alarm_acknowledgement(int seatalk_port, char *datagram) {
  int acknowledged_alarm;
  st_parse_alarm_acknowledgement(datagram, &acknowledged_alarm);
  st_received_alarm_acknowledgement(seatalk_port, acknowledged_alarm);
}

void st_handle_set_mob(int seatalk_port, char *datagram) {
//  set_mob(seatalk_port, 1);
}

void st_handle_maxview_keystroke(int seatalk_port, char *datagram) {
  int key_1, key_2, held_longer;
  st_parse_maxview_keystroke(datagram, &key_1, &key_2, &held_longer);
  st_received_maxview_keystroke(seatalk_port, key_1, key_2, held_longer);
}

void st_handle_target_waypoint_name(int seatalk_port, char *datagram) {
  int char_1, char_2, char_3, char_4;
  st_parse_target_waypoint_name(datagram, &char_1, &char_2, &char_3, &char_4);
  st_received_target_waypoint_name(seatalk_port, char_1, char_2, char_3, char_4);
}

void st_handle_course_computer_failure(int seatalk_port, char *datagram) {
  COURSE_COMPUTER_FAILURE_TYPE failure_type;
  st_parse_course_computer_failure(datagram, &failure_type);
  st_received_course_computer_failure(seatalk_port, failure_type);
}

void st_handle_autopilot_status(int seatalk_port, char *datagram) {
  int compass_heading, target_heading, alarms, rudder_position, display_flags;
  TURN_DIRECTION turning_direction;
  AUTOPILOT_MODE mode;
  st_parse_autopilot_status(datagram, &compass_heading, &turning_direction, &target_heading, &mode, &rudder_position, &alarms, &display_flags);
  st_received_autopilot_status(seatalk_port, compass_heading, turning_direction, target_heading, mode, rudder_position, alarms, display_flags);
}

void st_handle_waypoint_navigation(int seatalk_port, char *datagram) {
  int cross_track_error_present, cross_track_error_times_100, waypoint_bearing_present, waypoint_bearing, bearing_is_magnetic, waypoint_distance_present, waypoint_distance_times_100, direction_to_steer;
  st_parse_waypoint_navigation(datagram, &cross_track_error_present, &cross_track_error_times_100, &waypoint_bearing_present, &waypoint_bearing, &bearing_is_magnetic, &waypoint_distance_present, &waypoint_distance_times_100, &direction_to_steer);
  st_received_waypoint_navigation(seatalk_port, cross_track_error_present, cross_track_error_times_100, waypoint_bearing_present, waypoint_bearing, bearing_is_magnetic, waypoint_distance_present, waypoint_distance_times_100, direction_to_steer);
}

void st_handle_autopilot_command(int seatalk_port, char *datagram) {
  ST_AUTOPILOT_COMMAND autopilot_command;
  st_parse_autopilot_command(datagram, &autopilot_command);
  st_received_autopilot_command(seatalk_port, autopilot_command);
}

void st_handle_set_autopilot_response_level(int seatalk_port, char *datagram) {
  AUTOPILOT_RESPONSE_LEVEL response_level;
  st_parse_set_autopilot_response_level(datagram, &response_level);
  st_received_set_autopilot_response_level(seatalk_port, response_level);
}

void st_handle_autopilot_parameter(int seatalk_port, char *datagram) {
  int parameter, min_value, max_value, value;
  st_parse_autopilot_parameter(datagram, &parameter, &min_value, &max_value, &value);
  st_received_autopilot_parameter(seatalk_port, parameter, min_value, max_value, value);
}

void st_handle_heading(int seatalk_port, char *datagram) {
  int heading, locked_heading_active, locked_heading;
  st_parse_heading(datagram, &heading, &locked_heading_active, &locked_heading);
  st_received_heading(seatalk_port, heading, locked_heading_active, locked_heading);
}

void st_handle_set_rudder_gain(int seatalk_port, char *datagram) {
  int rudder_gain;
  st_parse_set_rudder_gain(datagram, &rudder_gain);
  st_received_set_rudder_gain(seatalk_port, rudder_gain);
}

void st_handle_set_autopilot_parameter(int seatalk_port, char *datagram) {
  int parameter, value;
  st_parse_set_autopilot_parameter(datagram, &parameter, &value);
  st_received_set_autopilot_parameter(seatalk_port, parameter, value);
}

void st_handle_enter_autopilot_setup(int seatalk_port, char *datagram) {
  st_parse_enter_autopilot_setup(datagram);
  st_received_enter_autopilot_setup(seatalk_port);
}

void st_handle_compass_variation(int seatalk_port, char *datagram) {
  int compass_variation;
  st_parse_compass_variation(datagram, &compass_variation);
  st_received_compass_variation(seatalk_port, compass_variation);
}

void st_handle_heading_and_rudder_position(int seatalk_port, char *datagram) {
  int heading, rudder_position;
  TURN_DIRECTION turning_direction;
  st_parse_heading_and_rudder_position(datagram, &heading, &turning_direction, &rudder_position);
  st_received_heading_and_rudder_position(seatalk_port, heading, turning_direction, rudder_position);
}

void st_handle_destination_waypoint_info(int seatalk_port, char *datagram) {
    char last_4, first_8;
    int more_records, last_record;
    st_parse_destination_waypoint_info(datagram, &last_4, &first_8, &more_records, &last_record);
    st_received_destination_waypoint_info(seatalk_port, last_4, first_8, more_records, last_record);
}

void st_handle_arrival_info(int seatalk_port, char *datagram) {
  int perpendicular_passed, circle_entered, char_1, char_2, char_3, char_4;
  char name[5];
  st_parse_arrival_info(datagram, &perpendicular_passed, &circle_entered, &char_1, &char_2, &char_3, &char_4);
  st_received_arrival_info(seatalk_port, perpendicular_passed, circle_entered, char_1, char_2, char_3, char_4);
}

void st_handle_gps_and_dgps_fix_info(int seatalk_port, char *datagram) {
  int signal_quality_available, signal_quality, hdop_available, hdop, antenna_height, satellite_count_available, satellite_count, geoseparation, dgps_age_available, dgps_age, dgps_station_id_available, dgps_station_id;
  st_parse_gps_and_dgps_fix_info(datagram, &signal_quality_available, &signal_quality, &hdop_available, &hdop, &antenna_height, &satellite_count_available, &satellite_count, &geoseparation, &dgps_age_available, &dgps_age, &dgps_station_id_available, &dgps_station_id);
  st_received_gps_and_dgps_fix_info(seatalk_port, signal_quality_available, signal_quality, hdop_available, hdop, antenna_height, satellite_count_available, satellite_count, geoseparation, dgps_age_available, dgps_age, dgps_station_id_available, dgps_station_id);
}

void seatalk_handle_datagram(int seatalk_port, char *datagram) {
  switch (DATAGRAM_ID(datagram)) {
    case ST_DATAGRAM_DEPTH_BELOW_TRANSDUCER:
      st_handle_depth_below_transducer(seatalk_port, datagram);
      break;
    case ST_DATAGRAM_EQUIPMENT_ID:
//      st_handle_equipment_id(seatalk_port, datagram);
      break;
    case ST_DATAGRAM_ENGINE_STATUS:
      st_handle_engine_rpm_and_pitch(seatalk_port, datagram);
      break;
    case ST_DATAGRAM_APPARENT_WIND_ANGLE:
      st_handle_apparent_wind_angle(seatalk_port, datagram);
      break;
    case ST_DATAGRAM_APPARENT_WIND_SPEED:
      st_handle_apparent_wind_speed(seatalk_port, datagram);
      break;
    case ST_DATAGRAM_WATER_SPEED:
      st_handle_water_speed(seatalk_port, datagram);
      break;
    case ST_DATAGRAM_TRIP_MILEAGE:
      st_handle_trip_mileage(seatalk_port, datagram);
      break;
    case ST_DATAGRAM_TOTAL_MILEAGE:
      st_handle_total_mileage(seatalk_port, datagram);
      break;
    case ST_DATAGRAM_WATER_TEMPERATURE:
      st_handle_water_temperature(seatalk_port, datagram);
      break;
    case ST_DATAGRAM_SPEED_DISTANCE_UNITS:
      st_handle_speed_distance_units(seatalk_port, datagram);
      break;
    case ST_DATAGRAM_LOG:
      st_handle_total_and_trip_mileage(seatalk_port, datagram);
      break;
    case ST_DATAGRAM_WATER_SPEED_WITH_AVERAGE:
      st_handle_average_water_speed(seatalk_port, datagram);
      break;
    case ST_DATAGRAM_PRECISE_WATER_TEMPERATURE:
      st_handle_precise_water_temperature(seatalk_port, datagram);
      break;
    case ST_DATAGRAM_SET_LAMP_INTENSITY:
      st_handle_set_lamp_intensity(seatalk_port, datagram);
      break;
    case ST_DATAGRAM_CANCEL_MOB:
      st_handle_cancel_mob(seatalk_port, datagram);
      break;
    case ST_DATAGRAM_CODELOCK_DATA:
      break;
    case ST_DATAGRAM_LAT_POSITION:
      st_handle_lat_position(seatalk_port, datagram);
      break;
    case ST_DATAGRAM_LON_POSITION:
      st_handle_lon_position(seatalk_port, datagram);
      break;
    case ST_DATAGRAM_SPEED_OVER_GROUND:
      st_handle_speed_over_ground(seatalk_port, datagram);
      break;
    case ST_DATAGRAM_COURSE_OVER_GROUND:
      st_handle_course_over_ground(seatalk_port, datagram);
      break;
    case ST_DATAGRAM_GMT_TIME:
      st_handle_gmt_time(seatalk_port, datagram);
      break;
    case ST_DATAGRAM_TRACK_KEYSTROKE:
      // accept_autopilot_remote_control_keystroke(seatalk_port, datagram);
      break;
    case ST_DATAGRAM_DATE:
      st_handle_date(seatalk_port, datagram);
      break;
    case ST_DATAGRAM_SATELLITE_STATUS:
      st_handle_satellite_info(seatalk_port, datagram);
      break;
    case ST_DATAGRAM_POSITION_FIX:
      st_handle_lat_lon_position(seatalk_port, datagram);
      break;
    case ST_DATAGRAM_SET_COUNTDOWN_TIMER:
      st_handle_set_countdown_timer(seatalk_port, datagram);
      break;
    case ST_DATAGRAM_INITIALIZATION_BROADCAST:
      break;
    case ST_DATAGRAM_DEPTH_UNITS:
      break;
    case ST_DATAGRAM_WIND_ALARM:
      st_handle_wind_alarm(seatalk_port, datagram);
      break;
    case ST_DATAGRAM_ALARM_ACKNOWLEDGEMENT:
      st_handle_alarm_acknowledgement(seatalk_port, datagram);
      break;
    case ST_DATAGRAM_SECOND_EQUIPMENT_ID:
      break;
    case ST_DATAGRAM_MOB_KEYSTROKE:
      st_handle_set_mob(seatalk_port, datagram);
      break;
    case ST_DATAGRAM_ST60_REMOTE_CONTROL_KEYSTROKE:
      st_handle_maxview_keystroke(seatalk_port, datagram);
      break;
    case ST_DATAGRAM_SET_LAMP_INTENSITY_2:
      st_handle_set_lamp_intensity(seatalk_port, datagram);
      break;
    case ST_DATAGRAM_COURSE_COMPUTER_SETUP:
//      st_handle_course_computer_setup(seatalk_port, datagram);
      break;
    case ST_DATAGRAM_TARGET_WAYPOINT_NAME:
//      st_handle_target_waypoint_name(seatalk_port, datagram);
      break;
    case ST_DATAGRAM_COURSE_COMPUTER_STARTUP_BROADCAST:
//      st_handle_course_computer_startup_broadcast(seatalk_port, datagram);
      break;
    case ST_DATAGRAM_AUTOPILOT_STATUS:
      st_handle_autopilot_status(seatalk_port, datagram);
      break;
    case ST_DATAGRAM_WAYPOINT_NAVIGATION:
      st_handle_waypoint_navigation(seatalk_port, datagram);
      break;
    case ST_DATAGRAM_AUTOPILOT_COMMAND:
      // accept_autopilot_remote_control_keystroke(seatalk_port, datagram);
      break;
    case ST_DATAGRAM_SET_AUTOPILOT_RESPONSE_LEVEL:
      st_handle_set_autopilot_response_level(seatalk_port, datagram);
      break;
    case ST_DATAGRAM_AUTOPILOT_PARAMETER:
      st_handle_autopilot_parameter(seatalk_port, datagram);
      break;
    case ST_DATAGRAM_HEADING:
      st_handle_heading(seatalk_port, datagram);
      break;
    case ST_DATAGRAM_DEVICE_INFORMATION:
//      st_handle_device_information(seatalk_port, datagram);
      break;
    case ST_DATAGRAM_SET_RUDDER_GAIN:
      st_handle_set_rudder_gain(seatalk_port, datagram);
      break;
    case ST_DATAGRAM_SET_AUTOPILOT_PARAMETER:
      st_handle_set_autopilot_parameter(seatalk_port, datagram);
      break;
    case ST_DATAGRAM_ENTER_AUTOPILOT_SETUP:
      st_handle_enter_autopilot_setup(seatalk_port, datagram);
      break;
    case ST_DATAGRAM_AUTOPILOT_STATUS_IN_SETUP_MODE:
//      st_handle_autopilot_status_in_setup_mode(seatalk_port, datagram);
      break;
    case ST_DATAGRAM_COMPASS_VARIATION:
      st_handle_compass_variation(seatalk_port, datagram);
      break;
    case ST_DATAGRAM_VERSION:
//      st_handle_version(seatalk_port, datagram)
      break;
    case ST_DATAGRAM_HEADING_AND_RUDDER_POSITION:
      st_handle_heading_and_rudder_position(seatalk_port, datagram);
      break;
    case ST_DATAGRAM_WAYPOINT:
//      st_handle_waypoint(seatalk_port, datagram);
      break;
    case ST_DATAGRAM_DESTINATION_WAYPOINT:
//      st_handle_destination_waypoint(seatalk_port, datagram);
      break;
    case ST_DATAGRAM_WAYPOINT_ARRIVAL_NOTICE:
      st_handle_arrival_info(seatalk_port, datagram);
      break;
    case ST_DATAGRAM_REQUEST_DEVICE_IDENTIFICATION:
//      st_handle_request_device_identification(seatalk_port, datagram);
      break;
    case ST_DATAGRAM_GPS_AND_DGPS_FIX_INFO:
      st_handle_gps_and_dgps_fix_info(seatalk_port, datagram);
      break;
    case ST_DATAGRAM_PURPOSE_UNKNOWN:
      break;
    case ST_DATAGRAM_ALARM_NOTICE_1_FOR_GUARD_1_OR_GUARD_2:
      break;
    case ST_DATAGRAM_ALARM_NOTICE_2_FOR_GUARD_1_OR_GUARD_2:
      break;
  }
}
#endif
