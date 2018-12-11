#ifndef _CMD
#define _CMD

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "Essential.cpp" 


//#define useSerial
#define serialBufferSize 35

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
  //long ww;
  volatile long commandData[4];
  char serialBuffer[serialBufferSize];
  int serialIndex;
  COMMAND newCmd;
		
	public:
		CMD()
		{
      for(int i=0;i<serialBufferSize;i++)
        serialBuffer[i] = 0;
      serialIndex = 0;
		}
		
		void CmdInit()
		{
			//#ifdef useSerial
			Serial.begin(9600);
			//#endif
      for(int i=0;i<serialBufferSize;i++)
        serialBuffer[i] = 0;
      serialIndex = 0;
		}
		
		bool available()
		{
			//#if defined useSerial
			if(Serial.available())
			{
        serialIndex++;
        serialBuffer[serialIndex] = Serial.read();
			}
     if(serialBuffer[serialIndex] == ';'){
        serialIndex = 0;                      //set buffer start index back to 0
				short commandArrayCount=0;
        int count = 1;                        //index start from 1
        char SDdata=serialBuffer[count];
        serialBuffer[count] = 0;              //clear old serial buffer
        while(SDdata != ';')                  //use ';' as sential value
        {
          //Serial.print(SDdata);
          switch(SDdata)
          {
            case 'M':
              commandData[0]=CmdMode_MOVE;
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
            case 'S':
              commandData[0]=CmdMode_DISABLE; //disable all motor
              break;
            case ',':
              commandArrayCount++;
              break;
            default:
              if(commandArrayCount != 0)      //do not edit mode value
              {
                if(SDdata<='9'&&SDdata>='0'&&commandData[0]!=CmdMode_COMMENT)
                  commandData[commandArrayCount]=(SDdata-'0')+commandData[commandArrayCount]*10;
                else
                {
                  Serial.print("unknown= ");
                  Serial.println(SDdata);
                }
              }
              break;
          }
          count++;
          SDdata=serialBuffer[count];
        }
        newCmd.mode=commandData[0];
        newCmd.d1=commandData[1];
        newCmd.d2=commandData[2];
        commandData[0] = 0;
        commandData[1] = 0;
        commandData[2] = 0;
        commandData[3] = 0;//clear old data
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
		}
		
		COMMAND read()
		{
			return newCmd;
		}
};

#endif
