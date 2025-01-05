#include "state/running_state.h"
#include "multitimer.h"

using namespace ModFirmWare;

#define LOGTAG "multitimer"

void TimerRunningState::start(MultiTimer *context)
//******************************************************************************
{
}

time_t TimerRunningState::pause(MultiTimer *context)
//******************************************************************************
{
#ifdef ESP32
  context->elapsed = MULTITIMER_TIMING() - context->reference;
  context->dettach(context);
#else
#endif

  if (NULL != context->onPause)
  {
    context->onPause(MULTITIMER_TIMING());
  }

  context->logger->info(LOGTAG, "Pausing timer at %d with elapsed time = %d", MULTITIMER_TIMING(), context->elapsed);
  return context->elapsed;

}

void TimerRunningState::resume(MultiTimer * context)
//******************************************************************************
{
}

void TimerRunningState::checkPeriods(MultiTimer* context)
//****************************************************************************************
{
  time_t now = MULTITIMER_TIMING();
  if (now <= context->lastTimeStamp)
  {
    return;
  }

  time_t elapsedTime = now - context->reference;
  time_t remaining = context->duration - context->elapsed;
  time_t remainingToMilestone = (context->nextEventTime + context->milestoneReference) - now;


  if ((0 == (now % context->shortPeriod)) && (NULL != context->onPeriod))
  {
    context->onPeriod(MultiTimer::periodtype_t::SHORT, now, elapsedTime, remaining, remainingToMilestone);
  }

  if ((0 == (now % context->longPeriod)) && (NULL != context->onPeriod))
  {
    context->onPeriod(MultiTimer::periodtype_t::LONG, now, elapsedTime, remaining, remainingToMilestone);
  }
  context->lastTimeStamp = now;
}
