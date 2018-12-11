/*
 *  Created by 朱雁丞 on 2018/02/13.
 *  Copyright (c) 朱雁丞 CHU,YEN CHENG All rights reserved.
 */
#define _useTimer4
#define DEBUGGER

#include "Funtion.h"
#include "ISR.h"
//#include "TIMER.h"

void setup() {
  	pinInit();
  	ISRinit();
    beep(500,200);
  	autoHome();
    //beep(1000,500);
  	
  	//ISR_enable();
    enableMotors();
}

void loop() {
  int t = millis();
  while(millis()-t <5000)
	  bufferUpdata();
  Serial.println("done");
  while(1){
    ISR_enable();
    bufferUpdata();
    if(endStopHitCheck()!=0)
      disableMotors();
  }
}
