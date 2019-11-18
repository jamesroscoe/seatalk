// Bus communication functions

// Implementation Instructions:
//   * a byte is received as s series of 8 bits followed by 1 command bit.
//   * each new byte is begun with a high-to-low transition on the SeaTalk bus
//   * the high-to-low should generate a falling edge interrupt request. The
//     IRQ handler should call initiate_seatalk_receive_character() to
//     initialize the byte receiver. Since any 1 to zero transition will trigger
//     the IRQ (even mid-byte) the return value needs to be checked. If 1 then
//     initialize a timer to call seatalk_receive_bit() at each 1/4800 second
//     interval to receive the byte and command bit.

// called from hardware layer when start bit received
// return values:
//   0 -- already receiving; hardware layer should do nothing
//   1 -- hardware layer should initiate the bit receiver timer
int initiate_seatalk_receive_character(void);

// called from hardware layer start bit handler if
// SHOULD_INITIATE_SEATALK_RECEIVE_CHARACTER returns 1

// called every 208333 to check the next bit value. Stop checking bit values
// after this returns 0.
// return values:
//   0 -- no more bits to receive; hardware layer should terminate rx timer
//   1 -- more bits expected
int seatalk_receive_bit(void);

// called from seatalk_state when there is a new datagram to transmit
void initiate_seatalk_transmitter(void);

// called from bit transmitter timer handler to set tx line value
// return values:
//   0 -- no more bits to transmit; hardware layer should terminate tx timer
//   any other value -- fire timer again after this many bit durations
int seatalk_transmit_bit(void);

typedef enum {
  BUS_STATE_IDLE,
  BUS_STATE_RECEIVING,
  BUS_STATE_WAIT_FOR_IDLE,
  BUS_STATE_TRANSMITTING,
} BUS_STATE;
