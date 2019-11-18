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

int interrupt_fired = 0;

// interrupt variables
short int test_gpio_rxd_irq = 0;

struct kobject *seatalk_kobj_ref;

static ssize_t sysfs_get_level(struct kobject *kobj, struct kobj_attribute *attr, char *buf);
static ssize_t sysfs_set_level(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count);
struct kobj_attribute level_attribute = __ATTR(level, 0660, sysfs_get_level, sysfs_set_level);
static ssize_t sysfs_get_speed_test(struct kobject *kobj, struct kobj_attribute *attr, char *buf);
struct kobj_attribute speed_test_attribute = __ATTR(speed_test, 0440, sysfs_get_speed_test, NULL);

int rising;
long rise_time = 0;
long fall_time = 0;
int handled = 0;

struct timespec start_time;

long time_delta(void) {
  struct timespec end_time = current_kernel_time();
  long delta_seconds = end_time.tv_sec - start_time.tv_sec, delta_nanos = end_time.tv_nsec - start_time.tv_nsec;
//  pr_info("delta_seconds: %l, delta_nanos: %l\n", delta_seconds, delta_nanos);
  return delta_seconds * 1000000000 + delta_nanos;
}

static irqreturn_t level_irq_handler(int irq, void *dev_id, struct pt_regs *regs) {
  unsigned long flags;
  int delta;

  local_irq_save(flags); // disable hardware interrupts
  delta = time_delta();
  if (rising) {
    rise_time += delta;
  } else {
    fall_time += delta;
  }
  handled = 1;
  local_irq_restore(flags); // restore hardware interrupts
  return IRQ_HANDLED;
}

static ssize_t sysfs_get_level(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
  buf[0] = get_seatalk_hardware_bit_value() ? '1' : '0';
  buf[1] = ',';
  buf[2] = '0' + interrupt_fired;
  buf[3] = 0xa;;
  interrupt_fired = 0;
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
      interrupt_fired = 0;
      value ^= 0x01;
      set_seatalk_hardware_bit_value(value);
      for (j = 0; j < time; j += 10) {
        udelay(10);
      }
      if (interrupt_fired != 1) {
        pr_info("leaving for loop because interrupt fired %d times during udelay()", interrupt_fired);
        break;
      }
    }
    if (interrupt_fired) {
      pr_info("interrupt fired correctly with %d us timing window", time);
      high = time;
      time = (time + low) / 2;
    } else {
      pr_info("interrupt did not fire with %d us timing window", time);
      low = time;
      time = (time + high) / 2;
//    } else {
//      pr_info("interrupt fired %d times with %d us timing window; exiting", interrupt_fired, time);
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
#define TIMING_ITERATIONS 200000
  int i;
  set_seatalk_hardware_bit_value(0);
  rise_time = 0;
  fall_time = 0;
  for (i = 0; i < TIMING_ITERATIONS; i++) {
    handled = 0;
    rising = (i & 0x1) == 0;
    start_time = current_kernel_time();
    set_seatalk_hardware_bit_value(rising);
    while (!handled) {
      udelay(10);
    }
  }
  return sprintf(buf, "After %d iterations rise time (ns): %d (total %d), fall_time: %d (total %d)\n", TIMING_ITERATIONS, ((rise_time * 2) / TIMING_ITERATIONS), rise_time, ((fall_time * 2) / TIMING_ITERATIONS), fall_time);
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
  if (request_irq(test_gpio_rxd_irq, (irq_handler_t) level_irq_handler, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, GPIO_RXD_DESC, GPIO_DEVICE_DESC)) {
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

