#include <linux/kernel.h>
#include <linux/module.h>
#define DEBUG
#include "../../seatalk_hardware_layer.c"
#include "../../../test/hardware/seatalk_hardware_test.h"

struct kobject *seatalk_kobj_ref;

static ssize_t sysfs_set_send(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count);
struct kobj_attribute send_attribute = __ATTR(send, 0220, NULL, sysfs_set_send);
static ssize_t sysfs_get_receive(struct kobject *kobj, struct kobj_attribute *attr, char *buf);
struct kobj_attribute receive_attribute = __ATTR(receive, 0440, sysfs_get_receive, NULL);

static ssize_t sysfs_set_send(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
  return set_bytes_to_send(buf, count);
}

static ssize_t sysfs_get_receive(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
  return get_received_bytes(buf);
}

static int init_sysfs(void) {
  seatalk_kobj_ref = kobject_create_and_add("seatalk", NULL);
  if (sysfs_create_file(seatalk_kobj_ref, &send_attribute.attr)) {
    pr_info("Unable to create sysfs file for send");
    goto clean_send;
  }
  if (sysfs_create_file(seatalk_kobj_ref, &receive_attribute.attr)) {
    pr_info("Unable to create sysfs file for receive");
    goto clean_receive;
  }
  return 0;

clean_receive:
  sysfs_remove_file(seatalk_kobj_ref, &receive_attribute.attr);

clean_send:
  sysfs_remove_file(seatalk_kobj_ref, &send_attribute.attr);
  kobject_put(seatalk_kobj_ref);
  return 1;
}

static void exit_sysfs(void) {
  sysfs_remove_file(seatalk_kobj_ref, &receive_attribute.attr);
  sysfs_remove_file(seatalk_kobj_ref, &send_attribute.attr);
  kobject_put(seatalk_kobj_ref);
}

static int __init init_seatalk_module(void) {
  if (init_sysfs()) {
    pr_info("Unable to initialize sysfs. Exiting.");
    goto cleanup;
  } 
  if (init_seatalk_hardware_signal()) {
    pr_info("Unable to initialize SeaTalk test communications layer. Exiting.");
    goto cleanup;
  }
  if (init_seatalk_hardware_irq()) {
    pr_info("Unable to initialize SeaTalk IRQ. Exiting.");
    goto cleanup_irq;
  }
  pr_info("Seatalk test module initialization complete.");
  return 0;

cleanup_irq:
  exit_seatalk_hardware_signal();

cleanup:
  return -1;
}

void __exit exit_seatalk_module(void) {
  pr_info("Exiting Seatalk test module");
  exit_seatalk_hardware_irq();
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

