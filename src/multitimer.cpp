#include "multitimer.h"
using namespace ModFirmWare;

#define LOGTAG "timer"

// Definition of the static member
MultiTimer::TimerBinding MultiTimer::timerBinding[MAX_ESP32_NUM_TIMERS] = {{NULL, NULL}, {NULL, NULL}, {NULL, NULL}, {NULL, NULL}};

TimerMileStone::TimerMileStone(MultiTimer *timer, time_t time, String name, bool backwards)
    : timer(timer), time(0), name(name)
//****************************************************************************************
{
  this->time = (backwards) ? timer->getDuration() - time : time;
}

MultiTimer::MultiTimer(time_t shortPeriod, time_t longPeriod)
    : duration(0), shortPeriod(shortPeriod), longPeriod(longPeriod), milestones(), currentMilestone(milestones.end()),
      onFinish(NULL), onMileStone(NULL), onPeriod(NULL), onReset(NULL), onPause(NULL),
      state(state_t::INVALID), elapsed(0), reference(0), lastTimeStamp(0), nextEventTime(0)
//****************************************************************************************
{
}

MultiTimer::~MultiTimer()
//****************************************************************************************
{
  clear();
}

bool MultiTimer::setup(Application *app)
//****************************************************************************************
{
  return Component::setup(app);
}

#ifdef ESP32
void MultiTimer::loop()
//****************************************************************************************
{
  if (triggered)
  {
    triggered = false;

    if (milestones.end() != currentMilestone)
    {
#ifdef DEVELOPMENT
      logger->debug(LOGTAG, "Milstone triggered at %d", MULTITIMER_TIMING());
#endif 
      onMileStone(currentMilestone->name.c_str(), MULTITIMER_TIMING() - reference);
      elapsed = currentMilestone->time;
      currentMilestone++;
      timeNextMilesStone();
    }
    else
    {
      elapsed = 0;
      reference = 0;
      currentMilestone = milestones.end();
      clear();
      dettach(this);

      onFinish(MULTITIMER_TIMING() - reference);
      state = state_t::DONE;
    }
  }

  if (RUNNING == state)
  {
    checkPeriods();
  }
}

#else
void MultiTimer::loop()
//****************************************************************************************
{
}
#endif

void MultiTimer::setDuration(time_t duration)
//****************************************************************************************
{
  this->duration = duration;
  state = state_t::READY;
}

time_t MultiTimer::addMileStoneAfterStart(time_t millisAfterStart, String caption)
//****************************************************************************************
{
  return addMilestone(millisAfterStart, caption, false);
}

time_t MultiTimer::addMileStoneBeforeEnd(time_t millisBeforeEnd, String caption)
//****************************************************************************************
{
  return addMilestone(millisBeforeEnd, caption, true);
}

void MultiTimer::setPeriodCallback(PeriodicCallBack cb)
//****************************************************************************************
{
  if (NULL == cb)
    return;
  onPeriod = cb;
}

void MultiTimer::setMileStoneCallback(MileStoneCallBack cb)
//****************************************************************************************
{
  if (NULL == cb)
    return;
  onMileStone = cb;
}

void MultiTimer::setFinishedCallback(SimpleCallBack cb)
//****************************************************************************************
{
  if (NULL == cb)
    return;
  onFinish = cb;
}

void MultiTimer::setResetCallback(SimpleCallBack cb)
//****************************************************************************************
{
  if (NULL == cb)
    return;
  onReset = cb;
}

void MultiTimer::setPauseCallback(SimpleCallBack cb)
//****************************************************************************************
{
  if (NULL == cb)
    return;
  onPause = cb;
}

void MultiTimer::start()
//****************************************************************************************
{
  if (!((state_t::DONE == state) || (state_t::READY == state)))
  {
    logger->warn(LOGTAG, "Trying to start timer but state is not right %d", state);
    return;
  }

  if (state_t::PAUSED == state)
  {
    resume();
  }
  else
  {
    if (0 < milestones.size())
    {
      currentMilestone = milestones.begin();
    }
    else
    {
      currentMilestone = milestones.end();
    }

    elapsed = 0;
    reference = MULTITIMER_TIMING();
    timeNextMilesStone();
  }
}

time_t MultiTimer::pause()
//****************************************************************************************
{
  if (!(state_t::RUNNING == state))
    return 0;

#ifdef ESP32
  elapsed = MULTITIMER_TIMING() - reference;
  dettach(this);
#else
#endif

  state = state_t::PAUSED;
  logger->info(LOGTAG, "Pausing timer at %d with elapsed time = %d", MULTITIMER_TIMING(), elapsed);
  return elapsed;
}

