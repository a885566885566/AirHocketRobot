#include "BlockBuffer.h"
#define DEBUGGER
//delete a tail-----------------------------------------
void BLOCKBUFFER::deleteTail()
{
    if(tail == 0)
      tail = maxBufferSize-1;
    else
      tail--;
}
		
//add a tail-----------------------------------------
void BLOCKBUFFER::addHead(BLOCK* input)
{   
    if(head == 0)
      	head = maxBufferSize-1;
    else
      	head--;
      
    blockArray[head].dx = (*input).dx;
    blockArray[head].dy = (*input).dy;
    blockArray[head].tx = (*input).tx;
    blockArray[head].ty = (*input).ty;
    blockArray[head].nowPoint.x = (*input).nowPoint.x;
    blockArray[head].nowPoint.y = (*input).nowPoint.y;
    blockArray[head].mode = (*input).mode;
    #ifdef DEBUGGER
    Serial.print("add head= ");
    printBlockInfo(input);
    Serial.print("add head recheck= ");
    printBlockInfo(&blockArray[head]);
    #endif
}

//return capcity-----------------------------------------
int BLOCKBUFFER::getCapcity()
{
    if(tail - head >= 0)
       return tail - head;
    else 
       return tail + maxBufferSize - head;
}

int BLOCKBUFFER::nextIndex(int index)
{
  if(index == 0)
      return maxBufferSize-1;  //make the index in the range of buffer
  else
      return index--;
}

void BLOCKBUFFER::setNextInsertBlock()
{
	int i = nextInsertCmdDist;
	int index=tail;
	while(i>0)
	{
    if(blockArray[nextIndex(index)].mode != CmdMode_MOVE)   //if next block is not in moving mode, use this block immediately
      break;
		index = nextIndex(index);
    if(index==head)					//if the index hit the head, it should be stopped and break
    	break;
		i--;
	}
	head = index;
}

BLOCK* BLOCKBUFFER::getLastBlock()
{
	if(getCapcity()<=0)                   //no block available
  {
    tempBlock->nowPoint.x = machinePrePoint.x;
    tempBlock->nowPoint.y = machinePrePoint.y;
    tempBlock->mode=CmdMode_MOVE;
    return tempBlock;
  }
  else
  {
    blockArray[tail];
    #ifdef DEBUGGER
        Serial.print("\nLast block = ");
        //(*tempBlock).printInfo();
    #endif
    return tempBlock;
  }
    
}

BLOCKBUFFER::BLOCKBUFFER()
{
	cmd.CmdInit();
  route.workFlag = false;
  route.preP.x = 0;
  route.preP.y = 0;
  head = tail = 0;
  busy = false;
}

void BLOCKBUFFER::init()
{
  cmd.CmdInit();
  route.workFlag = false;
  machinePrePoint.x = 0;
  machinePrePoint.y = 0; 
  head = tail = 0;
  busy = false;
}

void BLOCKBUFFER::setPosition(POINT newPoint)
{
  machinePrePoint.x = newPoint.x;
  machinePrePoint.y = newPoint.y; 
}
//get tail-----------------------------------------
BLOCK* BLOCKBUFFER::pop()
{
    if(getCapcity()<=0)                   //no block available
    {
      	tempBlock->mode=0;
      	return tempBlock;
    }
    tempBlock = &blockArray[tail];
    #ifdef DEBUGGER
      Serial.print("\npop block = ");
      printBlockInfo(tempBlock);
    #endif
    deleteTail();
    return tempBlock;
}

//push head-----------------------------------------
bool BLOCKBUFFER::push(BLOCK* newBlock)
{
    if(getCapcity()>=maxBufferSize-1)      //full
    {
	      #ifdef DEBUGGER
	      Serial.println("\nERROR -> try to push in a full block buffer");
	      #endif
	      return false;
    }/*
    BLOCK tempAddedBlock;
    tempAddedBlock.mode = newBlock->mode;
    tempAddedBlock.dx = newBlock->dx;
    tempAddedBlock.dy = newBlock->dy;
    tempAddedBlock.tx = newBlock->tx;
    tempAddedBlock.ty = newBlock->ty;
    tempAddedBlock.nowPoint.x = newBlock->nowPoint.x;
    tempAddedBlock.nowPoint.y = newBlock->nowPoint.y;*/
    addHead(newBlock);
    //newBlock.mode = 0;
    return true;
}


bool BLOCKBUFFER::notFull()
{
    if(getCapcity()<50)//maxBufferSize-1
    	return true;
    else
    	return false;
}

