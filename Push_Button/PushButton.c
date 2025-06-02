//===========================================================================================
// Project: ATmega32A Push Button Example
// Description: This code demonstrates how to read a push button connected to Port D and control
// an output on Port B based on the button state. When the button is pressed, all pins on Port B
// will be set HIGH, and when the button is not pressed, all pins on Port B will be set LOW.
// Compiler: avr-gcc
// Target microcontroller: ATmega32A
// This code is for an ATmega32A microcontroller
// by [mobin Alijani]
// Date: 2023-10-01

//==========================================================================================


#define F_CPU 1000000UL  // Define CPU frequency as 1 MHz

// Define memory-mapped I/O registers
#define PORTB (*(volatile unsigned char *)0x38)  // Data Register for Port B
#define DDRB  (*(volatile unsigned char *)0x37)  // Data Direction Register for Port B

#define PIND  (*(volatile unsigned char *)0x30)  // Input Pins Register for Port D
#define DDRD  (*(volatile unsigned char *)0x31)  // Data Direction Register for Port D

int main(void) {

    DDRB = 0xFF;   // Configure all PORTB pins as output
    DDRD = 0x00;   // Configure all PORTD pins as input
    PORTB = 0x00;  // Initialize PORTB to LOW (all outputs off)

    while (1) {
        // Check if PD7 (pin 7 of Port D) is HIGH (e.g., button pressed)
        if (PIND & (1 << 7)) {
            PORTB = 0xFF;  // Set all PORTB pins HIGH (turn on all connected outputs)
        } else {
            PORTB = 0x00;  // Set all PORTB pins LOW (turn off all outputs)
        }        
    }

    return 0; // This line is never reached
}

// Note: Assumes button is on PD7 and goes HIGH when pressed.
// Ensure button wiring and Vcc/GND are correct.
// Loop checks button state and updates PORTB accordingly.
// 'volatile' ensures accurate access to memory-mapped registers.
