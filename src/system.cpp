#include <unistd.h>
#include <cstddef>
#include <ctime>
#include <set>
#include <string>
#include <vector>

#include <iostream>
#include "linux_parser.h"
#include "process.h"
#include "processor.h"
#include "system.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

// DONE: Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// DONE: Return a container composed of the system's processes
vector<Process>& System::Processes() {
  UpdateProcesses();
  return processes_;
}

// Update existing processes and add new ones
void System::UpdateProcesses() {
  vector<int> pidsUpdate = LinuxParser::Pids();
  vector<Process> processesUpdate{};
  // update existing processes:
  for (Process& process : processes_) {
    // if the process still exists:
    if (std::find(pidsUpdate.begin(), pidsUpdate.end(), process.Pid()) !=
        pidsUpdate.end()) {
      process.UpdateCpuUtilization();
      processesUpdate.emplace_back(process);
    }
  }
  // find new processes
  for (int pid : pidsUpdate) {
    // check if it's a new process:
    if (std::find(Pids().begin(), Pids().end(), pid) == Pids().end()) {
      processesUpdate.emplace_back(Process(pid, StartTime()));
      pids_.emplace_back(pid);
    }
  }
  processes_ = processesUpdate;
  std::sort(processes_.begin(), processes_.end());
  std::reverse(processes_.begin(), processes_.end());
}

// DONE: Return the system's kernel identifier (string)
std::string System::Kernel() {
  if (kernel_ == "") {
    kernel_ = LinuxParser::Kernel();
  }
  return kernel_;
}

// DONE: Return the system's memory utilization
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

// DONE: Return the operating system name
std::string System::OperatingSystem() {
  if (os_ == "") {
    os_ = LinuxParser::OperatingSystem();
  }
  return os_;
}

vector<int>& System::Pids() { return pids_; }

// DONE: Return the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// DONE: Return the total number of processes on the system
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// DONE: Return the number of seconds since the system started running
long int System::UpTime() { return LinuxParser::UpTime(); }

// The timepoint at which the system has started
std::time_t System::StartTime() {
  if (startTime_ == 0) {
    if (UpTime() != 0) {
      std::time_t now = std::time(0);
      startTime_ = now - UpTime();
    } else {
      return 0;
    }
  }
  return startTime_;
}