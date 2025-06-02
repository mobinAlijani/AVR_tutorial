//===========================================================================================
// Project: ATmega32A LED Blink Example        
// Compiler: avr-gcc
// Target microcontroller: ATmega32A
// This code is for an ATmega32A microcontroller
// by [mobin Alijani]
// Date: 2023-10-01

//==========================================================================================
#define F_CPU 1000000UL  // Define CPU frequency as 1 MHz

// Define memory-mapped I/O addresses for PORTB and DDRB
#define PORTB (*(volatile unsigned char *)0x38)  // Data Register for Port B
#define DDRB  (*(volatile unsigned char *)0x37)  // Data Direction Register for Port B

// Software delay function in milliseconds (approximate)
void delay_ms(unsigned int time){
    for(unsigned int i = 0; i < time; i++) {
        for(volatile unsigned int j = 0; j < 50; j++);  // Inner loop creates the delay
    }
}

int main(void){

    DDRB |= (1 << 1);     // Set pin PB1 as output
    PORTB = 0x00;         // Clear all pins on PORTB (initial state: all LOW)

    while(1) {            // Infinite loop
        PORTB ^= (1 << 1);    // Toggle the state of PB1 (HIGH -> LOW or LOW -> HIGH)
        delay_ms(500);        // Delay for ~500 milliseconds
    }

    return 0; // This line will never be reached
}
