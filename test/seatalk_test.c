#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "test.h"
#include "../seatalk_transport_layer.h"
#include "seatalk_transport_layer_test.h"
#include "seatalk_datagram_test.h"
#include "boat_status_test.h"
#include "seatalk_protocol_test.h"

void main() {
  printf("Running tests...\n");
  // test test FSM for
  //   transmit
  //   receive
  //   collision management
  // test initiation of transmission in every possible state
  test_seatalk_transport_layer();
  // test preparation of each message
  // test parsing each known message
  test_seatalk_datagram();
  test_boat_status();
  // test message read/write
  test_seatalk_protocol();
  printf("Tests completed with %d assertions (%d failures)\n", total_assertions(), total_failures());
}
