#define F_CPU 1000000UL  //1MgHz 

#define PORTB (*(volatile unsigned char *)0x38)  
#define DDRB (*(volatile unsigned char *)0x37)

void delay_ms(unsigned int time){
	for(unsigned int i=0;i<time;i++){
	        for(volatile unsigned int j = 0; j < 50; j++);
	}
}


int main(void){

	DDRB |=(1<<1);    // output pin1 in PORTB
	PORTB = 0x00;	//All pin in PORTB off

	while(1){		//LOOP 
	
	PORTB ^=(1<<1);		//toggle pin1

	delay_ms(500);		//add delay 
	}
	return 0
}

