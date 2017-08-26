#ifndef ZO_SYSTEM_TIMER_H
#define ZO_SYSTEM_TIMER_H

#include <stdbool.h>


//timeout functionality 
void zoSystemTimerTimeOutInit(uint16_t *counter);
bool zoSystemTimerTimeOutExpired(uint16_t *counter, const uint16_t timeOutMiliSecond);

//time measuring functionality
//system measured in ms
void zoSystemTimerMeasureStart(uint16_t *counter);
uint16_t zoSystemTimerMeasureGet(uint16_t *counter);

#endif //ZO_SYSTEM_TIMER_H