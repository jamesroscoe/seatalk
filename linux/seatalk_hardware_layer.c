#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/hrtimer.h>
#include <linux/gpio.h>
#include "../seatalk_hardware_layer.h"
#include "../seatalk_transport_layer.h"

#define GPIO_RXD_PIN 23
#define GPIO_TXD_PIN 24
#define GPIO_RXD_DESC "Seatalk RxD pin"
#define GPIO_TXD_DESC "Seatalk TxD pin"
#define GPIO_DEVICE_DESC "Seatalk communications driver"

#define START_BIT_DIRECTION IRQF_TRIGGER_FALLING
#define GPIO_HIGH_VALUE 1
#define GPIO_LOW_VALUE 0

// interrupt variables
short int gpio_rxd_irq = 0;

// transmit and receive
// bit timer period; 1000000000 ns/s / 4800 bits/s = 208333 ns/bit
#define BIT_INTERVAL 208333
// start receive timer 3/4 bit after triggering edge of start bit
#define START_BIT_DELAY (BIT_INTERVAL / 4)

// receive data
static struct hrtimer hrtimer_rxd;
static enum hrtimer_restart receive_bit(struct hrtimer *timer);

// transmit data
static struct hrtimer hrtimer_txd;
static enum hrtimer_restart transmit_bit(struct hrtimer *timer);

static irqreturn_t rxd_irq_handler(int irq, void *dev_id, struct pt_regs *regs) {
  unsigned long flags;

  local_irq_save(flags); // disable hardware interrupts
  if (initiate_seatalk_receive_character()) {
    hrtimer_start(&hrtimer_rxd, ktime_set(0, BIT_INTERVAL + START_BIT_DELAY), HRTIMER_MODE_REL);
  }
  local_irq_restore(flags); // restore hardware interrupts
  return IRQ_HANDLED;
}

int get_seatalk_hardware_bit_value(void) {
  return gpio_get_value(GPIO_RXD_PIN) ? GPIO_HIGH_VALUE : GPIO_LOW_VALUE; // normal sense
}

void set_seatalk_hardware_bit_value(int bit_value) {
  gpio_set_value(GPIO_TXD_PIN, (bit_value == GPIO_HIGH_VALUE) ? 1 : 0); // normal sense
//#ifdef DEBUG
//  pr_info("set GPIO_TXD_PIN to %d\n", bit_value);
//#endif
}

static enum hrtimer_restart receive_bit(struct hrtimer *timer) {
  int more_bits_expected;
  hrtimer_forward_now(&hrtimer_rxd, ktime_set(0, BIT_INTERVAL));
  more_bits_expected = seatalk_receive_bit();
  if (more_bits_expected) {
    return HRTIMER_RESTART;
  } else {
    return HRTIMER_NORESTART;
  }
}

static enum hrtimer_restart transmit_bit(struct hrtimer *timer) {
  if (seatalk_transmit_bit() == 0) {
    return HRTIMER_NORESTART;
  } else {
    hrtimer_forward_now(&hrtimer_txd, ktime_set(0, BIT_INTERVAL));
    return HRTIMER_RESTART; // restart even at end of stop bit -- seatalk_can_transmit() takes care of whether there is more to send
  }
}

void initiate_seatalk_hardware_transmitter(int bit_delay) {
#ifdef DEBUG
  pr_info("initiate_seatalk_hardware_transmitter(%d)", bit_delay);
#endif
  hrtimer_cancel(&hrtimer_txd);
  hrtimer_start(&hrtimer_txd, ktime_set(0, BIT_INTERVAL * bit_delay), HRTIMER_MODE_REL);
}

int init_seatalk_hardware_signal(void) {
  // initialize rx
  if (gpio_request(GPIO_RXD_PIN, GPIO_RXD_DESC)) {
    pr_info("Unable to request GPIO RxD pin %d", GPIO_RXD_PIN);
    goto cleanup;
  }
  gpio_direction_input(GPIO_RXD_PIN);
  hrtimer_init(&hrtimer_rxd, CLOCK_REALTIME, HRTIMER_MODE_REL);
  hrtimer_rxd.function = receive_bit;

  // initialize tx
  if (gpio_request(GPIO_TXD_PIN, GPIO_TXD_DESC)) {
    pr_info("Unable to request GPIO TxD pin %d", GPIO_TXD_PIN);
    goto cleanup_rx;
  }
  gpio_direction_output(GPIO_TXD_PIN, 1);
  hrtimer_init(&hrtimer_txd, CLOCK_REALTIME, HRTIMER_MODE_REL);
  hrtimer_txd.function = transmit_bit;

  return 0;

cleanup_rx:
  gpio_free(GPIO_RXD_PIN);
cleanup:
  return -1;
}

int init_seatalk_hardware_irq(void) {
  // hook irq for RxD GPIO pin
  if ((gpio_rxd_irq = gpio_to_irq(GPIO_RXD_PIN)) < 0) {
    pr_info("Unable to map GPIO pin to irq");
    goto cleanup;
  }
  if (request_irq(gpio_rxd_irq, (irq_handler_t) rxd_irq_handler, START_BIT_DIRECTION, GPIO_RXD_DESC, GPIO_DEVICE_DESC)) {
    pr_info("Unable to request IRQ %d", gpio_rxd_irq);
    goto cleanup;
  }
  pr_info("Hooked falling-edge IRQ %d for GPIO pin %d", gpio_rxd_irq, GPIO_RXD_PIN);
  return 0;

cleanup:
  gpio_free(GPIO_TXD_PIN);
  gpio_free(GPIO_RXD_PIN);
  return -1;
}

void exit_seatalk_hardware_signal(void) {
  gpio_free(GPIO_TXD_PIN);
  gpio_free(GPIO_RXD_PIN);
  // cancel timers
  hrtimer_cancel(&hrtimer_rxd);
  hrtimer_cancel(&hrtimer_txd);
  return;
}

void exit_seatalk_hardware_irq(void) {
  // release irq
  free_irq(gpio_rxd_irq, GPIO_DEVICE_DESC);
}

