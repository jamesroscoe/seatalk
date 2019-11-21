#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include "../../seatalk_hardware_layer.h"

#define MAX_TIME 1000000
#define GPIO_RXD_PIN 23
#define GPIO_RXD_DESC "Seatalk RxD pin"
#define GPIO_TXD_DESC "Seatalk TxD pin"
#define GPIO_DEVICE_DESC "Seatalk communications driver"

#define DEBOUNCE_PERIOD 60000
#define TIMING_ITERATIONS 100000

// interrupt variables
short int test_gpio_rxd_irq = 0;

struct kobject *seatalk_kobj_ref;

static ssize_t sysfs_get_level(struct kobject *kobj, struct kobj_attribute *attr, char *buf);
static ssize_t sysfs_set_level(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count);
struct kobj_attribute level_attribute = __ATTR(level, 0660, sysfs_get_level, sysfs_set_level);
static ssize_t sysfs_get_speed_test(struct kobject *kobj, struct kobj_attribute *attr, char *buf);
struct kobj_attribute speed_test_attribute = __ATTR(speed_test, 0440, sysfs_get_speed_test, NULL);

int rising;
int last_value;
unsigned long irq_rise_time = 0;
unsigned long irq_fall_time = 0;
int handled = 0;
int rising_irq_count = 0;
int rising_bounce_count = 0;
int falling_irq_count = 0;
int falling_bounce_count = 0;

long poll_rise_time = 0;
long poll_fall_time = 0;

struct timespec start_time;
struct timespec last_interrupt_time;

unsigned long time_delta(struct timespec reference_time) {
  struct timespec end_time;
  unsigned long delta_seconds, delta_nanos;
  getrawmonotonic(&end_time);
  delta_seconds = end_time.tv_sec - reference_time.tv_sec;
  delta_nanos = end_time.tv_nsec - reference_time.tv_nsec;
  return delta_seconds * 1000000000 + delta_nanos;
}

int debounced(struct timespec reference_time) {
  return (time_delta(reference_time) >= DEBOUNCE_PERIOD);
}

static irqreturn_t transition_irq_handler(int irq, void *dev_id, struct pt_regs *regs) {
  unsigned long flags;
  unsigned long delta;

  local_irq_save(flags);
  last_value = !rising;
  if (debounced(last_interrupt_time)) {
    delta = time_delta(start_time);
    if (last_value) {
      falling_irq_count += 1;
      if (!handled) {
        irq_fall_time += delta;
      }
    } else {
      rising_irq_count += 1;
      if (!handled) {
        irq_rise_time += delta;
      }
    }
    getrawmonotonic(&last_interrupt_time);
    handled = 1;
  } else {
    if (last_value) {
      falling_bounce_count += 1;
    } else {
      rising_bounce_count += 1;
    }
  }
  local_irq_restore(flags);
  return IRQ_HANDLED;
}

static ssize_t sysfs_get_level(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
  buf[0] = get_seatalk_hardware_bit_value() ? '1' : '0';
  buf[1] = ',';
  buf[2] = '0' + rising_irq_count + falling_irq_count;
  buf[3] = 0xa;;
  rising_irq_count = 0;
  falling_irq_count = 0;
  return 4;
}

static ssize_t sysfs_set_level(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
  pr_info("write %d chars to level: %s", count, buf);
  if (count >= 1) {
    set_seatalk_hardware_bit_value(buf[0] == '1' ? 1 : 0);
  }
  return count;
}

