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
#define maxBufferSize 60
#define nextInsertCmdDist 5         //new command's block will be insert after this amount
#define deltaDistance 5            //command will be seperated into blocks by this steps
#define VT 10                       //average timer count


class BLOCKBUFFER
{
	private:
  long ss;
		volatile bool busy = false;
  	volatile BLOCK blockArray[maxBufferSize];
    volatile BLOCK newBlock;
		int head;
		int tail;
		volatile ROUTE route;
		volatile CMD cmd;
		POINT machinePrePoint;
		void addHead(BLOCK* input);
		int getCapcity();
    int nextIndex(int index);
		void setNextInsertBlock();
		POINT getLastPoint();
	
	public:
    volatile BLOCK* tempBlock;
		BLOCKBUFFER();
		void init();
    void deleteTail();
		void setPosition(POINT newPoint);
		BLOCK* pop();	
		bool push(BLOCK* newBlock);
		bool notFull();
		void refresh();
		void printInfo();
    void printBlockInfo(const BLOCK* const block);
    void printPointInfo(const POINT* const point);
};

#endif
