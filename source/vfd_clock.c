#define F_CPU 16000000
#define VFD_4_BIT
#define ONE_SECOND 54687
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "atmega_vfd.h"

FILE vfd_out = FDEV_SETUP_STREAM(vfd_putc, NULL, _FDEV_SETUP_WRITE);
volatile uint32_t sys_seconds;

ISR(TIMER1_COMPA_vect) {
	sys_seconds = sys_seconds + 1;
	printf("Elapsed Seconds");
	VFD_WRITE_ADR(VFD_L2_START);
	printf("= %d", sys_seconds);
}

void init() {
	// Initialize PORTB and PORTD as outputs
	DDRB = 0x07;
	DDRD = 0xFF;
	// Initialize PORTB and PORTD
	PORTB = 0xF8;
	PORTD = 0x00;
	// Initialize counter
	sys_seconds = 0;
	// Timer setup
	GTCCR |= 0x83;
	TCNT1 = 0x0000;
	TIMSK1 |= 0x02;
	OCR1A = ONE_SECOND;
	TCCR1B |= _BV(CS12);
	TCCR1B &= ~(_BV(CS11));
	TCCR1B &= ~(_BV(CS10));
	GTCCR &= ~(1 << TSM);
	// Set 8-bit bus mode
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
}

int main() {
	init();

	sei();
	for (;;) {};

	return 0;
}
