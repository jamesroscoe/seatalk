#include <linux/kernel.h>
#include "seatalk_hardware_test.h"
#include "../../seatalk_transport_layer.h"

char tx_buffer[256];
int tx_buffer_head, tx_buffer_tail = 0;

char rx_buffer[256];
int rx_buffer_head, rx_buffer_tail = 0;

int get_datagram_length(char second_byte) {
  return second_byte & 0x0f;
}

int seatalk_command_pending(void) {
  return tx_buffer_head != tx_buffer_tail;
}

int seatalk_sensor_pending(void) {
   return 0;
}

void handle_seatalk_datagram(char *datagram) {
  int i;
  pr_info("update_seatalk_state");
  for (i = 0; i < (get_datagram_length(datagram[1]) + 3); i++) {
    rx_buffer[rx_buffer_head] = datagram[i];
    rx_buffer_head = (rx_buffer_head + 1) & 0xff;
  }
  pr_info("rx_buffer_head: %d rx_buffer_tail: %d", rx_buffer_head, rx_buffer_tail);
}

int get_pending_datagram(char *datagram) {
  int i = 2;
  datagram[0] = 0xff;
  while (tx_buffer_tail != tx_buffer_head) {
    pr_info("adding %c (%2x) to output datagram", tx_buffer[i], tx_buffer[i]);
    datagram[i++] = tx_buffer[tx_buffer_tail];
    tx_buffer_tail = (tx_buffer_tail + 1) & 0xff;
  }
  datagram[1] = i-3;
  return i;
}

int set_bytes_to_send(const char *buf, int count) {
  int i;
  pr_info("send: %s", buf);
  for (i = 0; i < count; i++) {
    pr_info("adding %c (%2x) to tx_buffer", buf[i], buf[i]);
    tx_buffer[tx_buffer_head] = buf[i];
    tx_buffer_head = (tx_buffer_head + 1) & 0xff;
  }
  initiate_seatalk_transmitter();
  return count;
}

int get_received_bytes(char *buf) {
  int count = 0;
  while (1) {
    if (rx_buffer_tail == rx_buffer_head) {
      break;
    }
    buf[count++] = rx_buffer[rx_buffer_tail];
    rx_buffer_tail = (rx_buffer_tail + 1) & 0xff;
  }
  rx_buffer[count] = 0;
  pr_info("receive %d chars: %s", count, buf);
  return count;
}
