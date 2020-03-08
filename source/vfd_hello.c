#define F_CPU 16000000
#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include "atmega_vfd.h"

FILE vfd_out = FDEV_SETUP_STREAM(vfd_putc, NULL, _FDEV_SETUP_WRITE);

int main() {
	// Initialize PORTB and PORTD as outputs
	DDRB = 0x07;
	DDRD = 0xFF;
	// Initialize PORTB and PORTD
	PORTB = 0xF8;
	PORTD = 0x00;
	// Set 8-bit bus mode
	VFD_FUNCTION_SET();
	// Set brightness
	write_rs(0x03);
	// Turn dislay on, cursor off
	write_rs_n(0x0F);

	// Clear display
	VFD_DISPLAY_CLEAR();
	_delay_ms(10);
	VFD_ENTRY_MODE_SET();
	VFD_CURSOR_HOME();

	// Setup output stream
	stdout = &vfd_out;

	uint8_t read_val;

	for (;;) {
		VFD_CURSOR_HOME();
		printf("read value = %0x", read_val);
		read_val = vfd_read_adr();
		_delay_ms(1000);
	};

	return 0;
}
