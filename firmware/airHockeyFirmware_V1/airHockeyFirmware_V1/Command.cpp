#ifndef _CMD
#define _CMD

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "Essential.cpp" 


//#define useSerial

#define CmdMode_MOVE 1
#define CmdMode_HOME 2
#define CmdMode_DELAY 3
#define CmdMode_DISABLE 4
#define CmdMode_ENABLE 5
#define CmdMode_COMMENT 6
//enum CmdMode {MOVE=1,HOME,DELAY,DISABLE,ENABLE,COMMENT};

class CMD
{
	private:
  //int ww;
  long commandData[4];
		
	public:
		CMD()
		{
		}
		
		void CmdInit()
		{
			//#ifdef useSerial
			Serial.begin(9600);
			//#endif
		}
		
		bool available()
		{
			//#if defined useSerial
			if(Serial.available())
			{
        commandData[0] = 0;
        commandData[1] = 0;
        commandData[2] = 0;
        commandData[3] = 0;
				short commandArrayCount=0;
        char SDdata=99;
        //while(Serial.available())
        while(SDdata != ';')
        {
          SDdata=Serial.read();
          Serial.print(SDdata);
          switch(SDdata)
          {
            case 'M':
            Serial.print("\npreFirst=");
            Serial.println(commandData[0]);
              commandData[0]=CmdMode_MOVE;
              Serial.print("first=");
            Serial.println(commandData[0]);
              break;
            case 'A':
              commandData[0]=CmdMode_ENABLE;
              break;
            case 'H':
              commandData[0]=CmdMode_HOME;
              break;
            case 'D':
              commandData[0]=CmdMode_DELAY;//delay
              break;
            case 'B':
              commandData[0]=5;
              break;
            case 'V':
              commandData[0]=6; //set Speed
              break;
            case 'S':
              commandData[0]=CmdMode_DISABLE; //disable all motor
              break;
            case 'C':
              commandData[0]=CmdMode_COMMENT;//comment mode
              Serial.read();
              commandArrayCount++;
              break;
            case ',':
              commandArrayCount++;
              break;
            default:
              if(commandArrayCount != 0)
              {
                if(SDdata<='9'&&SDdata>='0'&&commandData[0]!=CmdMode_COMMENT)
                  commandData[commandArrayCount]=(SDdata-'0')+commandData[commandArrayCount]*10;
              }
              else
                Serial.println("waring -> first char is not mode lable");
              break;
          }
        }
        this->printInfo();
        //use checksum to check data
        if(commandData[0] == CmdMode_MOVE)
        {
        	if(commandData[1] + commandData[2] == commandData[3])
        		return true;
        	else
        	  return false;
				}
				
				return true;  //return true in default
			}
     return false;
			//#endif
		}
		
		COMMAND read()
		{
    Serial.print("cmd info");
    Serial.println(commandData[0]);
			COMMAND newCmd;
			newCmd.mode=commandData[0];
			newCmd.d1=commandData[1];
			newCmd.d2=commandData[2];
			return newCmd;
		}
		
		//#ifdef DEBUGGER
		void printInfo()
		{
            Serial.print("commnd=");//++","+commandData[1]+","+commandData[2]
            Serial.print(commandData[0]);
            Serial.print(",");
            Serial.print(commandData[1]);
            Serial.print(",");
            Serial.print(commandData[2]);
            Serial.print(", checksum=");
            Serial.println(commandData[3]);
		}
    	//#endif
};

#endif