time_t MultiTimer::resume()
//****************************************************************************************
{
  if (!(state_t::PAUSED == state))
    return 0;

  reference = MULTITIMER_TIMING() - elapsed;
  timeNextMilesStone();
  logger->info(LOGTAG, "Resuming timer at %d with next interval in = %d", MULTITIMER_TIMING(), nextEventTime);
  return elapsed;
}

time_t MultiTimer::reset(bool startImmidiately)
//****************************************************************************************
{
#ifdef ESP32
  dettach(this);
#else
#endif

  time_t t = MULTITIMER_TIMING() - reference;

  elapsed = 0;
  reference = 0;
  currentMilestone = milestones.end();

  if (startImmidiately)
  {
    start();
  }
  else
  {
    state = state_t::READY;
  }

  return t;
}

time_t MultiTimer::addMilestone(time_t time, String name, bool backwards)
//****************************************************************************************
{
  TimerMileStone ms(this, time, name, backwards);
  milestones.push_back(ms);
  milestones.sort();

  return ms.time;
}

void MultiTimer::clear()
//****************************************************************************************
{
  milestones.clear();
}

void MultiTimer::timeNextMilesStone()
//****************************************************************************************
{
  nextEventTime =
      ((currentMilestone == milestones.end()) ? duration : currentMilestone->time) - (MULTITIMER_TIMING() - reference);

#ifdef ESP32
  ESP32Timer *t = attach(this, nextEventTime);
  if (NULL == t)
  {
    logger->warn(LOGTAG, "Could not attach timer to a HW timer");
  }
#else
#endif

#ifdef DEVELOPMENT
  if (currentMilestone != milestones.end())
  {
    logger->debug(LOGTAG, "Next milestone is %s at timestamp %d", currentMilestone->name, currentMilestone->time);
  }
  logger->debug(LOGTAG, "Setting timer event in %d at %d (elapsed = %d)", nextEventTime, MULTITIMER_TIMING(), elapsed);
#endif

  state = state_t::RUNNING;
}

void MultiTimer::checkPeriods()
//****************************************************************************************
{
  time_t now = MULTITIMER_TIMING();
  if (now <= lastTimeStamp)
  {
    return;
  }

  if ((0 == (now % shortPeriod)) && (NULL != onPeriod))
  {
    onPeriod(periodtype_t::SHORT, now);
  }

  if ((0 == (now % longPeriod)) && (NULL != onPeriod))
  {
    onPeriod(periodtype_t::LONG, now);
  }
  lastTimeStamp = now;
}

bool MultiTimer::timerHandler(void *timerNo)
//****************************************************************************************
{
  uint32_t ix = reinterpret_cast<uint32_t>(timerNo);
  if ((ix >= MAX_ESP32_NUM_TIMERS) || (NULL == timerBinding[ix].mt))
  {
    return false;
  }

  timerBinding[ix].mt->triggered = true;
  return true;
}

ESP32Timer *MultiTimer::attach(MultiTimer *mt, time_t interval)
//****************************************************************************************
{
  for (int i = 0; i < MAX_ESP32_NUM_TIMERS; ++i)
  {
    if (mt == timerBinding[i].mt)
    {
      ESP32Timer *et = timerBinding[i].esp32Timer;
      et->detachInterrupt();
      timer_isr_callback_remove((timer_group_t)et->getTimerGroup(), (timer_idx_t)et->getTimer());
      et->attachInterruptInterval(interval * 1000, MultiTimer::timerHandler);
      return et;
    }
    else if (NULL == timerBinding[i].mt)
    {
      timerBinding[i].mt = mt;
      timerBinding[i].esp32Timer = new ESP32Timer(i);

      mt->triggered = false;
      timerBinding[i].esp32Timer->attachInterruptInterval(interval * (1000000L / (1 IN_SECONDS)), MultiTimer::timerHandler);
      return timerBinding[i].esp32Timer;
    }
  }

  return NULL;
}

bool MultiTimer::dettach(const MultiTimer *mt)
//****************************************************************************************
{
  for (int i = 0; i < MAX_ESP32_NUM_TIMERS; ++i)
  {
    if (mt == timerBinding[i].mt)
    {
      mt->logger->debug(LOGTAG, "Dettaching from %d", i);
      timerBinding[i].mt = NULL;

      ESP32Timer *et = timerBinding[i].esp32Timer;
      et->detachInterrupt();
      timer_isr_callback_remove((timer_group_t)et->getTimerGroup(), (timer_idx_t)et->getTimer());

      delete timerBinding[i].esp32Timer;
      timerBinding[i].esp32Timer = NULL;
    }

    return true;
  }

  return false;
}