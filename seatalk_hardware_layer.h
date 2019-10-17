// signal and irq initialization are separate in order to accommodate
// level and bit timing testing during development of hardware interfaces

int init_seatalk_hardware_signal(void);
void exit_seatalk_hardware_signal(void);
int init_seatalk_hardware_irq(void);
void exit_seatalk_hardware_irq(void);

int get_seatalk_hardware_bit_value(void);
void set_seatalk_hardware_bit_value(int bit_value);

void initiate_seatalk_hardware_transmitter(int bit_delay);
