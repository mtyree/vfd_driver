// VFD_DATA[7:0] = DB[7:0]
// VFD_CTRL[7:0] = {X, X, X, X, X, E, RW, RS}
// 1/16MHz = 62.5ns
// 1st row DDRAM addresses = 0x80 to 0xA7
// 2nd row DDRAM addresses = 0xC0 to 0xE7

#ifndef VFD_CTRL
#define VFD_CTRL PORTB
#endif

#ifndef VFD_DATA
#define VFD_DATA PORTD
#endif

#ifndef VFD_RS
#define VFD_RS	0
#endif

#ifndef VFD_RW
#define VFD_RW	1
#endif

#ifndef VFD_E
#define VFD_E	2
#endif

#define VFD_L1_START	0x80
#define VFD_L1_END		0xA7
#define VFD_L2_START	0xC0
#define VFD_L2_END		0xE7

#define VFD_DISPLAY_CLEAR()		write_rs_n(0x01)
#define VFD_ENTRY_MODE_SET()	write_rs_n(0x06)
#define VFD_CURSOR_HOME()		write_rs_n(0x03)

#ifndef VFD_4_BIT
#define VFD_8_BIT
#endif

#ifndef VFD_DB7
#define VFD_DB7	7
#endif

#ifndef VFD_DB6
#define VFD_DB6	6
#endif

#ifndef VFD_DB5
#define VFD_DB5 5
#endif

#ifndef VFD_DB4
#define VFD_DB4	4
#endif

#ifdef VFD_8_BIT
#define VFD_FUNCTION_SET()		write_rs_n(0x3F)
#endif

#define VFD_LNIB(byte)			byte & 0x0F
#define VFD_HNIB(byte)			(byte & 0xF0) >> 4
#define VFD_BUS_SET(nib)		((nib&_BV(0))<<VFD_DB4) | ((nib&_BV(1))<<(VFD_DB5-1)) | ((nib&_BV(2))<<(VFD_DB6-2)) | ((nib&_BV(3))<<(VFD_DB7-3))
#define VFD_IO_MASK				VFD_BUS_SET(0x0F)
#ifdef VFD_4_BIT
#define VFD_FUNCTION_SET()		write_rs_n(0x20)
#endif

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

#ifdef VFD_4_BIT
void vfd_write(uint8_t data) {
	uint8_t nib = VFD_BUS_SET(data);
	DDRD = VFD_IO_MASK;
	VFD_CTRL &= ~(_BV(VFD_RW));  	// E = 0, RW = 0
	VFD_DATA = nib;
	_delay_loop_1(2);
	VFD_CTRL |= _BV(VFD_E);   		// E = 1, RW = 0
	_delay_loop_1(2);
	VFD_CTRL &= ~(_BV(VFD_E));  	// E = 0, RW = 0
}
#else
void vfd_write(uint8_t data) {
	DDRD = VFD_IO_MASK;
	VFD_CTRL &= ~(_BV(VFD_RW));  	// E = 0, RW = 0
	VFD_DATA = data;
	_delay_loop_1(2);
	VFD_CTRL |= _BV(VFD_E);   		// E = 1, RW = 0
	_delay_loop_1(2);
	VFD_CTRL &= ~(_BV(VFD_E));  	// E = 0, RW = 0
}
#endif

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

#ifdef VFD_4_BIT
void write_rs(uint8_t data) {
	VFD_CTRL |= _BV(VFD_RS);
	vfd_write(VFD_HNIB(data));
	vfd_write(VFD_LNIB(data));
}
void write_rs_n(uint8_t data) {
	VFD_CTRL &= ~(_BV(VFD_RS));
	vfd_write(VFD_HNIB(data));
	vfd_write(VFD_LNIB(data));
}
#else
void write_rs(uint8_t data) {
	VFD_CTRL |= _BV(VFD_RS);
	vfd_write(data);
}
void write_rs_n(uint8_t data) {
	VFD_CTRL &= ~(_BV(VFD_RS));
	vfd_write(data);
}
#endif

int vfd_putc(uint8_t c, FILE *stream) {
	switch(c) {
		case 0x5C : // Convert ASCII '/' to VFD supported code
			write_rs(0x8C);
			break;
		// Add more substitutions as needed
		default :
			write_rs(c);
	}
	return 0;
}
