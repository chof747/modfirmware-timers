#include "state/paused_state.h"
#include "state/running_state.h"

#include "multitimer.h"

using namespace ModFirmWare;

void TimerPausedState::start(MultiTimer *context)
//******************************************************************************
{
  context->resume();
  context->setState(new TimerRunningState());
}

time_t TimerPausedState::pause(MultiTimer *context)
//******************************************************************************
{
  return 0;
}

void TimerPausedState::resume(MultiTimer * context)
//******************************************************************************
{
}
