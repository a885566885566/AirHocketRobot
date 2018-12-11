#ifndef _ISR
#define _ISR

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "Essential.cpp"
#include "Command.cpp"
#include "Funtion.h"
#include "BlockBuffer.h"

extern volatile BLOCKBUFFER blockBuffer;
 
extern volatile BLOCK* block;
extern volatile bool ISR_flag;

extern volatile int tempTimeX;
extern volatile int tempTimeY;

extern volatile bool Xstate;
extern volatile bool Ystate;

void ISR_enable();
void ISR_disable();
void ISRinit();
void bufferUpdata();
void setMachinePosition(POINT newPoint);

#endif
