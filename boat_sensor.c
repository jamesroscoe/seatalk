#include "logger.h"
#include "generic_time.h"
#include "settings.h"

#define SENSOR_VARIABLE(NAME) sensor_ ## NAME
#define SENSOR_EXPIRY_TIME(NAME) sensor_##NAME##_expiry
#define VALID_SENSOR(NAME) timeout_still_valid(SENSOR_EXPIRY_TIME(NAME))
#define VALIDATE_SENSOR(NAME) restart_timeout(&SENSOR_EXPIRY_TIME(NAME), SENSOR_TIME_TO_LIVE)
#define SET_SENSOR(NAME, VALUE) VALIDATE_SENSOR(NAME);\
  SENSOR_VARIABLE(NAME) = VALUE;\
  initiate_seatalk_transmitter()
#define INVALIDATE_SENSOR(NAME) invalidate_timeout(&SENSOR_EXPIRY_TIME(NAME)
#define DEFINE_SENSOR_EXPIRY(NAME) timeout SENSOR_EXPIRY_TIME(NAME) = TIMEOUT_DEFAULT;
#define DEFINE_SENSOR(NAME, TYPE) TYPE SENSOR_VARIABLE(NAME);\
  DEFINE_SENSOR_EXPIRY(NAME)

// sensor properies are intended to be written to the SeaTalk bus
// This includes remote control units
DEFINE_SENSOR(compass, int);

void set_compass_sensor(int value) {
//  LOG("set_ap_heading(%d) at %d\n", value, get_time());
  if ((value >= 0) && (value <= 359)) {
    SET_SENSOR(compass, value);
//    LOG("compass: %d, expires: %d\n", compass, EXPIRY_TIME(compass));
//    LOG("VALID(compass): %d\n", VALID(compass));
  } else {
    INVALIDATE_SENSOR(compass);
  }
}
