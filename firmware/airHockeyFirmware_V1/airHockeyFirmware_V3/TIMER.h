#ifndef TIMER_h
#define TIMER_h

//#include <inttypes.h>
class TIMER
{
  private:
    unsigned long startTime;
    unsigned long endTime;
    int mode;
    int maxState;
    int nowState;
    bool pinState;
    bool objWorking;
    int pin;
    int thisGroup;
    static bool workingGroup[10];
    static unsigned int timeMap[10][11];
    //static bool working;

  public:
    TIMER(int _mode,int _pin,int _group);
    bool start();
    void update();
};



#endif
