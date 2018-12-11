#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "TIMER.h"
bool TIMER:: workingGroup[10]={false,false,false,false,false,false,false,false,false,false};
unsigned int TIMER:: timeMap[10][11]={{50,50,50,50,0,0,0,0,0,0,4},
                            {500,100,0,0,0,0,0,0,0,0,2},
                            {500,100,100,100,100,100,0,0,0,0,6},
                            {50,50,50,50,50,50,0,0,0,0,6},
                            {50,50,50,50,50,50,0,0,0,0,6},
                            {50,50,50,50,50,50,0,0,0,0,6},
                            {50,50,50,50,50,50,0,0,0,0,6},
                            {50,50,50,50,50,50,0,0,0,0,6},
                            {50,50,50,50,50,50,0,0,0,0,6},
                            {50,50,50,50,50,50,0,0,0,0,6}};
                            
TIMER::TIMER(int _mode,int _pin,int _group)
{
    mode=_mode;
    maxState=timeMap[mode][10];
    startTime=0;
    endTime=0;
    objWorking=false;
    pin=_pin;
    thisGroup=_group;
    pinMode(pin,OUTPUT);
    digitalWrite(pin,LOW);
    pinState=false;
    nowState=0;
}

bool TIMER::start()
    {
      if((!objWorking)&&(!workingGroup[thisGroup]))
      {
        nowState=0;
        //working=true;
        workingGroup[thisGroup]=true;
        objWorking=true;
        startTime=millis();
        endTime=startTime+timeMap[mode][nowState];
        nowState++;
        pinState=true;
        digitalWrite(pin,pinState);
        return true;
      }
      return false;
    }
    
    void TIMER::update()
    {
      if(objWorking)
      {
        if(nowState>=maxState)
        {//complete
          workingGroup[thisGroup]=false;
          objWorking=false;
          startTime=0;
          endTime=0;
          nowState=0;
          pinState=false;
          digitalWrite(pin,pinState);
        }
        else if(millis()>=endTime&&nowState<maxState)
        {
          startTime=millis();
          endTime=startTime+timeMap[mode][nowState];
          nowState++;
          pinState=!pinState;
          digitalWrite(pin,pinState);
        }
      }
    }
