#include "seatalk_transport_layer.h"
#include "seatalk_hardware_layer.h"
#include "seatalk_protocol.h"
#include "seatalk_talker.h"
#include "seatalk_listener.h"
#include "seatalk_consumer.h"
#include "seatalk_datagram.h"
#include "logger.h"

#define IDLE_BITS 10

typedef struct {
    // transport layer state
    // receive data
    int rx_bit_number;
    char rx_byte;
    int rx_command_bit;
    char receive_buffer[MAX_DATAGRAM_LENGTH];
    int receive_buffer_position;
    int receive_datagram_bytes_remaining;

    // transmit data
    int tx_bit_number; // must be initialized to -1
    char tx_byte; // must be initialized to 0
    int tx_command_bit; // must be initialized to 1
    char transmit_buffer[MAX_DATAGRAM_LENGTH];
    int transmit_buffer_position; // must be initialized to 0
    int transmit_datagram_bytes_remaining; // must be initialized to 0

    // general
    BUS_STATE bus_state; // must be initialized to BUS_STATE_IDLE
} SEATALK_TRANSPORT_STATE;

SEATALK_TRANSPORT_STATE seatalk_transport_state[SEATALK_PORTS];

int seatalk_initialize_transport_layer() {
  int i;
  for (i = 0; i < SEATALK_PORTS; i++) {
    seatalk_transport_state[i].rx_bit_number = -1;
    seatalk_transport_state[i].rx_byte = 0;
    seatalk_transport_state[i].rx_command_bit = 0;
    seatalk_transport_state[i].receive_buffer_position = 0;
    seatalk_transport_state[i].receive_datagram_bytes_remaining = 255;
    seatalk_transport_state[i].tx_bit_number = -1;
    seatalk_transport_state[i].tx_byte = 0;
    seatalk_transport_state[i].tx_command_bit = 1;
    seatalk_transport_state[i].transmit_buffer_position = 0;
    seatalk_transport_state[i].transmit_datagram_bytes_remaining = 0;
    seatalk_transport_state[i].bus_state = BUS_STATE_IDLE;
  }
  seatalk_init_hardware_signal();
  seatalk_init_hardware_irq();
}

void seatalk_exit_transport_layer() {
  seatalk_exit_hardware_signal();
  seatalk_exit_hardware_irq();
}

void set_bus_state(int seatalk_port, BUS_STATE new_state)
{
    seatalk_transport_state[seatalk_port].bus_state = new_state;
}

// confirm no collision; ready next byte
// note: pattern is send-byte, receive-byte. Next transmit cannot take place
//   before receive because of event timing.
int seatalk_byte_transmitted(int seatalk_port)
{
    SEATALK_TRANSPORT_STATE *state = &seatalk_transport_state[seatalk_port];
#ifdef DEBUG
    LOG("seatalk_byte_transmitted: %x\n", state->tx_byte);
#endif
    state->transmit_buffer_position++;
    if (--state->transmit_datagram_bytes_remaining == 0) {
        return 1;
    } else {
        return 0;
    }
}

// collision detected; cancel current datagram.
void cancel_datagram(int seatalk_port)
{
    SEATALK_TRANSPORT_STATE *state = &seatalk_transport_state[seatalk_port];
    state->transmit_buffer_position += state->transmit_datagram_bytes_remaining;
    state->transmit_datagram_bytes_remaining = 0;
}

int seatalk_byte_received(int seatalk_port, char data_byte, int command_bit)
{
    SEATALK_TRANSPORT_STATE *state = &seatalk_transport_state[seatalk_port];
    BUS_STATE current_state = state->bus_state;
    set_bus_state(seatalk_port, BUS_STATE_WAIT_FOR_IDLE);
#ifdef DEBUG
    LOG("byte received: %x command_bit: %d bytes_remaining: %d", data_byte, command_bit, state->receive_datagram_bytes_remaining);
#endif
    if (current_state == BUS_STATE_TRANSMITTING) {
        // collision; cancel current datagram and idle the bus for 10 cycles
        if (data_byte != state->tx_byte) {
#ifdef DEBUF
            LOG("data byte (%2x) != tx_byte (%2x)", data_byte, state.tx_byte);
#endif
            cancel_datagram(seatalk_port);
            return 1;
        }
    }
    if (command_bit) { // parity (command) bit is high: start new command
#ifdef DEBUG
        LOG("command bit received; emptying buffer\n");
#endif
        state->receive_buffer_position = 0;
        state->receive_datagram_bytes_remaining = 255;
    }
    state->receive_buffer[state->receive_buffer_position++] = data_byte;
    if (state->receive_datagram_bytes_remaining == 254) { // second character
        state->receive_datagram_bytes_remaining = GET_DATAGRAM_LENGTH(data_byte); // datagram length
    } else if (!state->receive_datagram_bytes_remaining) {
        seatalk_handle_datagram(seatalk_port, state->receive_buffer);
    } else {
        state->receive_datagram_bytes_remaining -= 1;
    }
    if (current_state == BUS_STATE_TRANSMITTING) {
        if (seatalk_byte_transmitted(seatalk_port)) { // confirms no collision; ready next byte
            return 1;
        } else {
            return 0;
        }
    } else {
        return 1;
    }
}

