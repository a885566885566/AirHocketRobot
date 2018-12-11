#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#ifndef _POINT
#define _POINT
class POINT
{
  public:
  int t;
  long x;
  long y;
  POINT()
  {
    x = y = 0;
  }
  POINT(long newX, long newY)
  {
    x = newX;
    y = newY;
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
#endif
