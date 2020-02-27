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

#define START_BIT_DIRECTION IRQF_TRIGGER_RISING
#define GPIO_RX_LOW_VALUE 1
#define GPIO_RX_HIGH_VALUE 0
#define GPIO_TX_LOW_VALUE 1
#define GPIO_TX_HIGH_VALUE 0

// interrupt variables
short int gpio_rxd_irq = 0;

// transmit and receive
// bit timer period; 1000000000 ns/s / 4800 bits/s = 208333 ns/bit
#define BIT_INTERVAL 208333
// start receive timer 1/4 bit after triggering edge of start bit
#define START_BIT_DELAY (BIT_INTERVAL / 4)
#define DEBOUNCE_NANOS 60000

// receive data
static struct hrtimer hrtimer_rxd;
static enum hrtimer_restart receive_bit(struct hrtimer *timer);
int debouncing = 0;

// transmit data
static struct hrtimer hrtimer_txd;
static enum hrtimer_restart transmit_bit(struct hrtimer *timer);

static irqreturn_t rxd_irq_handler(int irq, void *dev_id, struct pt_regs *regs) {
  unsigned long flags;

  // disable hardware interrupts to prevent re-entry into this IRQ handler
  local_irq_save(flags);
  // debounce the state transition by ignoring IRQs for DEBOUNCE_NANOS nanoseconds after each "real" one
  if (debouncing) {
    pr_info("debouncing\n");
  } else {
    // make sure the receiver is not already active
    if (initiate_seatalk_receive_character()) {
      // reset debounce timer
      // schedule receive event for first bit
      hrtimer_start(&hrtimer_rxd, ktime_set(0, BIT_INTERVAL + START_BIT_DELAY), HRTIMER_MODE_REL);
    }
  }
  // restore hardware interrupts
  local_irq_restore(flags);
  // let OS know this IRQ has been handled successfully
  return IRQ_HANDLED;
}

int get_seatalk_hardware_bit_value(void) {
  return gpio_get_value(GPIO_RXD_PIN) ? GPIO_RX_HIGH_VALUE : GPIO_RX_LOW_VALUE; // normal sense
}

void set_seatalk_hardware_bit_value(int bit_value) {
  gpio_set_value(GPIO_TXD_PIN, (bit_value == GPIO_TX_HIGH_VALUE) ? 1 : 0); // normal sense
//#ifdef DEBUG
//  pr_info("set GPIO_TXD_PIN to %d\n", bit_value);
//#endif
}

static enum hrtimer_restart receive_bit(struct hrtimer *timer) {
  // after a character has been received there is a rising-edge stop bit with a lot
  // of signal bounce. Wait DEBOUNCE_NANOS after the stop bit timing to ignore bounces
  if (debouncing) {
    // debounce period has expired. Sleep the receiver until the next start bit detected
    debouncing = 0;
    return HRTIMER_NORESTART;
  } else {
    // calculate the wake-up time for the next bit (if any)
    // (done now to limit time lag on very slow machines)
    // reeive a single bit. Return value indicates whether more bits are expected
    if (seatalk_receive_bit()) {
      // more bits are expected. Restart the timer
      hrtimer_forward_now(&hrtimer_rxd, ktime_set(0, BIT_INTERVAL));
      return HRTIMER_RESTART;
    } else {
      // no more bits are expected. Start debounce timer
      hrtimer_forward_now(&hrtimer_rxd, ktime_set(0, DEBOUNCE_NANOS));
      debouncing = 1;
      return HRTIMER_RESTART;
    }
  }
}

static enum hrtimer_restart transmit_bit(struct hrtimer *timer) {
  // calculate the wake-up time for the next bit (if any)
  // (done now to limit time lag on very slow machines)
  hrtimer_forward_now(&hrtimer_txd, ktime_set(0, BIT_INTERVAL));
  // send single bit. Return value indicates whether there are more bits to send
  if (seatalk_transmit_bit() == 0) {
    // no more bits to send. Don't restart. Transmission will wake when more data enqueued
    return HRTIMER_NORESTART;
  } else {
    // more bits to send. Restart the timer
    return HRTIMER_RESTART;
  }
}

void initiate_seatalk_hardware_transmitter(int bit_delay) {
  // reawaken the transmittter. Wait a 10-bit duration as guard time after last char
  // that has been on the bus
  // first stop pending timer (if any)
  hrtimer_cancel(&hrtimer_txd);
  // schedule new timer after delay period
  hrtimer_start(&hrtimer_txd, ktime_set(0, BIT_INTERVAL * bit_delay), HRTIMER_MODE_REL);
}

int init_seatalk_hardware_signal(void) {
  // initialize rx
  // reserve GPIO_RXD_PIN (default GPIO 23)
  if (gpio_request(GPIO_RXD_PIN, GPIO_RXD_DESC)) {
    pr_info("Unable to request GPIO RxD pin %d", GPIO_RXD_PIN);
    goto cleanup;
  }
  // set pin direction to input
  gpio_direction_input(GPIO_RXD_PIN);
  // initialize the receive timer but don't start it
  hrtimer_init(&hrtimer_rxd, CLOCK_REALTIME, HRTIMER_MODE_REL);
  hrtimer_rxd.function = receive_bit;

  // initialize tx
  // reserve GPIO_TXD_PIN (default GPIO 24)
  if (gpio_request(GPIO_TXD_PIN, GPIO_TXD_DESC)) {
    pr_info("Unable to request GPIO TxD pin %d", GPIO_TXD_PIN);
    goto cleanup_rx;
  }
  // set pin direction to output
  gpio_direction_output(GPIO_TXD_PIN, 1);
  // set at-rest pin value to high
  set_seatalk_hardware_bit_value(1);
  // initialize the transmit timer bit don't start it
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
  // get IRQ number for input pin
  if ((gpio_rxd_irq = gpio_to_irq(GPIO_RXD_PIN)) < 0) {
    pr_info("Unable to map GPIO pin to irq");
    goto cleanup;
  }
  // set up interrupt vector for START_BIT_DIRECTION (falling edge if using normal sense)
  if (request_irq(gpio_rxd_irq, (irq_handler_t) rxd_irq_handler, START_BIT_DIRECTION, GPIO_RXD_DESC, GPIO_DEVICE_DESC)) {
    pr_info("Unable to request IRQ %d", gpio_rxd_irq);
    goto cleanup;
  }
  pr_info("Hooked falling-edge IRQ %d for GPIO pin %d", gpio_rxd_irq, GPIO_RXD_PIN);
  // initialize debounce timer
  return 0;

cleanup:
  gpio_free(GPIO_TXD_PIN);
  gpio_free(GPIO_RXD_PIN);
  return -1;
}

void exit_seatalk_hardware_signal(void) {
  // release RxD and TxD pins
  gpio_free(GPIO_TXD_PIN);
  gpio_free(GPIO_RXD_PIN);
  // cancel timers
  hrtimer_cancel(&hrtimer_rxd);
  hrtimer_cancel(&hrtimer_txd);
  return;
}

void exit_seatalk_hardware_irq(void) {
  // release IRQ
  free_irq(gpio_rxd_irq, GPIO_DEVICE_DESC);
}
