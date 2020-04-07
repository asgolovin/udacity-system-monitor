#ifndef SYSTEM_H
#define SYSTEM_H

#include <string>
#include <vector>

#include "process.h"
#include "processor.h"

class System {
 public:
  Processor& Cpu();
  std::vector<Process>& Processes();
  void UpdateProcesses();
  std::vector<int>& Pids();
  float MemoryUtilization();
  long UpTime();
  std::time_t StartTime();
  int TotalProcesses();
  int RunningProcesses();
  std::string Kernel();
  std::string OperatingSystem();

  // DONE: Define any necessary private members
 private:
  Processor cpu_ = {};
  std::vector<Process> processes_ = {};
  std::vector<int> pids_ = {};
  std::string os_ = "";
  std::string kernel_ = "";
  std::time_t startTime_{0};
};

#endif