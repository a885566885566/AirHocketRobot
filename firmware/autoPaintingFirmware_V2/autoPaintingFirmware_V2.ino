#include <ServoTimer1.h>

//#define DEBUGGER
#define _useTimer4
#include "Essential.cpp"
#include "SDcmd.cpp"
#include "Block.cpp"
//#include "Servo.h"
#include <MegaServo.h>
#define X_dir 3
#define X_Step 2
#define Y_dir 6
#define Y_Step 5

volatile BLOCK_BUFFER blockBuffer;
volatile BLOCK* block;
volatile bool ISR_flag;

volatile long tempTimeX=0;
volatile long tempTimeY=0;
volatile bool Xstate = false;
volatile bool Ystate = false;

//ServoTimer1 penServo;
MegaServo penServo;
bool autoReloadEnable = false;
void setup() {
  // put your setup code here, to run once: 
  //Serial.begin(9600);
  //Serial.println("*");
  pinMode(53,OUTPUT);
  digitalWrite(53,LOW);
  pinMode(X_dir,OUTPUT);
  pinMode(X_Step,OUTPUT);
  pinMode(Y_dir,OUTPUT);
  pinMode(Y_Step,OUTPUT);
  block->mode=0;
  penServo.attach(9);
  ISR_disable();
  while(blockBuffer.notFull())
        blockBuffer.loadCommand();
}

void loop() {
  // put your main code here, to run repeatedly:
  //Serial.println("*");
  /*
  if(Serial.available())
    switch(Serial.read())
    {
      case 'p':
        blockBuffer.printInfo();
        break;
      case 'a':
        autoReloadEnable = true;
        break;
      case '+':
        //block->goalP.x=count;
        //block->goalP.y=count*10;
        
        blockBuffer.push(block);
        blockBuffer.printInfo();
        break;
      case '-':
        block = (blockBuffer.pop());
        #ifdef DEBUGGER
        Serial.print("====POP NEW BLOCK= ");
        #endif
        block->printInfo();
        break;
      case '1':
        ISR_enable();
        break;
    }
     if(blockBuffer.getCapcity()<99)
        {
          Serial.println("++");
          blockBuffer.loadCommand();
        }*/
   //if(autoReloadEnable)
     //if(blockBuffer.notFull())
     
     ISR_enable();
     while(1)
     {
       //Serial.println("+");
       //if(blockBuffer.notFull())
        //{
         // Serial.println("+");
          //blockBuffer.loadCommand();
        //}
     }
}

void ISR_enable()
{
  ISR_flag = false;
  TCCR3A = 0;
  TCCR3B = 0; 
  TCCR3B |= (1<<WGM32);  // CTC mode; Clear Timer on Compare
  TCCR3B |= (1<<CS31);  // Prescaler == 8 ||(1<<CS30)
  TIMSK3 |= (1 << OCIE3A);  // enable CTC for TIMER1_COMPA_vect
  TCNT3=0;  // counter 歸零 
  OCR3A = 10;
}

void ISR_disable()
{
  TCCR3A = 0;
  TCCR3B = 0; 
}

ISR(TIMER3_COMPA_vect)
{
  //if(block->mode == 0) //empty block
  if(!ISR_flag)
  {
    //Serial.println("stock");
    //Serial.println(block->mode);
    //if(blockBuffer.getCapcity()<5)
    //  blockBuffer.loadCommand();
    block = (blockBuffer.pop());
    if(block->mode == 1)
    {
      //Serial.println("enter");
      ISR_flag = true;
      tempTimeX = block->tx;
      tempTimeY = block->ty;
      if(block->dx >= 0)
        digitalWrite(X_dir,HIGH);
      else
      {
        digitalWrite(X_dir,LOW);
        block->dx *= -1;
      }
      if(block->dy >= 0)
        digitalWrite(Y_dir,HIGH);
      else
      {
        digitalWrite(Y_dir,LOW);
        block->dy *= -1;
      }
    }
    else if(block->mode == 2) 
    {
      if(block->tx == 1)
        penServo.write(120);
      else
        penServo.write(105);
    }
  }
  if(block->mode == 1)
  {
    //Serial.println("hit");
    if(block->dx > 0)
    {
      if(tempTimeX > 0)
      {
        tempTimeX--;
      }
      else
      {
        Xstate = !Xstate;
        digitalWrite(X_Step,Xstate);
        tempTimeX = block->tx;
        block->dx--;
      }
    }
    if(block->dy > 0)
    {
      if(tempTimeY > 0)
      {
        tempTimeY--;
        //Serial.println(tempTimeY);
      }
      else
      {
        Ystate = !Ystate;
        digitalWrite(Y_Step,Ystate);
        tempTimeY = block->ty;
        block->dy--;
      }
    }
  }
    if((block->dx <= 0)&&(block->dy <= 0))
    {
      block->mode = 0;
      ISR_flag = false;
      //Serial.println("++");
    }
    if(blockBuffer.notFull())
    {
      //ISR_flag = ISR_flag;
      //int ccc = block->mode;
      //block->mode = ccc;
      blockBuffer.loadCommand();
    }
    /*
    if(blockBuffer.notFull())
        {
          //Serial.println("+");
          blockBuffer.loadCommand();
        }*/
}
