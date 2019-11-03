#define TIME_TO_LIVE 5
#define HEADING_SIZE 4
#define FLAG_SIZE 4
#define MODE_SIZE 10
#define ANGLE_SIZE 5

#define ALARM_SHALLOW_WATER 0x0001
#define ALARM_DEEP_WATER 0x0002
#define ALARM_ANCHOR 0x0004
#define ALARM_APPARENT_WIND_ANGLE_LOW 0x0008
#define ALARM_APPARENT_WIND_ANGLE_HIGH 0x0010
#define ALARM_APPARENT_WIND_SPEED_LOW 0x0020
#define ALARM_APPARENT_WIND_SPEED_HIGH 0x0040
#define ALARM_TRUE_WIND_ANGLE_LOW 0x0080
#define ALARM_TRUE_WIND_ANGLE_HIGH 0x0100
#define ALARM_TRUE_WIND_SPEED_LOW 0x0200
#define ALARM_TRUE_WIND_SPEED_HIGH 0x0400
#define ALARM_AUTOPILOT_OFF_COURSE 0x0800
#define ALARM_AUTOPILOT_WIND_SHIFT 0x1000

void initialize_status(void);

int get_active_alarms(int *active_alarms);
void set_active_alarms(int alarms);

int get_depth_below_transducer(int *feet_time_10); // depth in 0.1 foot units
void set_depth_below_transducer(int feet_times_10);

typedef enum DISTANCE_UNITS { DISTANCE_UNITS_NAUTICAL, DISTANCE_UNITS_STATUTE, DISTANCE_UNITS_METRIC };

//enum DISTANCE_UNITS get_distance_units(void);
//void set_distance_units(enum DISTANCE_UNITS distance_units);

typedef enum ENGINE_ID { ENGINE_ID_SINGLE, ENGINE_ID_PORT, ENGINE_ID_STARBOARD };

typedef struct ENGINE_STATUS {
  int rpm;
  int pitch_percent;
};

int get_engine_status(enum ENGINE_ID engine_id, struct ENGINE_STATUS *engine_status);
void set_engine_rpm(enum ENGINE_ID engine_id, int rpm);
void set_engine_prop_pitch_percent(enum ENGINE_ID engine_id, int prop_pitch_percent);

typedef enum ANGLE_REFERENCE { ANGLE_REFERENCE_TRUE, ANGLE_REFERENCE_MAGNETIC, ANGLE_REFERENCE_BOAT };

int get_apparent_wind_angle(int *degrees);
void set_apparent_wind_angle(int degrees_right);
int get_apparent_wind_speed(int *knots_times_10);
void set_apparent_wind_speed(int knots_times_10);

typedef enum TURN_DIRECTION { TURN_DIRECTION_NONE, TURN_DIRECTION_LEFT, TURN_DIRECTION_RIGHT };

int get_heading(int *degrees);
void set_heading(int degrees);
int get_heading_reference(enum ANGLE_REFERENCE *referebce);
void set_heading_reference(enum ANGLE_REFERENCE reference);
int get_turn_direction(enum TURN_DIRECTION *direction);
void set_turn_direction(enum TURN_DIRECTION direction);
int get_water_speed(int *knots_times_100);
void set_water_speed(int knots_times_100);
int get_average_water_speed(int *knots_times_100);
void set_average_water_speed(int knots_times_100);
int get_rudder_position(int *degrees_right);
void set_rudder_position(int degrees_right);

int get_course_over_ground(int *degrees);
void set_course_over_ground(int degrees);
int get_course_over_ground_reference(enum ANGLE_REFERENCE *reference);
void set_course_over_ground_reference(enum ANGLE_REFERENCE reference);
int get_speed_over_ground(int *knots_times_100);
void set_speed_over_ground(int knots_times_100);
int get_trip_mileage(int *nautical_miles_times_100);
void set_trip_mileage(int nautical_miles_times_100);
int get_total_mileage(int *nautical_miles_times_10);
void set_total_mileage(int nautical_miles_times_10);

