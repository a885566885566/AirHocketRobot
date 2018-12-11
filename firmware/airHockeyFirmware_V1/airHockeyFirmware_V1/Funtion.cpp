#include "Funtion.h"

void enableMotors()
{
	digitalWrite(X_EnablePin,enableState);
	digitalWrite(Y_EnablePin,enableState);
}

void disableMotors()
{
	digitalWrite(X_EnablePin,!enableState);
	digitalWrite(Y_EnablePin,!enableState);
}

void pinInit()
{
	pinMode(X_dirPin,OUTPUT);
  	pinMode(X_StepPin,OUTPUT);
  	pinMode(X_EnablePin,OUTPUT);
  	pinMode(Y_dirPin,OUTPUT);
  	pinMode(Y_StepPin,OUTPUT);
  	pinMode(Y_EnablePin,OUTPUT);

    pinMode(beeperPin,OUTPUT);
    pinMode(ledPin,OUTPUT);
    
  	pinMode(X_minEndstop,INPUT);
  	pinMode(Y_maxEndstop,INPUT);
  	pinMode(X_minEndstop,INPUT);
  	pinMode(Y_maxEndstop,INPUT);
  	
  	digitalWrite(X_EnablePin,LOW);
  	digitalWrite(X_StepPin,LOW);
  	digitalWrite(X_dirPin,LOW);
  	digitalWrite(Y_EnablePin,LOW);
  	digitalWrite(Y_StepPin,LOW);
  	digitalWrite(Y_dirPin,LOW);
  	digitalWrite(ledPin,LOW);
}

int endStopHitCheck()
{
	#ifdef useDigiEndStop
  	if(digitalRead(X_minEndstop)==endStopHitState_X_min)
    	return 1;
  	else if(digitalRead(X_maxEndstop)==endStopHitState_X_max)
    	return 2;
  	else if(digitalRead(Y_minEndstop)==endStopHitState_Y_min)
    	return 3;
  	else if(digitalRead(Y_maxEndstop)==endStopHitState_Y_max)
    	return 4;
  	else
    	return 0;
    #endif
}

void autoHome()
{
	//ISR_disable();
  	enableMotors();
  	digitalWrite(X_dirPin,X_axisMinDir);
  	while(endStopHitCheck()!=1)			//hit X min
  	{
    	digitalWrite(X_StepPin,LOW);
    	delayMicroseconds(defaultSpeed);
    	digitalWrite(X_StepPin,HIGH);
    	delayMicroseconds(defaultSpeed);
  	}

    digitalWrite(X_dirPin,!X_axisMinDir);
    for(int i=0;i<350;i++)
    {
      digitalWrite(X_StepPin,LOW);
      delayMicroseconds(defaultSpeed);
      digitalWrite(X_StepPin,HIGH);
      delayMicroseconds(defaultSpeed);
    }

      
  	digitalWrite(Y_dirPin,Y_axisMinDir);
  	while(endStopHitCheck()!=3)			//hit Y min
  	{
    	digitalWrite(Y_StepPin,LOW);
    	delayMicroseconds(defaultSpeed);
    	digitalWrite(Y_StepPin,HIGH);
    	delayMicroseconds(defaultSpeed);
  	}

  	digitalWrite(Y_dirPin,!Y_axisMinDir);
  	for(int i=0;i<350;i++)
  	{
	    digitalWrite(Y_StepPin,LOW);
	    delayMicroseconds(defaultSpeed);
	    digitalWrite(Y_StepPin,HIGH);
	    delayMicroseconds(defaultSpeed);
  	}
   POINT homePoint(0,0);
  	setMachinePosition(homePoint);
  	disableMotors();
}
void beep(int fre,int dur)
{
  tone(beeperPin,fre,dur);
}
