#ifndef _BLOCKBUFFER
#define _BLOCKBUFFER

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "Essential.cpp"
#include "Command.cpp"

#define DEBUGGER
#define maxBufferSize 100
#define nextInsertCmdDist 5         //new command's block will be insert after this amount
#define deltaDistance 10            //command will be seperated into blocks by this steps
#define VT 10                       //average timer count


class BLOCKBUFFER
{
	private:
  long ss;
		volatile bool busy = false;
  		BLOCK blockArray[maxBufferSize];
      	BLOCK* newBlock;
		int head;
		int tail;
		ROUTE route;
		CMD cmd;
		POINT machinePrePoint;
		BLOCK* tempBlock;
		void deleteTail();
		void addHead(BLOCK* input);
		int getCapcity();
    int nextIndex(int index);
		void setNextInsertBlock();
		BLOCK* getLastBlock();
	
	public:
		BLOCKBUFFER();
		void init();
		void setPosition(POINT newPoint);
		BLOCK* pop();	
		bool push(BLOCK* newBlock);
		bool notFull();
		void refresh();
		void printInfo();
    void printBlockInfo(BLOCK* block);
    void printPointInfo(POINT* point);
};

#endif
