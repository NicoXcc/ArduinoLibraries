  #include <avr/io.h>
  #include <avr/interrupt.h>

#include "zoSystemTimer.h"
//#include "zoMcu.h"


static volatile uint16_t Miliseconds = 0;

//timeout functionality 

 void zoSystemTimerTimeOutInit(uint16_t *counter)
{
	zoSystemTimerMeasureStart(counter);
}

 bool zoSystemTimerTimeOutExpired(uint16_t *counter, const uint16_t timeOutMiliSecond)
{
	return( ( zoSystemTimerMeasureGet(counter) >= timeOutMiliSecond ) ? true : false );
}

//time measuring functionality
//system measured in ms
 void zoSystemTimerMeasureStart(uint16_t *counter)
{
//	noInterrupts();
	*counter = Miliseconds;
//	interrupts();
}

uint16_t zoSystemTimerMeasureGet(uint16_t *counter)
{
	uint16_t curr;
	
//	noInterrupts();
	curr= Miliseconds;	
//	interrupts();

	if(*counter <= curr)
		return (curr - *counter);
	else
		return (0xFFFF + curr -*counter); 
}

