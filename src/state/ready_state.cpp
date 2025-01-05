#include "state/ready_state.h"
#include "state/running_state.h"
#include "multitimer.h"

using namespace ModFirmWare;

void TimerReadyState::start(MultiTimer *context)
//******************************************************************************
{
  context->startTimer();
  context->setState(new TimerRunningState());
}

time_t TimerReadyState::pause(MultiTimer *context)
//******************************************************************************
{
  return 0;
}

void TimerReadyState::resume(MultiTimer * context)
//******************************************************************************
{
}
