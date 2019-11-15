#define SENSOR_ACCESSOR(NAME, TYPE) int has_##NAME##_sensor_value_to_transmit(void);\
int pop_##NAME##_sensor_value(TYPE *NAME);\
void update_##NAME##_sensor(TYPE NAME);\
void invalidate_##NAME##_sensor(void)\

SENSOR_ACCESSOR(heading, int);
SENSOR_ACCESSOR(water_speed_in_knots_times_100, int);
SENSOR_ACCESSOR(apparent_wind_angle, int);
SENSOR_ACCESSOR(apparent_wind_speed_in_knots_times_10, int);
SENSOR_ACCESSOR(depth_below_transducer_in_feet_times_10, int);
SENSOR_ACCESSOR(course_over_ground, int);
SENSOR_ACCESSOR(speed_over_ground_in_knots_times_100, int);
SENSOR_ACCESSOR(water_temperature_in_degrees_celsius_times_10, int);
SENSOR_ACCESSOR(rudder_position_in_degrees_right, int);

void initialize_sensors(void);
