#include "defines.h"
#include "test.h"
#include "seatalk_hardware_layer.h"
#include "../seatalk_transport_layer.h"
#include "seatalk_transport_layer_test.h"

int rx_bit_value = 0;

int init_seatalk_hardware_layer(void) {}  // not used here
void exit_seatalk_hardware_layer(void) {} // not used here

int get_seatalk_hardware_bit_value(void) {
  return rx_bit_value;
}

void set_seatalk_hardware_bit_value(int bit_value) {
  rx_bit_value = bit_value;
}

void initiate_seatalk_hardware_transmitter(int bit_delay) {
}

int simulate_receive_character(char byte, int command_bit) {
  // set up character to receive
  if (!initiate_seatalk_receive_character()) {
    return TRANSPORT_LAYER_REFUSED_INITIATE_RECEIVE;
  }
  for (int i = -1; i <= 9; i++) {
    int more_bits_expected;
    if (i == -1) { // start bit
      rx_bit_value = 0;
    } else {
      int old_rx_bit_value = rx_bit_value;
      if (rx_bit_value < old_rx_bit_value) { // falling?
        if (initiate_seatalk_receive_character()) {
          return TRANSPORT_LAYER_ACCEPTED_INITIATE_RECEIVE_MID_BYTE;
        }
      }
      if (i <= 7) {
        rx_bit_value = (byte >> i) & 1;
      } else if (i == 8) {
        rx_bit_value = command_bit;
      } else {
        rx_bit_value = 1;
      }
      more_bits_expected = (seatalk_receive_bit());
      if (i < 9) {
        if (!more_bits_expected) {
          return TRANSPORT_LAYER_REFUSED_MORE_BITS_BEFORE_END_OF_BYTE;
        }
      } else if (more_bits_expected) {
        return TRANSPORT_LAYER_REQUESTED_MORE_BITS_AFTER_STOP_BIT;
      }
    }
  }
  return 0;
}

/*
int simulate_transmit_character(char byte, int command_bit, char receive_byte, int command_bit) {
  
  seatalk_begin_transmit_byte(); // sets bus state
  if (!seatalk_can_transmit()) {
     return CAN_TRANSMIT_FALSE; // no character ready
  }
  seatalk_byte_to_send
}

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