int seatalk_initiate_receive_character(int seatalk_port)
{
    SEATALK_TRANSPORT_STATE *state = &seatalk_transport_state[seatalk_port];

    BUS_STATE current_state = state->bus_state;
    // rx_bit_number is -1 if we are not currently receiving a byte
    if (state->rx_bit_number != -1) { // are we mid-character?
        return 0;                // exit and tell receiver to ignore this 1 to 0 transition
    }
#ifdef DEBUG
    if (state->tx_bit_number != 0) {
        LOG("starting new character after tx bit %d (should be 0)\n", state->tx_bit_number);
    }
#endif
    if ((current_state == BUS_STATE_IDLE) || (current_state == BUS_STATE_WAIT_FOR_IDLE)) {
        set_bus_state(seatalk_port, BUS_STATE_RECEIVING);
    }
    return 1;
}

int seatalk_receive_bit(int seatalk_port)
{
    SEATALK_TRANSPORT_STATE *state = &seatalk_transport_state[seatalk_port];

    int bit_value = seatalk_get_hardware_bit_value(seatalk_port);
    if (++state->rx_bit_number == 0) {
#ifdef DEBUG
        LOG("rx beginning new character\n");
#endif
        state->rx_byte = 0;
        state->rx_command_bit = 0;
    }
    if (state->rx_bit_number <= 7) {
#ifdef DEBUG
        if (bit_value != ((state->tx_byte >> state->rx_bit_number) & 0x1)) {
            LOG("received %d but expected %d\n", bit_value, !bit_value);
        }
#endif
        state->rx_byte |= (bit_value << state->rx_bit_number);
        return 1;
    } else if (state->rx_bit_number == 8) {
        // we have now received all 8 data bits
        // check command (parity) bit status
        state->rx_command_bit = bit_value;
        return 1;
    } else { // by process of elimination this must be bit 9, stop bit
        state->rx_bit_number = -1;
        if (!bit_value) {
            // stop bit needs to be high. If it is not then we are misaligned
            // with the actual bytes and must discard this byte. We should
            // eventually "walk" around to correct start bit/stop bit alignment.
//      LOG("%x/%d received but stop_bit low", rx_byte, rx_command_bit);
            set_bus_state(seatalk_port, BUS_STATE_IDLE);
            return 0;
        }
        if (seatalk_byte_received(seatalk_port, state->rx_byte, state->rx_command_bit)) {
            seatalk_initiate_hardware_transmitter(seatalk_port, IDLE_BITS);
        }
        return 0;
    }
}

void seatalk_wake_transmitter(int seatalk_port)
{
    if (seatalk_transport_state[seatalk_port].bus_state == BUS_STATE_IDLE) {
        seatalk_initiate_hardware_transmitter(seatalk_port, 1);
    }
}

int seatalk_can_transmit(int seatalk_port)
{
    SEATALK_TRANSPORT_STATE *state = &seatalk_transport_state[seatalk_port];
    return state->transmit_datagram_bytes_remaining;
}

// get next datagram byte
int seatalk_byte_to_send(int seatalk_port, char *data_byte, int *command_bit)
{
    SEATALK_TRANSPORT_STATE *state = &seatalk_transport_state[seatalk_port];

    if (state->transmit_datagram_bytes_remaining == 0) {
        return 0;
    }
    *data_byte = state->transmit_buffer[state->transmit_buffer_position];
    *command_bit = state->transmit_buffer_position == 0;
    return 1;
}

int seatalk_transmit_bit(int seatalk_port)
{
    SEATALK_TRANSPORT_STATE *state = &seatalk_transport_state[seatalk_port];

    int current_bit = state->tx_bit_number;
    if (current_bit == -1) {
        if (state->bus_state == BUS_STATE_WAIT_FOR_IDLE) {
            set_bus_state(seatalk_port, BUS_STATE_IDLE);
        }
        if (!seatalk_can_transmit(seatalk_port)) {
            seatalk_ready_to_send(seatalk_port);
            if (!seatalk_can_transmit(seatalk_port)) {
                return 0;
            }
        }
        if (!seatalk_byte_to_send(seatalk_port, &state->tx_byte, &state->tx_command_bit)) {
            // this should be impossible since we just confirmed data was pending
            return 0;
        }
        set_bus_state(seatalk_port, BUS_STATE_TRANSMITTING);
#ifdef DEBUG
        LOG("character to send: %2x; current RxD value: %d; sending start bit\n", state->tx_byte, get_seatalk_hardware_bit_value(seatalk_port));
#endif
        seatalk_set_hardware_bit_value(seatalk_port, 0); // start bit draws line down to zero
    } else if (current_bit <= 7) {
#ifdef DEBUG
        if ((state->tx_bit_number > 0) && (state->rx_bit_number == -1)) {
            LOG("rx didn't start on time\n");
        }
#endif
        seatalk_set_hardware_bit_value(seatalk_port, (state->tx_byte >> current_bit) & 0x01); // data bit
    } else if (current_bit == 8) {
        seatalk_set_hardware_bit_value(seatalk_port, state->tx_command_bit); // send command bit
    } else {
        state->tx_bit_number = -1; // start new character on next iteration
        seatalk_set_hardware_bit_value(seatalk_port, 1); // set high for stop bit
        return 1; // transmit loop returns to bit zero and stops there if nothing to send
    }
    state->tx_bit_number++;
    return 1;
}

int seatalk_queue_datagram(int seatalk_port, int datagram_length, char *datagram) {
  SEATALK_TRANSPORT_STATE *state;
  int i;

  state = &seatalk_transport_state[seatalk_port];

  if (!seatalk_can_transmit(seatalk_port)) {
    return 0;
  }

  for (i = 0; i < datagram_length; i++) {
    state->transmit_buffer[i] = datagram[i];
  }
  state->transmit_datagram_bytes_remaining = datagram_length;
  state->transmit_buffer_position = 0;
  return datagram_length;
}
