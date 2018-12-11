#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#ifndef _BLOCK
#define _BLOCK
#include "Essential.cpp"
#include "SDcmd.cpp"

#define deltaDistance 10
class ROUTE
{
  public:
  int k;
  long unit;
  POINT goalP;
  POINT preP;
  POINT nowP;
  bool workFlag;
  bool longerSide;
};

struct BLOCK
{
  public:
  //define main speed in proceed
  //int startSpeed;
  //int mainSpeed;
  //int endSpeed;
  //define actual step position for acceration
  //long totalPosition;
  //int position1;
  //int position2;
  volatile int temp;
  volatile int mode;
  volatile int dx,dy;
  volatile int tx,ty;
  //POINT goalP;
  /*
  BLOCK()
  {
    startSpeed = 0;
    mainSpeed = 0;
    endSpeed = 0;
    totalPosition = 0;
    position1 = 0;
    position2 = 0;
  }
*/
  void printInfo()
  {
    Serial.print("BLOCK ");
    Serial.print(" Mode= ");
    Serial.print(mode);
    Serial.print("D= ");
    Serial.print(dx);
    Serial.print(",");
    Serial.print(dy);
    Serial.print(" T= ");
    Serial.print(tx);
    Serial.print(",");
    Serial.println(ty);
    //goalP.printInfo();
  }
  private:
  //define for auto acceration position
  //const double t1 = 0.2;
  //const double t2 = 0.8;
};

class BLOCK_BUFFER
{
  #define maxBufferSize 100
  private:
  volatile bool busy = false;
  int VT = 10;
  BLOCK blockArray[maxBufferSize];
  int head;
  int tail;
  ROUTE route;
  SDcmd sdcmd;
  POINT machinePrePoint;
  BLOCK* tempBlock;
  //COMMAND newCmd;
  //  BLOCK* newBlock;
  //delete a tail-----------------------------------------
  void deleteTail()
  {
    //blockArray[tail].goalP.x=0;
    //blockArray[tail].goalP.x=0;
    if(tail == 0)
      tail = maxBufferSize-1;
    else
      tail--;
  }
  //add a tail-----------------------------------------
  void addHead(BLOCK* input)
  {
    #ifdef DEBUGGER
      Serial.print("add head =");
      input->printInfo();
    #endif
    
    if(head == 0)
      head = maxBufferSize-1;
    else
      head--;
      
    blockArray[head].dx = (*input).dx;
    blockArray[head].dy = (*input).dy;
    blockArray[head].tx = (*input).tx;
    blockArray[head].ty = (*input).ty;
    blockArray[head].mode = (*input).mode;
    
  }
  
  //return capcity-----------------------------------------
  int getCapcity()
  {
    if(tail - head >= 0)
      return tail - head;
    else 
      return tail + maxBufferSize - head;
  }
  public:
  
