#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "seatalk_transport_layer.h"
#include "seatalk_datagram.h"
#include "seatalk_state.h"
#include "test.h"

void main() {
  printf("Running tests...\n");
  test_seatalk_transport_layer();
  test_seatalk_datagram();
  test_seatalk_state();
  // test parsing each known message
  // test test FSM for
  //   transmit
  //   receive
  //   collision management
  // test preparation of each message
  // test message read/write
  // test initiation of transmission in every possible state
  printf("Tests completed with %d assertions (%d failures)\n", total_assertions(), total_failures());
}
