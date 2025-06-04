//===========================================================================================
// Project: ATmega32A Debounced Button with Timer0
// Compiler: avr-gcc (version 7.3.0)
// Target microcontroller: ATmega32A
// Description: This code manages a button connected to pin PD6 (active-low with pull-up resistor)
//              and toggles an LED on pin PB1 when the button is pressed. The button is active-low
//              (pressed = 0, released = 1). Debouncing is handled using Timer0 interrupts.
// Author: [Mobin Alijani]
// Date created: 2023-10-01
// Date modified: 2025-06-04
//===========================================================================================

//============================================Libraries========================================
// Include necessary libraries for I/O registers and interrupt handling
#include <avr/io.h>        // Provides definitions for ATmega32A I/O registers
#include <avr/interrupt.h> // Provides definitions for interrupt handling

//============================================Defines========================================
// Constants for hardware configuration and program logic
#define F_CPU 8000000UL      // CPU frequency set to 8 MHz
#define TIMER0_PRESCALER 64  // Prescaler for Timer0
#define delayTime 50         // Debounce delay time in milliseconds
#define LED_Toggle() PORTB ^= (1 << PB1) // Macro to toggle LED on pin PB1


//============================================Global Variables========================================
// Global variables and structures used throughout the program
volatile unsigned long millisCounter = 0; // Millisecond counter, shared with ISR
                                         // 'volatile' ensures the compiler doesn't optimize
                                         // accesses to this variable, as it's modified in an ISR.

// Structure to manage a debounced button
struct DebouncedButton
{
    unsigned long previous;        // Previous timestamp (milliseconds) for tracking changes
    unsigned char ReadButtonState; // Current button state (read from pin)
    unsigned char lastButtonState; // Previous button state for detecting changes
    unsigned char ButtonState;     // Debounced button state (0 or 1)
    unsigned char debounceDelay;   // Debounce delay in milliseconds
    volatile unsigned char* port;  // Pointer to PORT register (e.g., &PORTD)
    volatile unsigned char* pin;   // Pointer to PIN register (e.g., &PIND)
    volatile unsigned char* DDRx;  // Pointer to DDR register (e.g., &DDRD)
    unsigned char buttonPin;       // Button pin number (e.g., PD6)

} Button1; // Instance of the structure for the button on PD6

//============================================Interrupt Service Routines (ISRs)========================================
// Timer0 Compare Match ISR
// Triggered every 1ms to increment the millisecond counter
ISR(TIMER0_COMP_vect) {
    millisCounter++; // Increment the millisecond counter
}

//============================================Functions========================================
// Initialize Timer0
// Configures Timer0 in CTC mode with a prescaler of 64 to generate 1ms interrupts
void initTimer0(void)
{
    TCCR0 = 0; // Clear Timer0 control register

    // Set CTC mode (Clear Timer on Compare Match)
    TCCR0 |= (1<<WGM01);  // Enable CTC mode
    TCCR0 &= ~(1<<WGM00); // Ensure WGM00 is cleared for CTC mode

    // Set prescaler to 64
    TCCR0 |= (1<<CS01) | (1<<CS00); // CS01 and CS00 set for prescaler 64

    // Enable Output Compare Match Interrupt
    TIMSK |= (1<<OCIE0); // Enable Timer0 compare match interrupt

    // Set output compare register for 1ms interrupts
    // Formula: OCR0 = (F_CPU / (Prescaler * Desired_Frequency)) - 1
    //          = (8000000 / (64 * 1000)) - 1 = 124
    OCR0 = 124;
    TCNT0 = 0; // Initialize Timer0 counter to 0
}

// Initialize button configuration
// Sets up the button pin as input with pull-up and initializes the button structure
void initButton(struct DebouncedButton* btn, volatile unsigned char* port, volatile unsigned char* pin,
                volatile unsigned char* DDRx, unsigned char buttonPin, unsigned char debounceDelay)
{
    // Initialize button structure fields
    btn->previous = 0;          // Clear previous timestamp
    btn->ReadButtonState = 0;   // Clear current button state
    btn->ButtonState = 0;       // Clear debounced button state
    btn->lastButtonState = 0;   // Clear last button state

    // Assign configuration parameters
    btn->debounceDelay = debounceDelay; // Set debounce delay
    btn->port = port;                   // Set PORT register pointer
    btn->pin = pin;                     // Set PIN register pointer
    btn->DDRx = DDRx;                   // Set DDR register pointer
    btn->buttonPin = buttonPin;         // Set button pin number

    // Configure button pin as input with pull-up resistor
    *DDRx &= ~(1 << buttonPin); // Clear DDR bit to set pin as input
    *port |= (1 << buttonPin);  // Set PORT bit to enable pull-up resistor

}

// Get current time in milliseconds
// Returns the value of millisCounter with atomic access
unsigned long millis(void)
{
    unsigned long ms;      // Temporary variable to store millisCounter
    cli();                 // Disable interrupts for atomic access
    ms = millisCounter;    // Read millisCounter
    sei();                 // Re-enable interrupts
    return ms;             // Return millisecond count
}

// Check if the specified delay has elapsed
// Handles timer overflow for reliable timing
unsigned char isTimeElapsed(unsigned long current, unsigned long previous, unsigned char delay)
{
    // Return true if delay has elapsed or timer has overflowed
    return (current - previous) >= delay || current < previous;

}

// Update button state with debouncing
// Reads the button state, applies debouncing, and returns 1 if button is pressed
unsigned char updateButton(struct DebouncedButton* btn)
{
    // Read button state (active-low: 0 = pressed, 1 = released)
    btn->ReadButtonState = (*(btn->pin) & (1 << btn->buttonPin)) ? 0 : 1;
    

    // Detect button state change
    if (btn->ReadButtonState != btn->lastButtonState) {
        btn->previous = millis(); // Record time of state change
    }

    // Check if debounce delay has passed or timer overflow occurred
    if (isTimeElapsed(millis(), btn->previous, btn->debounceDelay))
    {
        // Update debounced state if changed
        if (btn->ButtonState != btn->ReadButtonState)
        {
            btn->ButtonState = btn->ReadButtonState; // Update debounced state
            if (btn->ButtonState) { // Button pressed (active-low)
                return 1; // Signal button press
            }
        }
        btn->previous = millis(); // Update previous time
    }
    btn->lastButtonState = btn->ReadButtonState; // Update last button state
    return 0; // No button press detected
}

//============================================Main Code========================================
// Main program entry point
int main(void)
{
    initTimer0(); // Initialize Timer0 for 1ms interrupts

    // Initialize button on PD6 with pull-up resistor
    initButton(&Button1, &PORTD, &PIND, &DDRD, PD6, delayTime);

    // Configure LED pin as output
    DDRB |= (1 << 1);  // Set PB1 as output
    PORTB &= ~(1 << 1); // Initialize LED off

    sei(); // Enable global interrupts

    Button1.previous = millis(); // Initialize previous timestamp

    // Main loop
    while (1)
    {
        // Check for button press and toggle LED if pressed
        if (updateButton(&Button1)) {
            LED_Toggle();
        }
    }
}
