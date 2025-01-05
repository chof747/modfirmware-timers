#ifndef MODFIRMWARE_RUNNINGSTATE_H
#define MODFIRMWARE_RUNNINGSTATE_H

#include "timerstate.h"

namespace ModFirmWare
{

  class TimerRunningState : public TimerState
  {
    void start(MultiTimer *context) override;
    time_t pause(MultiTimer *context) override;
    void resume(MultiTimer *context) override;

    void checkPeriods(MultiTimer *context) override;
  };

};

#endif //MODFIRMWARE_RUNNINGSTATE_H