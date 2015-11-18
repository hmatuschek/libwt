#include "cputime.hh"

using namespace wt;


CpuTime::CpuTime()
{
  // pass...
}

void
CpuTime::start() {
  this->clocks.push_back(clock());
}

double
CpuTime::stop() {
  // measure time.
  clock_t end = clock();

  // Get time-diff since start:
  double dt = end-this->clocks.back();
  dt /= CLOCKS_PER_SEC;

  // Remove start time from stack:
  this->clocks.pop_back();

  // Return delta t:
  return dt;
}

double
CpuTime::getTime() {
  clock_t end = clock();

  // get diff:
  double dt = end - this->clocks.back();
  dt /= CLOCKS_PER_SEC;

  return dt;
}




RealTime::RealTime()
{
  // pass...
}

void
RealTime::start() {
  struct timeval now; gettimeofday(&now, 0);
  this->clocks.push_back(now);
}

double
RealTime::stop() {
  // measure time.
  struct timeval now; gettimeofday(&now, 0);

  // Get time-diff since start:
  double dt = ((double)(now.tv_usec - this->clocks.back().tv_usec))/1e6;
  dt += now.tv_sec-this->clocks.back().tv_sec;

  // Remove start time from stack:
  this->clocks.pop_back();

  // Return delta t:
  return dt;
}


double
RealTime::getTime()
{
  // measure time.
  struct timeval now; gettimeofday(&now, 0);

  // Get time-diff since start:
  double dt = ((double)(now.tv_usec - this->clocks.back().tv_usec))/1e6;
  dt += now.tv_sec-this->clocks.back().tv_sec;

  // Remove start time from stack:
  this->clocks.pop_back();

  // Return delta t:
  return dt;
}
