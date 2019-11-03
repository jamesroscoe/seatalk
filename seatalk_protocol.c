int build_command_datagram(char *datagram) {
//  switch (pending_command) {
//  }
  return 0;
}

int seatalk_sensor_pending(void) {
//  return VALID_SENSOR(compass);
  return 0;
}

int build_sensor_datagram(char *datagram) {
//  if (VALID_SENSOR(compass)) {
//    return build_heading(datagram, SENSOR_VARIABLE(compass), 0, 0);
//  } else {
    return 0;
//  }
}

int get_pending_datagram(char *datagram) {
  int length;
  if (length = build_command_datagram(datagram)) {
    return length;
  } else if (length = build_sensor_datagram(datagram)) {
    return length;
  } else {
    return 0;
  }
}

void update_seatalk_state(char *datagram) {
//  printf("update_seatalk_state\n");
//  int i;
//  pr_info("update_seatalk_state: ");
//  for (i = 0; i < (datagram[1] & 0x0f) + 3; i++) {
//    pr_info("%x ", datagram[i]);
//  }
//  pr_info("\n");
  handle_seatalk_datagram(datagram);
//  state_updated();
}
