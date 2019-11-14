#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "test.h"
#include "../seatalk_transport_layer.h"
#include "seatalk_transport_layer_test.h"
#include "seatalk_datagram_test.h"
#include "boat_status_test.h"
#include "boat_sensor_test.h"
#include "seatalk_command_test.h"
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
  // test read/write of status values
  test_boat_status();
  // test read/write of sensor values
  test_boat_sensor();
  // test generating commands
  test_seatalk_command();
  // test status values update when new datagrams received
  // test sensor values generate correct datagrams to transmit
  // test commands generate correct datagrams to transmit
  test_seatalk_protocol();
  printf("Tests completed with %d assertions (%d failures)\n", total_assertions(), total_failures());
}
