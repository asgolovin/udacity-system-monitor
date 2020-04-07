#ifndef PROCESS_H
#define PROCESS_H

#include <ctime>
#include <string>

/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(int pid, time_t systemStartTime);
  time_t SystemStartTime();
  int Pid() const;
  std::string User();
  std::string Command();
  float CpuUtilization() const;
  void UpdateCpuUtilization();
  std::string Ram();
  long int UpTime();
  float ActiveTime();
  bool operator<(const Process& a) const; 

 private:
  int pid_{0};
  time_t startTime_{0}, systemStartTime_{0};
  std::string user_{""}, command_{""};
  float prevActiveTime_{0.};
  long prevUpTime_{0};
  float cpuUtilization_{0};
};

#endif