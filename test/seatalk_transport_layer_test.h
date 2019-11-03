#define WAIT_FOR_IDLE -100
#define NO_WAIT_FOR_IDLE -101
#define CAN_TRANSMIT_FALSE -110

enum BUS_STATE get_bus_state();

void test_seatalk_transport_layer();