int get_water_temperature(int *degrees_celsius_times_10);
void set_water_temperature(int degrees_celsius_times_10);

//int get_lamp_intensity(int *intensity);
//void set_lamp_intensity(int intensity);

typedef struct POSITION {
  int hemisphere_latitude;
  int degrees_latitude;
  int minutes_latitude_times_1000;
  int hemisphere_longitude;
  int degrees_longitude;
  int minutes_longitude_times_1000;
};

int get_position(struct POSITION *position);
void set_position_latitude(int hemisphere, int degrees, int minutes_times_1000);
void set_position_longitude(int hemisphere, int degrees, int minutes_times_1000);
int get_compass_variation(int *degrees_west);;
void set_compass_variation(int degrees_west);

struct GPS_FIX_QUALITY {
  int signal_quality_valid;
  int signal_quality;
  int position_error_valid;
  int position_error;
  int antenna_height;
  int satellite_count_valid;
  int satellite_count;
  int geoseparation;
  int dgps_age_valid;
  int dgps_age;
  int dgps_station_id_valid;
  int dgps_station_id;
};

int get_gps_fix_quality(struct GPS_FIX_QUALITY *fix_quality);
void set_gps_fix_satellite_quality(int satellite_quality_valid, int satellite_quality);
void set_gps_fix_position_error(int position_eror_valid, int position_error);
void set_gps_fix_antenna_height(int antenna_height);
void set_gps_fix_satellite_count(int satellite_count_valid, int satellite_count, int geoseparation);
void set_gps_fix_dgps_age(int dgps_age_valid, int dgps_age);
void set_gps_fix_dgps_station_id(int dgps_station_id_valid, int dgps_station_id);

typedef struct NAVIGATION_STATUS {
  char waypoint_name[5];
  struct POSITION waypoint_position;
  int waypoint_bearing;
  enum ANGLE_REFERENCE waypoint_bearing_reference;
  int waypoint_range_in_nautical_miles_times_100;;
  int cross_track_error_times_100;
};

int get_navigation_status(struct NAVIGATION_STATUS *navigation_status);
void set_navigation_status_waypoint_name(char *name);
void set_navigation_status_waypoint_position_latitude(int hemisphere, int degrees, int minutes_times_1000);
void set_navigation_status_waypoint_position_longitude(int hemisphere, int degrees, int minutes_times_1000);
void set_navigation_status_waypoint_bearing_and_distance(int degrees, int nautical_miles_times_100);
void set_navigation_status_cross_track_error(int nautical_miles_times_100);

typedef struct DATE_AND_TIME {
  int year;
  int month;
  int day;
  int hour;
  int minute;
  int second;
};

int get_gmt_date_and_time(struct DATE_AND_TIME *gmt_date_and_time);
void set_gmt_date(int year, int month, int day);
void set_gmt_time(int hour, int minute, int second);

typedef enum AUTOPILOT_MODE { AUTOPILOT_MODE_STANDBY, AUTOPILOT_MODE_AUTO, AUTOPILOT_MODE_VANE, AUTOPILOT_MODE_TRACK };
typedef enum AUTOPILOT_RESPONSE_LEVEL { AUTOPILOT_RESPONSE_LEVEL_AUTOMATIC_DEADBAND, AUTOPILOT_RESPONSE_LEVEL_MINIMUM_DEADBAND };

typedef struct AUTOPILOT_STATUS {
  int target_heading;
  enum AUTOPILOT_MODE mode;
  enum AUTOPILOT_RESPONSE_LEVEL response_level;
  int rudder_gain;
};

int get_autopilot_status(struct AUTOPILOT_STATUS *new_ap_status);
void set_autopilot_target_heading(int degrees);
void set_autopilot_mode(enum AUTOPILOT_MODE mode);
void set_autopilot_response_level(enum AUTOPILOT_RESPONSE_LEVEL response_level);
void set_autopilot_rudder_gain(int rudder_gain);
