#define F_CPU 16000000
#define VFD_4_BIT
#define VFD_DB4 2
#define VFD_DB5 3
#define VFD_DB6 4
#define VFD_DB7 6
#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include "atmega_vfd.h"

FILE vfd_out = FDEV_SETUP_STREAM(vfd_putc, NULL, _FDEV_SETUP_WRITE);

int main() {
	// Initialize PORTB and PORTD as outputs
	DDRB = 0x07;
	DDRD = VFD_IO_MASK;
	// Initialize PORTB and PORTD
	PORTB = 0x00;
	PORTD = 0x00;
	// Set 8-bit bus mode
	_delay_ms(50);
	VFD_FUNCTION_SET();
	// Set brightness
	VFD_BRIGHT_25();
	// Turn dislay on, cursor off
	VFD_DISPLAY_SET(VFD_DISPLAY_ON, VFD_CURSOR_OFF, VFD_BLINK_OFF);

	// Clear display
	VFD_DISPLAY_CLEAR();
	_delay_ms(10);
	VFD_ENTRY_MODE_SET();
	VFD_CURSOR_HOME();

	// Setup output stream
	stdout = &vfd_out;

	// uint8_t read_val;

	printf("hello");

	for (;;) {};

	return 0;
}
