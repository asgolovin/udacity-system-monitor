#include "processor.h"
#include "linux_parser.h"

// DONE: Return the aggregate CPU utilization
float Processor::Utilization() {
  // first cycle:
  if (!(prevActive_ && prevIdle_)) {
    prevActive_ = LinuxParser::ActiveJiffies();
    prevIdle_ = LinuxParser::IdleJiffies();
    // avoid division by zero
    if (prevActive_ + prevIdle_ == 0) {
      return 0.;
    }
    return (float)prevActive_ / (prevActive_ + prevIdle_);
  }
  long nowActive = LinuxParser::ActiveJiffies();
  long nowIdle = LinuxParser::IdleJiffies();
  long activeDelta = nowActive - prevActive_;
  long totalDelta = (nowActive + nowIdle) - (prevActive_ + prevIdle_);
  // avoid division by zero
  if (totalDelta == 0) {
    return 0.;
  }
  float utilization = (float)activeDelta / totalDelta;
  prevActive_ = nowActive;
  prevIdle_ = nowIdle;
  return utilization;
}