#include <unistd.h>
#include <cctype>
#include <ctime>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid, time_t systemStartTime) {
  pid_ = pid;
  user_ = LinuxParser::User(pid);
  command_ = LinuxParser::Command(pid);
  systemStartTime_ = systemStartTime;
  UpdateCpuUtilization();
}

time_t Process::SystemStartTime() { return systemStartTime_; }

// DONE: Return this process's ID
int Process::Pid() const { return pid_; }

// return the active time of the process
float Process::ActiveTime() {
  const int pid = Pid();
  return (float)LinuxParser::ActiveJiffies(pid) / sysconf(_SC_CLK_TCK);
}

// DONE: Return this process's CPU utilization
float Process::CpuUtilization() const { return cpuUtilization_; }

// Update this process's CPU utilization
void Process::UpdateCpuUtilization() {
  // first cycle
  if (!(prevActiveTime_ && prevUpTime_)) {
    prevActiveTime_ = ActiveTime();
    prevUpTime_ = UpTime();
    if (prevUpTime_ != 0) {
      cpuUtilization_ = prevActiveTime_ / prevUpTime_;
    }
  }
  // update previous values once every three seconds
  else if (UpTime() - prevUpTime_ > 3) {
    prevActiveTime_ = ActiveTime();
    prevUpTime_ = UpTime();
  }
  float activeTimeDelta = ActiveTime() - prevActiveTime_;
  long upTimeDelta = UpTime() - prevUpTime_;
  if (upTimeDelta != 0) {
    cpuUtilization_ = activeTimeDelta / upTimeDelta;
  }
}

// DONE: Return the command that generated this process
string Process::Command() { return command_; }

// DONE: Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(Pid()); }

// DONE: Return the user (name) that generated this process
string Process::User() { return user_; }

// DONE: Return the age of this process (in seconds)
long int Process::UpTime() {
  if (startTime_ == 0) {
    if (systemStartTime_ != 0) {
      startTime_ = LinuxParser::StartTime(Pid());
    } else {
      return 0;
    }
  }
  return std::time(0) - SystemStartTime() - startTime_;
}

// DONE: Overload the "less than" comparison operator for Process objects
bool Process::operator<(const Process& a) const {
  return this->CpuUtilization() < a.CpuUtilization();
}