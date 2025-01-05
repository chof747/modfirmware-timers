#include "state/done_state.h"
#include "multitimer.h"

using namespace ModFirmWare;

void TimerDoneState::start(MultiTimer *context)
//******************************************************************************
{
}

time_t TimerDoneState::pause(MultiTimer *context)
//******************************************************************************
{
  return 0;
}

void TimerDoneState::resume(MultiTimer * context)
//******************************************************************************
{
}
