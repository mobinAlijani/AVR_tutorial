//===========================================================================================
// Project: ATmega32A Timer0 Interrupt Example          
// Compiler: avr-gcc
// Target microcontroller: ATmega32A
// This code is for an ATmega32A microcontroller
// by [mobin Alijani]
// Date: 2023-10-01
//============================================TIMER0========================================

//TCCR0- timer/counter control register
//    7      6      5        4       3       2       1      0
//  FOC0 | WGM00 | WGM01 | COM00 | COM01 | CS02  | CS01  | CS00 |      addres 0x53


//WGM0[1:0] - Waveform Generation Mode
//             |  WGM01 | WGM00 |
// Normal mode:      0  |   0 
// CTC mode:         0  |   1
// Fast PWM mode:    1  |   0
// PWM:              1  |   1


// COM0[1:0] - Compare Output Mode
// 0 0: Normal port operation, OC0 disconnected
// 0 1: Toggle OC0 on Compare Match
// 1 0: Clear OC0 on Compare Match
// 1 1: Set OC0 on Compare Match


// CS0[2:0] - Clock Select
// 0 0 0: No clock source (Timer/Counter stopped)
// 0 0 1: clkI/O/1 (No prescaling)
// 0 1 0: clkI/O/8 (From prescaler)
// 0 1 1: clkI/O/64 (From prescaler)
// 1 0 0: clkI/O/256 (From prescaler)
// 1 0 1: clkI/O/1024 (From prescaler)  
// 1 1 0: External clock source on T0 pin, clock on falling edge
// 1 1 1: External clock source on T0 pin, clock on rising edge

//============================================TCCR0========================================

//TCNT0 - Timer/Counter Register
//the timer/counter register TCNT0 can hold 0 to 255 

//============================================TIMSK========================================

//TIMSK - Timer/Counter Interrupt Mask Register
//    7      6         5        4       3       2       1       0
//  OCIE2 | TOIE2 | OCIE1B | OCIE1A | TOIE1 | OCIE0 | TOIE0 | TOIE2 |      address 0x59

//OCIE0  - Output Compare Match Interrupt Enable for Timer/Counter0
//TOIE0  - Timer/Counter0 Overflow Interrupt Enable
//OCIE1A - Output Compare Match A Interrupt Enable for Timer/Counter1
//OCIE1B - Output Compare Match B Interrupt Enable for Timer/Counter1
//TOIE1  - Timer/Counter1 Overflow Interrupt Enable
//OCIE2  - Output Compare Match Interrupt Enable for Timer/Counter2
//TOIE2  - Timer/Counter2 Overflow Interrupt Enable

//============================================TIFR========================================

//TIFR - Timer/Counter Interrupt Flag Register
//    7      6      5      4      3      2      1      0       
//  OCF2 | TOV2 | OCF1B | OCF1A | TOV1 | OCF0 | TOV0 | TOV2 |      address 0x58

//OCF0  - Output Compare Flag for Timer/Counter0
//TOV0  - Timer/Counter0 Overflow Flag
//OCF1A - Output Compare Flag A for Timer/Counter1  
//OCF1B - Output Compare Flag B for Timer/Counter1
//TOV1  - Timer/Counter1 Overflow Flag
//OCF2  - Output Compare Flag for Timer/Counter2
//TOV2  - Timer/Counter2 Overflow Flag

//============================================libraries========================================
#include <avr/io.h>
#include <avr/interrupt.h>

//============================================Defines========================================
#define F_CPU 8000000UL // Define CPU frequency as 8 MHz
#define TIMER0_PRESCALER 64 // Define prescaler for Timer0
#define delayTime 1000 // Define delay time in milliseconds
// This will toggle an LED every 1000 milliseconds (1 second)

//============================================global variables========================================

unsigned long previous = 0;
unsigned long millisCounter = 0;


//============================================ISRs========================================
// Timer0 overflow interrupt service routine
ISR(TIMER0_COMP_vect) {
    millisCounter++;
}

//============================================functions========================================
// Timer0 initialization function
// This function sets up Timer0 in CTC mode with a prescaler of 64
void initTimer0(void)
{ 
    //set output compare mode to CTC (Clear Timer on Compare Match) 
    TCCR0 |= (1<<WGM01);
    TCCR0 &= ~(1<<WGM00); // CTC mode

    //set the prescaler to 64
    TCCR0 |= (1<<CS01) | (1<<CS00); // Set CS02 and CS01 to 1, CS00 remains 0

    //enable the timer overflow interrupt
    TIMSK |= (1<<OCIE0); // Enable Output Compare Match Interrupt for Timer0


    //set the output compare register to 125
    OCR0 = 124;
    TCNT0 = 0; 
    // This will generate an interrupt every 1 ms with a 16 MHz clock and prescaler of 64
    //OCR0 = 1-(interrupt_CPU*time)/prescaler
    //125 = 1 - (8000000 * 0.001) / 64

}

//millis function
unsigned long millis(void){

    unsigned long ms;

    // Disable interrupts to ensure atomic access to millisCounter
    cli();
    ms = millisCounter;
    sei(); // Re-enable interrupts

    return ms;

}


//==============================================main code========================================

int main(void){

    initTimer0();

    DDRB |= (1 << 1); // Set PB1 as output (for example, to toggle an LED)

    PORTB &= ~(1 << 1); // Ensure PB1 is low initially

    sei(); // Enable global interrupts

    previous = millis(); // Initialize previous time
    while (1)
    {
        if(millis() - previous >= delayTime){
            PORTB ^= (1 << 1); // Toggle PB1
            previous = millis(); // Update previous time
        }
    }
    
}