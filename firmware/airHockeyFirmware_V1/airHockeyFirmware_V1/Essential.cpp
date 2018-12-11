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

class POINT
{
  public:
  int t;
  long x;
  long y;
  
  POINT(long newX, long newY)
  {
    t = 0;
    x = newX;
    y = newY;
  }
  POINT()
  {
    t = x = y = 0;
  }
  #ifdef DEBUGGER
  void printInfo()
  {
    Serial.print("(X,Y)=");
    Serial.print(x);
    Serial.print(",");
    Serial.println(y);
  }
  #endif
};

class COMMAND
{
  public:
  int ttt;
  int mode;
  long d1,d2;
  COMMAND()
  {
    mode = d1 = d2 =0;
  }
  
  #ifdef DEBUGGER
  void printInfo()
  {
    Serial.print("(m,d1,d2)=");
    Serial.print(mode);
    Serial.print(",");
    Serial.print(d1);
    Serial.print(",");
    Serial.println(d2);
  }
  #endif
};

class ROUTE
{
  public:
  int k;
  long unit;
  POINT goalP;
  POINT preP;
  POINT nowP;
  bool workFlag;
  bool longerSide;
};

class BLOCK
{
  public:
  volatile int temp;
  volatile int mode;
  volatile int dx,dy;
  volatile int tx,ty;
  volatile POINT nowPoint;

  BLOCK()
  {
    temp = mode = dx = dy = tx = ty = nowPoint.x = nowPoint.y = 0;
  }
  
  void printInfo()
  {
    Serial.print("BLOCK ");
    Serial.print(" Mode= ");
    Serial.print(mode);
    Serial.print("D= ");
    Serial.print(dx);
    Serial.print(",");
    Serial.print(dy);
    Serial.print(" T= ");
    Serial.print(tx);
    Serial.print(",");
    Serial.println(ty);
    //goalP.printInfo();
  }
  //private:
  //define for auto acceration position
  //const double t1 = 0.2;
  //const double t2 = 0.8;
};
#endif
