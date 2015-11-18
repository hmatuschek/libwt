#ifndef __FLUC_CPUTIME_HH__
#define __FLUC_CPUTIME_HH__

#include <time.h>
#include <sys/time.h>
#include <list>


namespace wt {

/** A utility class to measure the CPU time used by some algorithms. */
class CpuTime
{
public:
  /** Constructs a new CPU time clock. */
  CpuTime();
  /** Starts the clock. */
  void start();
  /**  Stops the clock and returns the time in seconds. */
  double stop();
  /** Returns the current time of the current clock. */
  double getTime();

protected:
  /** The stack of start times. */
  std::list< clock_t > clocks;
};


/** A utility class to measure the real time used by some algorithms. */
class RealTime
{
public:
  /** Constructs a new CPU time clock. */
  RealTime();

  /** Starts the clock. */
  void start();
  /** Stops the clock and returns the time in seconds. */
  double stop();
  /** Returns the current time of the current clock. */
  double getTime();

protected:
  /** The stack of start times. */
  std::list< struct timeval > clocks;
};


}

#endif // CPUTIME_HH
