#include <stdio.h>
#include "defines.h"
#include "test.h"
#include "seatalk_transport_layer.h"
#include "seatalk_hardware_layer.h"

#include "../seatalk_transport_layer.c"

/*
int simulate_transmit_character(char byte, int command_bit, char receive_byte, int receive_command_bit) {
  seatalk_begin_transmit_byte(); // sets bus state
  if (!seatalk_can_transmit()) {
     return CAN_TRANSMIT_FALSE; // no character ready
  }
  seatalk_byte_to_send(
}
*/

/*
int simulate_clear_transmit() {
  int can_transmit_response, receive_response;
  seatalk_begin_transmit_byte(); // called by transmit timer event
  can_transmit_response = seatalk_can_transmit();
  if (!can_transmit_response) {
    return CAN_TRANSMIT_FALSE; // no datagram ready
  }
  while (can_transmit_response) {
    char transmit_byte;
    int transmit_command_bit;
    seatalk_byte_to_send(&transmit_byte, &transmit_command_bit);
    receive_response = simulate_receive_character(transmit_byte, transmit_command_bit);
    seatalk_begin_transmit_byte();
    can_transmit_response = seatalk_can_transmit();
  }
  if (receive_response == WAIT_FOR_IDLE) { // expected result
    return 0; // datagram complete; bus waiting 10 cycles
  } else {
    return receive_response;
  }
}
*/

enum BUS_STATE get_bus_state(void) {
  return bus_state;
}

void print_bus_state(enum BUS_STATE state) {
  printf("state: %d (IDLE=%d, RECEIVING=%d, TRANSMITTING=%d, WAIT_FOR_IDLE=%d)\n",
    state, BUS_STATE_IDLE, BUS_STATE_RECEIVING, BUS_STATE_TRANSMITTING, 
    BUS_STATE_WAIT_FOR_IDLE);
}

void simulate_receive_datagram(char *datagram, int length) {
  // printf("datagram (%d bytes): ", length);
  for (int i = 0; i < length; i++) {
    // printf("%.2x ", datagram[i]);
    simulate_receive_character(datagram[i], (i == 0) ? 1 : 0);
  }
  // printf("\n");
}

void prepare_transmit_datagram(char *datagram, int length) {
  for (int i = 0; i < length; i++) {
    transmit_buffer[i] = datagram[i];
  }
  transmit_buffer_position = 0;
  transmit_datagram_bytes_remaining = length;
}

// simulate starting the transmit loop
// return value: bit 0 is whether the current bit number changed
//               bit 1 is whether more bits are to be sent
int simulate_initialize_seatalk_hardware_transmitter() {
  int current_bit = tx_bit_number;
  int more_bits = seatalk_transmit_bit();
  return (current_bit != tx_bit_number) | (more_bits << 1);
}

/*
void simulate_send_datagram(char *datagram, int length, char *receive_datagram, char receive_length) {
  populate_transmit_buffer(&transmit_buffer, datagram, length);
  for (int i = 0; i < length; i++) {
    if (i == receive_length) {
      break;
    }
    int command_bit = (i == 0) ? 1 : 0;
    simulate_transmit_character(datagram[i], command_bit, receive_datagram[i], command_bit);
  }
}
*/

TEST(bus_returns_to_idle_state)
  // after sending a datagram the bus goes into a waiting state for
  // 10 bit cycles
// receive a character to force wait_for_idle
  refute(simulate_receive_character(0x84, 1), "simulate_receive_character should complete successfully");
  assert_equal_int((int)BUS_STATE_WAIT_FOR_IDLE, (int)get_bus_state(),
    "bus_state should be wait_for_idle after byte received");
  refute(seatalk_can_transmit(), "seatalk_can_transmit() should return false");
  refute(seatalk_transmit_bit(), "seatalk_transmit_bit() should have returned FALSE");
  assert_equal_int((int)BUS_STATE_IDLE, (int)get_bus_state(),
    "bus_state should be idle after call to seatalk_bus_idle");
}

TEST(incomplete_datagram)
  // send partial datagram and ensure autopilot status not updated
  // send complete datagram and ensure autopilot status updated
  struct AUTOPILOT_STATUS ap_status;
  simulate_receive_datagram("\x84\xc6\x07\x00\x00\x00\x00\x00\x0f", 5);
  assert(get_autopilot_status(&ap_status),
    "get_ap_status should return non-zero for failure");
  simulate_receive_datagram("\x84\xc6\x07\x00\x00\x00\x00\x00\x0f", 9);
  assert(!get_autopilot_status(&ap_status),
    "get_ap_status should pass with zero result");
}

TEST(initialize_seatalk_hardware_transmitter_sans_datagram)
  // have no pending datagram
  // begin (simulated) send loop
  // result should be zero (no change in bit #, no more bits to send)
  prepare_transmit_datagram("", 0);
  assert_equal_int(0, simulate_initialize_seatalk_hardware_transmitter(), "there should be no bit number change and no more bits to send");
}

TEST(transmit_datagram)
//  simulate_send_datagram("\x84\xc6\x07\x00\x00\x00\x00\x00\x0f", 9);
}

TEST(collision)
//  prepare_datagram("\x84\xc6\x07\x00\x00\x00\x00\x00\x0f", 9);
//  simulate_character
//  simulate_datagram("\x84\xc6\x97", 3);
//  assert_equal(0, receive_datagram_buffer_position, "after cancel 
}

void test_seatalk_transport_layer() {
  printf("---Testing seatalk_protocol.c\n");
  test_bus_returns_to_idle_state();
  test_incomplete_datagram();
  test_initialize_seatalk_hardware_transmitter_sans_datagram();
  test_transmit_datagram();
  test_collision();
}
