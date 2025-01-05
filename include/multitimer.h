#ifndef MULTITIMER_H
#define MULTITIMER_H

#include "modfw_component.h"
#include <Arduino.h>
#include <list>
#include <map>

#include "timerstate.h"

#ifdef ESP32
#include "ESP32TimerInterrupt.hpp"
#endif

namespace ModFirmWare
{
  class MultiTimer;
  class TimerRunningState;

  class TimerMileStone
  {
  public:
    TimerMileStone(MultiTimer *timer) : timer(timer), time(0), name("") {}
    TimerMileStone(MultiTimer *timer, time_t time, String name, bool backwards = false);

    bool operator<(const TimerMileStone &other) const { return this->time < other.time; }

  protected:
    time_t time;
    String name;

    MultiTimer *timer;
    friend MultiTimer;
  };

#ifndef IN_SECONDS
#define IN_SECONDS *1000
#endif

#ifndef MULTITIMER_TIMING
#define MULTITIMER_TIMING() millis()
#endif

  class MultiTimer : public Component
  {

  public:
    enum periodtype_t
    {
      SHORT = 1,
      LONG = 2
    };

    using MileStoneCallBack = std::function<bool(const char *caption, time_t atTime)>;
    using PeriodicCallBack = std::function<bool(const periodtype_t periodType, time_t atTime, time_t elapsed, time_t remaining, time_t toNextMilestone)>;
    using SimpleCallBack = std::function<void(time_t atTime)>;

    MultiTimer(time_t shortPeriod = 1 IN_SECONDS, time_t longPeriod = 60 IN_SECONDS);
    ~MultiTimer();

    bool setup(Application *app);
    void loop();

    void setDuration(time_t duration);
    inline time_t getDuration() { return this->duration; }

    void setPeriodicInterval(periodtype_t periodType, time_t period);
    time_t getPeriodicInterval(periodtype_t periodType) { return (periodtype_t::SHORT == periodType) ? shortPeriod : longPeriod; }

    time_t addMileStoneAfterStart(time_t millisAfterStart, String caption);
    time_t addMileStoneBeforeEnd(time_t millisBeforeEnd, String caption);
    void resetMileStones() { clear(); }

    void setPeriodCallback(PeriodicCallBack cb);
    void setMileStoneCallback(MileStoneCallBack cb);
    void setFinishedCallback(SimpleCallBack cb);
    void setResetCallback(SimpleCallBack cb);
    void setPauseCallback(SimpleCallBack cb);

    void start();
    time_t pause();
    time_t resume();
    time_t reset(bool startImmidiately = false);

    void setState(TimerState *newState);

    void startTimer();

  protected:
    friend class TimerRunningState;

    typedef std::list<TimerMileStone> milestonelist_t;
    typedef milestonelist_t::iterator milestoneit_t;

    time_t duration;
    time_t shortPeriod;
    time_t longPeriod;

    milestonelist_t milestones;
    milestoneit_t currentMilestone;

    PeriodicCallBack onPeriod;
    MileStoneCallBack onMileStone;
    SimpleCallBack onStart;
    SimpleCallBack onFinish;
    SimpleCallBack onReset;
    SimpleCallBack onPause;
    SimpleCallBack onResume;

    time_t addMilestone(time_t time, String name, bool backwards = false);

    time_t elapsed;
    time_t reference;
    time_t lastTimeStamp;

    time_t nextEventTime;
    time_t milestoneReference;

    volatile bool triggered;

    void clear();
    void timeNextMilesStone();
    void checkPeriods();

  private:
    // state_t state;
    TimerState *state;

#ifdef ESP32

    struct TimerBinding
    {
      MultiTimer *mt;
      ESP32Timer *esp32Timer;
    };

    static TimerBinding timerBinding[MAX_ESP32_NUM_TIMERS];
    static bool IRAM_ATTR timerHandler(void *timerNo);
    static ESP32Timer *attach(MultiTimer *mt, time_t interval);
    static bool dettach(const MultiTimer *mt);
#endif
  };

};

#endif // MULTITIMER_H