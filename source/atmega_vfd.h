// VFD_DATA[7:0] = DB[7:0]
// VFD_CTRL[7:0] = {X, X, X, X, X, E, RW, RS}
// 1/16MHz = 62.5ns
// 1st row DDRAM addresses = 0x80 to 0xA7
// 2nd row DDRAM addresses = 0xC0 to 0xE7

#define VFD_CTRL PORTB
#define VFD_DATA PORTD

#define VFD_RS	0
#define VFD_RW	1
#define VFD_E	2

#define VFD_L1_START	0x80
#define VFD_L1_END		0xA7
#define VFD_L2_START	0xC0
#define VFD_L2_END		0xE7

#define VFD_DISPLAY_CLEAR()		write_rs_n(0x01)
#define VFD_ENTRY_MODE_SET()	write_rs_n(0x06)
#define VFD_CURSOR_HOME()		write_rs_n(0x03)
#define VFD_FUNCTION_SET()		write_rs_n(0x3F)
// Brightness setting functions, must be executed after FUNCTION_SET command
#define VFD_BRIGHT_25()			write_rs(0x03)
#define VFD_BRIGHT_50()			write_rs(0x02)
#define VFD_BRIGHT_75()			write_rs(0x01)
#define VFD_BRIGHT_100()		write_rs(0x00)
// Various display settings
#define VFD_DISPLAY_ON	0x04
#define VFD_CURSOR_ON	0x02
#define VFD_BLINK_ON	0x01
#define VFD_DISPLAY_OFF	0x00
#define VFD_CURSOR_OFF	0x00
#define VFD_BLINK_OFF	0x00
#define VFD_DISPLAY_SET(display_set, cursor_set, blink_set)	write_rs_n(0x08 + display_set + cursor_set + blink_set)

#define VFD_WAIT()				do {} while (vfd_read_adr() >> 7 == 0x01)
#define VFD_SHIFT_RIGHT()		write_rs_n(0x1C)
#define VFD_WRITE_ADR(addr)		write_rs_n(addr)
#define VFD_WRTE_DATA(data)		write_rs(data)

void vfd_write(uint8_t data) {
	DDRD = 0xFF;
	VFD_CTRL &= ~(_BV(VFD_RW));  	// E = 0, RW = 0
	VFD_DATA = data;
	_delay_loop_1(2);
	VFD_CTRL |= _BV(VFD_E);   		// E = 1, RW = 0
	_delay_loop_1(2);
	VFD_CTRL &= ~(_BV(VFD_E));  	// E = 0, RW = 0
}

uint8_t vfd_read_adr() {
	VFD_DATA = 0x00;
	VFD_CTRL &= ~(_BV(VFD_RS));
	DDRD = 0x00;
	VFD_CTRL |= _BV(VFD_RW);		// E = 0, RW = 1
	_delay_loop_1(2);
	VFD_CTRL |= _BV(VFD_E);			// E = 1, RW = 1
	_delay_loop_1(2);
	VFD_CTRL &= ~(_BV(VFD_E));		// E = 0, RW = 1
	return PIND;
}

void write_rs(uint8_t data) {
	VFD_CTRL |= _BV(VFD_RS);
	vfd_write(data);
}

void write_rs_n(uint8_t data) {
	VFD_CTRL &= ~(_BV(VFD_RS));
	vfd_write(data);
}

int vfd_putc(uint8_t c, FILE *stream) {
	VFD_CTRL |= _BV(VFD_RS);
	switch(c) {
		case 0x5C : // Convert ASCII '/' to VFD supported code
			vfd_write(0x8C);
			break;
		// Add more substitutions as needed
		default :
			vfd_write(c);
	}
	return 0;
}
