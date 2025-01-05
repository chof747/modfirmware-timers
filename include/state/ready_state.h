#ifndef MODFIRMWARE_READYSTATE_H
#define MODFIRMWARE_READYSTATE_H

#include "timerstate.h"

namespace ModFirmWare
{

  class TimerReadyState : public TimerState
  {
    void start(MultiTimer *context) override;
    time_t pause(MultiTimer *context) override;
    void resume(MultiTimer *context) override;
  };

};

#endif //MODFIRMWARE_READYSTATE_H