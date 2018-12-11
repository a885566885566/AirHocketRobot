#ifndef FUNTION_H
#define FUNTION_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "Essential.cpp"
#include "ISR.h"

#define xInitMove 350
#define yInitMove 350


void enableMotors();
void disableMotors();
void pinInit();
int endStopHitCheck();            //return 0 means nothing hitting
void autoHome();
void beep(int fre,int dur);
void X_initMove(bool dir);
void Y_initMove(bool dir);
void gamingCheck();

#endif