void BLOCKBUFFER::refresh()
{
	if(!busy)								//make sure the process won't be interrupt by itself
	{
		busy = true;
		//deal with new command
		if(cmd.available())
		{
		  COMMAND newCmd = cmd.read();	//get new cmd
      Serial.println("get new cmd");
      cmd.printInfo();
			if(newCmd.mode == CmdMode_MOVE) //only add new cmd when mode == move
			{
		      route.workFlag = true;								//mark that it's a route working
				  setNextInsertBlock();
    			BLOCK* lastBlock = getLastBlock();
    			POINT lastPoint;
				  lastPoint.x = lastBlock->nowPoint.x;					///////////////////////////////////check this
    			lastPoint.y = lastBlock->nowPoint.y;
				
		        route.goalP.x = newCmd.d1 - lastPoint.x;		//last move point
		        route.goalP.y = newCmd.d2 - lastPoint.y;		//cal delta move
		        route.nowP.x = 0;
		        route.nowP.y = 0;
		        route.preP.x = 0;
		        route.preP.y = 0;
		        machinePrePoint.x = newCmd.d1;					//last command point
		        machinePrePoint.y = newCmd.d2;
            /*
            Serial.print("cmdddddddd= ");
            Serial.print(newCmd.mode);
            Serial.print(",");
            Serial.print(newCmd.d1);
            Serial.print(",");
            Serial.println(newCmd.d2);
            */
		        if(abs(route.goalP.x)-abs(route.goalP.y)>0)
		            route.longerSide = true;
		        else
		          	route.longerSide = false;
		        #ifdef DEBUGGER
		          	Serial.println("BLOCK->new cmd has been loaded suscessfully");
		        #endif
			}
			else							//deal with other command
			{
				switch(newCmd.mode)
				{
					case CmdMode_HOME:
						newBlock->mode = CmdMode_HOME;
            Serial.print("newBlockInfo");
            printBlockInfo(newBlock);
						push(newBlock);
						break;
					case CmdMode_DELAY:
						newBlock->mode = CmdMode_DELAY;
						newBlock->dx = newCmd.d1;
						push(newBlock);
						break;
				}
			}
		}
		
		//add blocks to buffer
		if(notFull()&&route.workFlag)
		{
			busy = true;
      int ODx,ODy,OTx,OTy;
			if(route.longerSide)		//longer side is x axis
      { 
        route.longerSide = true;
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
        else            //end not precisely
        {
            #ifdef DEBUGGER
            Serial.println("\n\nGOEND1===========================================");
            #endif
            ODx = route.goalP.x - route.nowP.x;
            ODy = route.goalP.y - route.nowP.y;
            route.workFlag = false;
        }
        route.unit = VT / ODy;
      }
      else
      { 
        	route.longerSide = false;
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
          Serial.println("\n\nGOEND2===========================================");
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
      	//newRoute = true;
     	newBlock->dx = ODx;
     	newBlock->dy = ODy;
     	newBlock->tx = OTx;
    	newBlock->ty = OTy;
    	newBlock->nowPoint.x = route.nowP.x;
    	newBlock->nowPoint.y = route.nowP.y;
    	newBlock->mode = CmdMode_MOVE;
    	Serial.print("data before push= ");
      printBlockInfo(newBlock);
    	push(newBlock);					//push into block buffer
      //newBlock->mode = 0;				//clear old data -> can't clear it because it returned it's reference
		}
		busy = false;						//set the state back to free
	}
}

void BLOCKBUFFER::printPointInfo(POINT* point){
  Serial.print("Point =");
  Serial.print(point->x);
  Serial.print(", ");
  Serial.print(point->y);
}
    
void BLOCKBUFFER::printBlockInfo(BLOCK* block){
  Serial.print("BLOCK info - nowP = ");
  printPointInfo(&(block->nowPoint));
  Serial.print(", mode=");
  Serial.print(block->mode);
  Serial.print(", dx=");
  Serial.print(block->dx);
  Serial.print(", dy=");
  Serial.print(block->dy);
  Serial.print(", tx=");
  Serial.print(block->tx);
  Serial.print(", ty=");
  Serial.println(block->ty);
}
void BLOCKBUFFER::printInfo()
{
    Serial.println("BLOCK=====================");
    Serial.print("head =");
    Serial.print(head);
    Serial.print(",tail =");
    Serial.print(tail);
    Serial.print(",capcity = ");
    Serial.println(getCapcity());
    for(int i = 0;i<maxBufferSize;i++)
      printBlockInfo(&blockArray[i]);
}
