#define F_CPU 1000000UL  //1MgHz 

#define PORTB (*(volatile unsigned char *)0x38)  
#define DDRB (*(volatile unsigned char *)0x37)
#define PIND (*(volatile unsigned char *)0x30)
#define DDRD (*(volatile unsigned char *)0x31)

int main(void){

    DDRB = 0xFF;  // Set PORTB as output
    DDRD = 0x00; // Set PORTD as input
    PORTB = 0x00; // Initialize PORTB to low


    while (1)
    {
        if (PIND & (1 << 7)) { // Check if the 7th bit of PIND is set (button pressed)
            PORTB = 0xFF; // Set PORTB high
        } else {
            PORTB = 0x00; // Set PORTB low
        }        
    }
    
    return 0; // This line is never reached

}