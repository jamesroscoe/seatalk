typedef enum {
  SENSOR_ID_NONE,
  SENSOR_ID_HEADING,
  SENSOR_ID_WATER_SPEED,
  SENSOR_ID_APPARENT_WIND_ANGLE,
  SENSOR_ID_APPARENT_WIND_SPEED,
  SENSOR_ID_DEPTH_BELOW_TRANSDUCER,
  SENSOR_ID_COURSE_OVER_GROUND,
  SENSOR_ID_SPEED_OVER_GROUND,
  SENSOR_ID_WATER_TEMPERATURE,
  SENSOR_ID_RUDDER_POSITION
} SENSOR_ID;

void wake_transmitter(void);
int seatalk_sensor_pending(SENSOR_ID *sensor_id);
void handle_seatalk_datagram(char *datagram);
int get_pending_datagram(char *datagram);