  BLOCK_BUFFER()
  {
    route.workFlag = false;
    head = tail = 0;
    sdcmd.SDinit();
    machinePrePoint.x = 0;
    machinePrePoint.y = 0;
    //for(int i = 0;i<maxBufferSize;i++)
    //  blockArray[i];// = new BLOCK();
  }
  //get tail-----------------------------------------
  BLOCK* pop()
  {
    //Serial.println(getCapcity());
    //if(getCapcity()<=10)
    //{
    //  loadCommand();
    //}
      
    if(getCapcity()<=0)                   //no block available
    {
      //loadCommand();
      tempBlock->mode=0;
      return tempBlock;
    }
    //BLOCK* tempBlock = &blockArray[tail];
    tempBlock = &blockArray[tail];
    #ifdef DEBUGGER
      Serial.print("\npop block = ");
      tempBlock->printInfo();
    #endif
    deleteTail();
    return tempBlock;
  }
  //push head-----------------------------------------
  bool push(BLOCK* newBlock)
  {
    if(getCapcity()>=maxBufferSize-1)      //full
    {
      #ifdef DEBUGGER
      Serial.println("\nERROR -> try to push in a full block buffer");
      #endif
      return false;
    }
    addHead(newBlock);
    newBlock->mode = 0;
    return true;
  }
  bool notFull()
  {
    if(getCapcity()<50)//maxBufferSize-1
    return true;
    else
    return false;
  }
  void loadCommand()
  {
    if(!busy&&notFull())
    {
      busy = true;
    //Serial.println("load");
    COMMAND newCmd;
    BLOCK* newBlock;
    bool newRoute = false;
    
    int ODx,ODy,OTx,OTy;
    if(!route.workFlag) //have completed a command
    {
      newCmd = sdcmd.getSDcmd();
      //delay(1);
      //Serial.println(newCmd.mode);
      if(newCmd.mode == 1)
      {
        POINT deltaMove;
        deltaMove.x = newCmd.d1 - machinePrePoint.x;
        deltaMove.y = newCmd.d2 - machinePrePoint.y;
        route.goalP.x = deltaMove.x;
        route.goalP.y = deltaMove.y;
        route.nowP.x = 0;
        route.nowP.y = 0;
        route.preP.x = 0;
        route.preP.y = 0;
        route.workFlag = true;
        machinePrePoint.x = newCmd.d1;
        machinePrePoint.y = newCmd.d2;
        newBlock->mode = 1;
        if(abs(route.goalP.x)-abs(route.goalP.y)>0)
          route.longerSide = true;
        else
          route.longerSide = false;
      }
      else  //for other funtion
      {
        if(newCmd.mode == 2)
        {
          newBlock->mode = 2;
          if(newCmd.d1 == 1)
            newBlock->tx = 1;
          else
            newBlock->tx = 0;
          newRoute = true;
        }
      }
    }
    else
    {
      #ifdef DEBUGGER
      Serial.print ("Processing Cmd. nowP= ");
      route.nowP.printInfo();
      #endif
      if(route.longerSide)
      {
        if(abs(route.goalP.x - route.nowP.x)>deltaDistance)
        {
          if(route.goalP.x>0)
          ODx = deltaDistance;
          else
          ODx = -1*deltaDistance;
          route.nowP.x += ODx;
          ODy = (route.goalP.y*route.nowP.x)/route.goalP.x - route.preP.y;
          route.nowP.y += ODy;
        }
        else
        {
          #ifdef DEBUGGER
          Serial.println("\n\nGOEND===========================================");
          #endif
          ODx = route.goalP.x - route.nowP.x;
          ODy = route.goalP.y - route.nowP.y;
          route.workFlag = false;
        }
        route.unit = VT / ODy;
      }
      else
      {
        if(abs(route.goalP.y - route.nowP.y)>deltaDistance)
        {
          if(route.goalP.y>0)
          ODy = deltaDistance;
          else
          ODy = -1*deltaDistance;
          route.nowP.y += ODy;
          ODx = (route.goalP.x*route.nowP.y)/route.goalP.y - route.preP.x;
          route.nowP.x += ODx;
        }
        else
        {
          #ifdef DEBUGGER
          Serial.println("\n\nGOEND===========================================");
          #endif
          ODx = route.goalP.x - route.nowP.x;
          ODy = route.goalP.y - route.nowP.y;
          route.workFlag = false;
        }
        route.unit = VT / ODx;
      }
      
      OTx = abs(route.unit * ODy);
      OTy = abs(route.unit * ODx);
      if(ODx==0)
      {
        OTx = 0;
        OTy = VT;
      }
      if(ODy==0)
      {
        OTy = 0;
        OTx = VT;
      }
      route.preP.x = route.nowP.x;
      route.preP.y = route.nowP.y;
      newRoute = true;
      newBlock->dx = ODx;
      newBlock->dy = ODy;
      newBlock->tx = OTx;
      newBlock->ty = OTy;
      newBlock->mode = 1;
    }
    if(newRoute)
    {
        #ifdef DEBUGGER
          Serial.print("BLOCK->new cmd has been loades suscessful     ");
          (*newBlock).printInfo();
        #endif
      push(newBlock);
      newBlock->mode = 0;
    }
    }
    busy = false;
  }
  #ifdef DEBUGGER
  void printInfo()
  {
    Serial.println("BLOCK=====================");
    Serial.print("head =");
    Serial.print(head);
    Serial.print(",tail =");
    Serial.print(tail);
    Serial.print(",capcity = ");
    Serial.println(getCapcity());
    for(int i = 0;i<maxBufferSize;i++)
      blockArray[i].printInfo();
  }
  #endif
};
#endif
