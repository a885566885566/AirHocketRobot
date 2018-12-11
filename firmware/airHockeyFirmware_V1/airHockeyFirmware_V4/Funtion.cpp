#include "Funtion.h"

void enableMotors(){
	digitalWrite(X_EnablePin,enableState);
	digitalWrite(Y_EnablePin,enableState);
}

void disableMotors(){
	digitalWrite(X_EnablePin,!enableState);
	digitalWrite(Y_EnablePin,!enableState);
}

void pinInit(){
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

int endStopHitCheck(){
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
}

void X_initMove(bool dir){
  if(dir)
    digitalWrite(X_dirPin,!X_axisMinDir);
  else
    digitalWrite(X_dirPin,X_axisMinDir);
  for(int i=0;i<xInitMove;i++){
    digitalWrite(X_StepPin,LOW);
    delayMicroseconds(defaultSpeed);
    digitalWrite(X_StepPin,HIGH);
    delayMicroseconds(defaultSpeed);
  }
}
void Y_initMove(bool dir){
  if(dir)
    digitalWrite(Y_dirPin,!Y_axisMinDir);
  else
    digitalWrite(Y_dirPin,Y_axisMinDir);
  for(int i=0;i<yInitMove;i++){
    digitalWrite(Y_StepPin,LOW);
    delayMicroseconds(defaultSpeed);
    digitalWrite(Y_StepPin,HIGH);
    delayMicroseconds(defaultSpeed);
  }
}

void autoHome(){
	  ISR_disable();
  	enableMotors();
  	digitalWrite(X_dirPin,X_axisMinDir);
  	while(endStopHitCheck()!=1){			//hit X min
    	digitalWrite(X_StepPin,LOW);
    	delayMicroseconds(defaultSpeed);
    	digitalWrite(X_StepPin,HIGH);
    	delayMicroseconds(defaultSpeed);
  	}
    X_initMove(true);

  	digitalWrite(Y_dirPin,Y_axisMinDir);
  	while(endStopHitCheck()!=3){			//hit Y min
    	digitalWrite(Y_StepPin,LOW);
    	delayMicroseconds(defaultSpeed);
    	digitalWrite(Y_StepPin,HIGH);
    	delayMicroseconds(defaultSpeed);
  	}
    Y_initMove(true);
  	
    POINT homeP;
    homeP.x = homeP.y = 0;
  	setMachinePosition(homeP);
  	disableMotors();
}
void gamingCheck(){
  int hitState = endStopHitCheck();
  if(hitState != 0){
    ISR_disable();
    POINT newPoint;
    switch(endStopHitCheck()){
      case 1:     //x min hitting
        X_initMove(true);
        newPoint.x = 0;    //not edit this axis value
        newPoint.y = -1;
        break;
      case 2:     //x max hitting
        X_initMove(false);
        newPoint.x = machineMaxSizeX;    //not edit this axis value
        newPoint.y = -1;
        break;
      case 3:     //y min hitting
        Y_initMove(true);
        newPoint.x = -1;    //not edit this axis value
        newPoint.y = 0;
        break;
      case 4:     //y max hitting
        Y_initMove(false);
        newPoint.x = -1;    //not edit this axis value
        newPoint.y = machineMaxSizeY;
        break;
    }
    setMachinePosition(newPoint);
    ISR_enable();
  }
}
void beep(int fre,int dur)
{
  tone(beeperPin,fre,dur);
}
