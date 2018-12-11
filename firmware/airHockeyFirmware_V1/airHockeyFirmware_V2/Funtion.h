#ifndef FUNTION_H
#define FUNTION_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "Essential.cpp"
#include "ISR.h"

#define useDigiEndStop

void enableMotors();
void disableMotors();
void pinInit();
int endStopHitCheck();
void autoHome();
void beep(int fre,int dur);

#endif
