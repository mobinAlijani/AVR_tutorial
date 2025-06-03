//===========================================================================================
// Project: ATmega32A Debounced Button with Timer0          
// Compiler: avr-gcc
// Target microcontroller: ATmega32A
// This code is for an ATmega32A microcontroller
// by [mobin Alijani]
// Date: 2023-10-01

//============================================libraries========================================
#include <avr/io.h>
#include <avr/interrupt.h>

//============================================Defines========================================
#define F_CPU 8000000UL // Define CPU frequency as 8 MHz
#define TIMER0_PRESCALER 64 // Define prescaler for Timer0
#define delayTime 50 // Define delay time in milliseconds
// This will toggle an LED every 1000 milliseconds (1 second)

//============================================global variables========================================

unsigned long previous = 0;
unsigned long millisCounter = 0;

unsigned char ReadButtonState = 0;
unsigned char lastButtonState = 0;
unsigned char ButtonState = 0;

struct deBouncd_Button
{
    unsigned long previous; // Previous time in milliseconds
    unsigned long millisCounter; // Millis counter for debouncing
    unsigned char ReadButtonState; // Current state of the button
    unsigned char lastButtonState; // Last state of the button
    unsigned char ButtonState; // Debounced button state
} Button1 =
{
    .previous = 0,
    .millisCounter = 0,
    .ReadButtonState = 0,
    .lastButtonState = 0,
    .ButtonState = 0
};



//============================================ISRs========================================
// Timer0 overflow interrupt service routine
ISR(TIMER0_COMP_vect) {
    millisCounter++;
    Button1.millisCounter++; // Increment the millis counter
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

    PORTD |= (1 << 6); // Enable pull-up resistor on PD6 (if used as input)
    DDRD &= ~(1 << 6); // Set PD6 as input (if used as input)

    sei(); // Enable global interrupts

    Button1.previous = millis(); // Initialize the previous time
    Button1.millisCounter = 0; // Initialize the millis counters
    // Main loop
    while (1)
    {
        Button1.ReadButtonState = (PIND & (1 << 6));

        if(Button1.ReadButtonState != Button1.lastButtonState) { // Check if the button state has changed
            Button1.lastButtonState = Button1.ReadButtonState; // Update the last button states
            Button1.previous = millis();    
        }


        if(millis() - Button1.previous >= delayTime){

            if(Button1.ButtonState != Button1.ReadButtonState){
                Button1.ButtonState = Button1.ReadButtonState;
                if(Button1.ButtonState == 0){ // Button pressed (assuming active low)
                    PORTB ^= (1 << 1); // Toggle PB1 (LED)
                }
            }

            Button1.previous =millis(); // Update previous time
        }

        Button1.lastButtonState = Button1.ReadButtonState; // Update the last button state for the next iteration
    }
    
}