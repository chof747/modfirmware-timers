#ifndef MODFIRMWARE_PAUSEDSTATE_H
#define MODFIRMWARE_PAUSEDSTATE_H

#include "timerstate.h"

namespace ModFirmWare
{

  class TimerPausedState : public TimerState
  {
    void start(MultiTimer *context) override;
    time_t pause(MultiTimer *context) override;
    void resume(MultiTimer *context) override;
  };

};

#endif //MODFIRMWARE_PAUSEDSTATE_H