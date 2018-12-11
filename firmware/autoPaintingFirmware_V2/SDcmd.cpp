/*
 * SD card pin 
 * MOSI  51
 * MISO  50
 * SCK   52
 * SS    53
 */
 
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#ifndef __SDCMD
#define __SDCMD

#include <SPI.h>
#include <SD.h>
#include "Essential.cpp"

class SDcmd
{
  private:
  File SD_File;
  long commandData[3];
  
  public:
  COMMAND result;
  void SDinit()
  {
    pinMode(53,OUTPUT);
    digitalWrite(53,LOW);
    if (!SD.begin(53))
    {
      #ifdef DEBUGGER
        Serial.println("initialization failed!");
      #endif
      //warningSignal(1);
    }
    else
    {
      #ifdef DEBUGGER
        Serial.println("initialization done.");
      #endif
        SD_File = SD.open("data.txt");
    } 
  } 
   COMMAND getSDcmd()
    {
      if (SD_File) 
      {
        if(SD_File.available())
        {
          //warningSignal(2);
          short commandArrayCount=0;
          char SDdata=99;
          while(SDdata!='\n')
          {
            SDdata=SD_File.read();
            switch(SDdata)
            {
              case 'M':
                commandData[0]=1;
                break;
              case 'A':
                commandData[0]=2;
                break;
              case 'H':
                commandData[0]=3;
                break;
              case 'D':
                commandData[0]=4;//delay
                break;
              case 'B':
                commandData[0]=5;
                break;
              case 'V':
                commandData[0]=6; //set Speed
                break;
              case 'S':
                commandData[0]=7; //disable all motor
                break;
              case 'C':
                commandData[0]=9;//comment mode
                commandArrayCount++;
                break;
              case ',':
                commandArrayCount++;
                break;
              default:
                if(SDdata<=58&&SDdata>=48&&commandData[0]!=3)
                  commandData[commandArrayCount]=(SDdata-48)+commandData[commandArrayCount]*10;
                break;
            }
          }
          
          #ifdef DEBUGGER
            Serial.print("commnd=");//++","+commandData[1]+","+commandData[2]
            Serial.print(commandData[0]);
            Serial.print(",");
            Serial.print(commandData[1]);
            Serial.print(",");
            Serial.println(commandData[2]);
          #endif
  
          //COMMAND result;
          result.mode = commandData[0];
          result.d1 = commandData[1];
          result.d2 = commandData[2];
          
          for(int i=0;i<3;i++)
            commandData[i]=0;//clear data
            
          return result;
        }
     } 
     else
     Serial.println("SD");
  }
};

#endif
