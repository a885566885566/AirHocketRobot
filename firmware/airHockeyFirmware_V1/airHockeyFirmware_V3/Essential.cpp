#ifndef _ESSENTIAL
#define _ESSENTIAL

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

//pin setting
#define X_dirPin 6
#define X_StepPin 5
#define X_EnablePin 7
#define X_minEndstop A0
#define X_maxEndstop A1
#define Y_dirPin 3
#define Y_StepPin 2
#define Y_EnablePin 4
#define Y_minEndstop A2
#define Y_maxEndstop A3

#define beeperPin 8
#define ledPin 13

#define endStopHitState_X_min HIGH
#define endStopHitState_X_max LOW

#define endStopHitState_Y_min HIGH
#define endStopHitState_Y_max LOW

#define X_axisMinDir LOW
#define Y_axisMinDir HIGH

#define enableState LOW

#define defaultSpeed 50 //ms in unit

typedef struct{
  //long ttt;
  long x;
  long y;
}POINT;

typedef struct
{
  //long eddd;
  byte mode;
  long d1,d2;
}COMMAND;

typedef struct
{
  //long kk;
  long unit;
  POINT goalP;
  POINT preP;
  POINT nowP;
  bool workFlag;
  bool longerSide;
}ROUTE;

typedef struct _BLOCK
{
  //volatile int temp[8];
  //volatile long asd;
  volatile byte mode;
  volatile long dx,dy;
  volatile long tx,ty;
  volatile long x,y; 
  //volatile POINT nowPoint;
}BLOCK;
#endif
