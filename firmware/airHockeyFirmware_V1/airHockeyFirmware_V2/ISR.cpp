#include "ISR.h"


volatile BLOCKBUFFER blockBuffer;
 
volatile BLOCK* block;
volatile bool ISR_flag;

volatile int tempTimeX;
volatile int tempTimeY;

volatile bool Xstate;
volatile bool Ystate;

void ISR_enable()
{
  TCCR3A = 0;
  TCCR3B = 0; 
  TCCR3B |= (1<<WGM32);  			// CTC mode; Clear Timer on Compare
  TCCR3B |= (1<<CS31);  			// Prescaler == 8 ||(1<<CS30)
  TIMSK3 |= (1 << OCIE3A);  		// enable CTC for TIMER1_COMPA_vect
  TCNT3=0;  						// counter �k�s 
  OCR3A = 10;
}

void ISR_disable()
{
  TCCR3A = 0;
  TCCR3B = 0; 
}

void ISRinit()
{
  ISR_flag = false;
  tempTimeX=0;
  tempTimeY=0;
  Xstate = false;
  Ystate = false;
  blockBuffer.init();
	ISR_disable();
}

void bufferUpdata()
{
	blockBuffer.refresh();
}

ISR(TIMER3_COMPA_vect)
{
	if(!ISR_flag)
  	{
	    block = (blockBuffer.pop());
	    if(block->mode == CmdMode_MOVE)
	    {
  			ISR_flag = true;
	      	tempTimeX = block->tx;
	      	tempTimeY = block->ty;
	      	if(block->dx >= 0)					//set the direction of X axis motor  
	        	digitalWrite(X_dirPin,!X_axisMinDir);
	      	else
	      	{
	        	digitalWrite(X_dirPin,X_axisMinDir);
	        	block->dx *= -1;
	      	}
	      	if(block->dy >= 0)					//set the direction of Y axis motor  
	        	digitalWrite(Y_dirPin,!Y_axisMinDir);
	      	else
	      	{
	        	digitalWrite(Y_dirPin,Y_axisMinDir);
	        	block->dy *= -1;
	      	}
    	}
    	else									//for other command
	    {
	    	switch(block->mode)
	    	{
	    		case CmdMode_HOME:
          {
          ISR_disable();
	    			autoHome();
           POINT homePoint;
           //homePoint.x = homePoint.y = 0;
           blockBuffer.setPosition(homePoint);
           //setMachinePosition(homePoint);
	    			break;
          }
	    		case CmdMode_DELAY:
	    			ISR_disable();
	    			delay(block->dx);
	    			ISR_enable();
	    			break;
			}
	    }
    }
  	if(block->mode == CmdMode_MOVE)
  	{
    	if(block->dx > 0)
    	{
      		if(tempTimeX > 0)
      		{ 
        		tempTimeX--;
      		} 
			else
     		{
		        Xstate = !Xstate;
		        digitalWrite(X_StepPin,Xstate);
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
		        digitalWrite(Y_StepPin,Ystate);
		        tempTimeY = block->ty;
		        block->dy--;
	      	}
	    }
	    if((block->dx <= 0)&&(block->dy <= 0))
	    {
	      	block->mode = 0;
	      	ISR_flag = false;
	    }
    }
}

void setMachinePosition(POINT newPoint)
{
  blockBuffer.setPosition(newPoint);
}
