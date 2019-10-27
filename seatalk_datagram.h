#define SHALLOW_WATER_ALARM 0x0001
#define DEEP_WATER_ALARM 0x0002
#define ANCHOR_ALARM 0x0004

typedef enum ENGINE_ID { ENGINE_ID_SINGLE, ENGINE_ID_PORT, ENGINE_ID_STARBOARD };
typedef enum DISTANCE_UNITS { DISTANCE_UNITS_NAUTICAL, DISTANCE_UNITS_STATUTE, DISTANCE_UNITS_METRIC };

int get_datagram_length(char second_byte);
void handle_seatalk_datagram(char *datagram);

int build_compass_datagram(char *datagram, int compass);
