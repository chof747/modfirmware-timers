#ifndef MODFIRMWARE_DONESTATE_H
#define MODFIRMWARE_DONESTATE_H

#include "timerstate.h"

namespace ModFirmWare
{

  class TimerDoneState : public TimerState
  {
    void start(MultiTimer *context) override;
    time_t pause(MultiTimer *context) override;
    void resume(MultiTimer *context) override;
  };

};

#endif //MODFIRMWARE_DONESTATE_H