#ifndef MODFIRMWARE_TIMERSTATE_H
#define MODFIRMWARE_TIMERSTATE_H

#include <Arduino.h>

namespace ModFirmWare
{
  class MultiTimer;

  class TimerState
  {
  public:
    virtual ~TimerState() {}
    virtual void start(MultiTimer *context) = 0;
    virtual time_t pause(MultiTimer *context) = 0;
    virtual void resume(MultiTimer *context) = 0;

    virtual void checkPeriods(MultiTimer* context) {}
  };

};

#endif // MODFIRMWARE_TIMERSTATE_H