// Bus communication functions

// called from hardware layer when start bit received
// return values:
//   0 -- already receiving; hardware layer should do nothing
//   1 -- hardware layer should initiate the bit receiver timer
int initiate_seatalk_receive_character(void);

// called from bit receiver timer handler to store rx line value
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

typedef enum BUS_STATE {
  BUS_STATE_IDLE,
  BUS_STATE_RECEIVING,
  BUS_STATE_WAIT_FOR_IDLE,
  BUS_STATE_TRANSMITTING,
};