static ssize_t sysfs_get_speed_test(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
/*
  int high = MAX_TIME;
  int low = 0;
  int time = high;
  int i, j;
  int value = get_seatalk_hardware_bit_value();;
  while (1) {
    pr_info("testing at %d us", time);
    for (i = 0; i < MAX_TIME; i += time) {
      irq_count = 0;
      value ^= 0x01;
      set_seatalk_hardware_bit_value(value);
      for (j = 0; j < time; j += 10) {
        udelay(10);
      }
      if (irq_count != 1) {
        pr_info("leaving for loop because interrupt fired %d times during udelay()", irq_count);
        break;
      }
    }
    if (irq_count) {
      pr_info("interrupt fired correctly with %d us timing window", time);
      high = time;
      time = (time + low) / 2;
    } else {
      pr_info("interrupt did not fire with %d us timing window", time);
      low = time;
      time = (time + high) / 2;
//    } else {
//      pr_info("interrupt fired %d times with %d us timing window; exiting", irq_count, time);
//      break;
    }
    if ((high - low) <= 50) {
      pr_info("speed test complete; minimum reliable propagation time: %d", high);
      break;
    }
  }
  set_seatalk_hardware_bit_value(1);
  return sprintf(buf, "%d\n", high);
*/
  int i, switched, delta;
  set_seatalk_hardware_bit_value(0);
  irq_rise_time = 0;
  irq_fall_time = 0;
  poll_rise_time = 0;
  poll_fall_time = 0;
  rising_irq_count = 0;
  rising_bounce_count = 0;
  falling_irq_count = 0;
  falling_bounce_count = 0;
  getrawmonotonic(&last_interrupt_time);
  for (i = 0; i < TIMING_ITERATIONS; i++) {
    while (!debounced(last_interrupt_time)) {
      udelay(1);
    }
    handled = 0;
    switched = 0;
    rising = (i & 0x1) == 0;
    getrawmonotonic(&start_time);
    set_seatalk_hardware_bit_value(rising);
    while (!switched) {
      last_value = get_seatalk_hardware_bit_value();
      if ((switched = (get_seatalk_hardware_bit_value() == rising))) {
        delta = time_delta(start_time);
        if (rising) {
          poll_rise_time += delta;
        } else {
          poll_fall_time += delta;
        }
      } else {
        udelay(1);
      }
    }
    if (!handled) {
      pr_info("switched state without triggering interrupt\n");
    }
  }
  return sprintf(buf, "After %d iterations:\nrise irq count: %d, bounces: %d, time (ns): %d (total %d), fall irq_count: %d, bounces: %d, time: %d (total %d)\nPolling rise time (ns): %d (total %d), fall time: %d (total %d)\n", TIMING_ITERATIONS, rising_irq_count, rising_bounce_count, ((irq_rise_time * 2) / TIMING_ITERATIONS), irq_rise_time, falling_irq_count, falling_bounce_count, ((irq_fall_time * 2) / TIMING_ITERATIONS), irq_fall_time, ((poll_rise_time * 2) / TIMING_ITERATIONS), poll_rise_time, ((poll_fall_time * 2) / TIMING_ITERATIONS), poll_fall_time);
}

static int init_sysfs(void) {
  seatalk_kobj_ref = kobject_create_and_add("seatalk", NULL);
  if (sysfs_create_file(seatalk_kobj_ref, &level_attribute.attr)) {
    pr_info("Unable to create sysfs file for levels");
    goto clean_level;
  }
  if (sysfs_create_file(seatalk_kobj_ref, &speed_test_attribute.attr)) {
    pr_info("Unable to create sysfs file for speed_test");
    goto clean_speed_test;
  }
  return 0;

clean_speed_test:
  sysfs_remove_file(seatalk_kobj_ref, &speed_test_attribute.attr);

clean_level:
  sysfs_remove_file(seatalk_kobj_ref, &level_attribute.attr);
  kobject_put(seatalk_kobj_ref);
  return 1;
}

static void exit_sysfs(void) {
  sysfs_remove_file(seatalk_kobj_ref, &speed_test_attribute.attr);
  sysfs_remove_file(seatalk_kobj_ref, &level_attribute.attr);
  kobject_put(seatalk_kobj_ref);
}

int seatalk_receive_bit(void) { return 0; }
int initiate_seatalk_receive_character(void) { return 0; }
int seatalk_transmit_bit(void) { return 0; }

static int __init init_seatalk_module(void) {
  if (init_sysfs()) {
    pr_info("Unable to initialize sysfs. Exiting.");
    goto cleanup;
  } 
  if (init_seatalk_hardware_signal()) {
    pr_info("Unable to initialize SeaTalk signal. Exiting.");
    goto cleanup_signal;
  }
  pr_info("initialized SeaTalk sysfs");
  if ((test_gpio_rxd_irq = gpio_to_irq(GPIO_RXD_PIN)) < 0) {
    pr_info("Unable to map GPIO pin to irq");
    goto cleanup_signal;
  }
  pr_info("requesting IRQ %d", test_gpio_rxd_irq);
  if (request_irq(test_gpio_rxd_irq, (irq_handler_t) transition_irq_handler, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, GPIO_RXD_DESC, GPIO_DEVICE_DESC)) {
    pr_info("Unable to request IRQ %d", test_gpio_rxd_irq);
    goto cleanup_irq;
  }
  pr_info("Hooked IRQ %d for GPIO pin %d", test_gpio_rxd_irq, GPIO_RXD_PIN);
  return 0;

cleanup_irq:
  exit_seatalk_hardware_signal();

cleanup_signal:
  exit_sysfs();

cleanup:
  return -1;
}

void __exit exit_seatalk_module(void) {
  pr_info("Exiting Seatalk test module");
  free_irq(test_gpio_rxd_irq, GPIO_DEVICE_DESC);
  exit_seatalk_hardware_signal();
  exit_sysfs();
  pr_info("Seatalk test module cleanup complete.");
}

module_init(init_seatalk_module);
module_exit(exit_seatalk_module);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("James Roscoe <james.roscoe@me.com>");
MODULE_DESCRIPTION("Open-source SeaTalk (Raymarine trademark) tester.");
MODULE_VERSION("1.0");

