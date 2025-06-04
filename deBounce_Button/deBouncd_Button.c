//===========================================================================================
// Project: ATmega32A Debounced Button with Timer0          
// Compiler: avr-gcc
// Target microcontroller: ATmega32A
// This code toggles an LED on PB1 when a button on PD6 (with pull-up resistor) is pressed.
// Button is active-low (pressed = 0, released = 1).
// Author: [mobin Alijani]
// Date: 2023-10-01
// Modified: 2025-06-04 

//============================================libraries========================================
#include <avr/io.h>
#include <avr/interrupt.h>

//============================================Defines========================================
#define F_CPU 8000000UL // Define CPU frequency as 8 MHz
#define TIMER0_PRESCALER 64 // Define prescaler for Timer0
#define delayTime 50 // Define debounce delay time in milliseconds

#define LED_Toggle() PORTB ^= (1 << PB1)   // Toggle LED on PB1
#define BTN_Pin  ((PIND & (1 << PD6)) != 0)  // for read button pin 


//============================================global variables========================================
volatile unsigned long millisCounter = 0; // Counter for milliseconds, shared with ISR

struct DebouncedButton
{
    unsigned long previous; // Previous time in milliseconds
    unsigned char ReadButtonState; // Current state of the button
    unsigned char lastButtonState; // Last state of the button
    unsigned char ButtonState; // Debounced button state
    unsigned char debounceDelay; // Debounce delay in milliseconds
    volatile unsigned char* port; // Pointer to PORT register (e.g., &PORTB)
    volatile unsigned char* pin;  // Pointer to PIN register (e.g., &PIND)
    unsigned char buttonPin;      // Button pin number (e.g., PD6)
    unsigned char ledPin;         // LED pin number (e.g., PB1)
}Button1;

//============================================ISRs========================================
// Timer0 compare match interrupt service routine
ISR(TIMER0_COMP_vect) {
    millisCounter++; // Increment millisecond counter
}

//============================================functions========================================
// Timer0 initialization function
// Sets up Timer0 in CTC mode with a prescaler of 64 to generate 1ms interrupts
void initTimer0(void)
{ 
    TCCR0 = 0;

    // Set CTC mode (Clear Timer on Compare Match)
    TCCR0 |= (1<<WGM01);
    TCCR0 &= ~(1<<WGM00);

    // Set prescaler to 64
    TCCR0 |= (1<<CS01) | (1<<CS00);

    // Enable Output Compare Match Interrupt
    TIMSK |= (1<<OCIE0);

    // Set output compare register for 1ms interrupts
    // OCR0 = (F_CPU / (Prescaler * Desired_Frequency)) - 1 = (8000000 / (64*1000)) - 1 = 124
    OCR0 = 124;
    TCNT0 = 0;
}

// Initialize and Config PORTs
void initButton(struct DebouncedButton* btn, volatile unsigned char* port, volatile unsigned char* pin
    ,unsigned char buttonPin, unsigned char ledPin, unsigned char debounceDelay)
{
    btn->previous=0;
    btn->ReadButtonState=0;
    btn->ButtonState=0;
    btn->lastButtonState=0;

    btn->debounceDelay=debounceDelay;
    btn->port = port;
    btn->pin = pin;
    btn->buttonPin = buttonPin;
    btn->ledPin = ledPin;

    // Configure button pin as input with pull-up
    DDRD &= ~(1 << buttonPin); // Set buttonPin as input
    *port |= (1 << buttonPin); // Enable pull-up resistor

    // Configure LED pin as output
    DDRB |= (1 << ledPin); // Set ledPin as output
    *port &= ~(1 << ledPin); // LED off initially
}

// Returns current time in milliseconds
unsigned long millis(void)
{
    unsigned long ms;
    cli(); // Disable interrupts for atomic access
    ms = millisCounter;
    sei(); // Re-enable interrupts
    return ms;
}


// Check if the specified delay has elapsed, handling timer overflow
unsigned char isTimeElapsed(unsigned long current, unsigned long previous, unsigned char delay) {
    return (current - previous) >= delay || current < previous;
}


//==============================================main code========================================
int main(void)
{
    initTimer0(); // Initialize Timer0

    initButton(&Button1,&PORTD,&PIND,PD6,PB1,delayTime);

    sei(); // Enable global interrupts

    Button1.previous = millis(); // Initialize previous time

    // Main loop
    while (1)
    {
        // Read button state (active-low: 0 = pressed, 1 = released)
        Button1.ReadButtonState = (BTN_Pin) ? 0 : 1;

        // Detect button state change
        if (Button1.ReadButtonState != Button1.lastButtonState) {
            Button1.previous = millis(); // Record time of state change
        }

        // Check if debounce delay has passed or timer overflow occurred
        if (isTimeElapsed(millis(),Button1.previous,Button1.debounceDelay))
        {
            // Update debounced state if changed
            if (Button1.ButtonState != Button1.ReadButtonState)
            {
                Button1.ButtonState = Button1.ReadButtonState;
                if (Button1.ButtonState) { // Button pressed (active-low)
                    LED_Toggle();
                }
            }
            Button1.previous = millis(); // Update previous time
        }

        Button1.lastButtonState = Button1.ReadButtonState; // Update last button state
    }
}