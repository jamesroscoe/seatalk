#include "seatalk_protocol.h"
#include "seatalk_transport_layer.h"

int st_initialize() {
  seatalk_initialize_transport_layer();
  return 0;
}

void st_exit() {
  seatalk_exit_transport_layer();
}
