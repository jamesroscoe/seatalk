int get_datagram_length(char second_byte);
void parse_seatalk_datagram(char *datagram);

int build_compass_datagram(char *datagram, int compass);
