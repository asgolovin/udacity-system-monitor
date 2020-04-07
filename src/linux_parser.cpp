#include <dirent.h>
#include <unistd.h>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

#include "format.h"
#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// Searches the textfile for the keyword and returns the next word. If there are
// multiple values which belong to the keyword, returns the value on the given
// position (starting with 1).
template <typename Value>
Value LinuxParser::FindKeyValuePair(string filepath, string key, int valuePos) {
  string line, word;
  Value value = Value();
  std::ifstream filestream(filepath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> word) {
        if (word == key) {
          // skip words to get to the right position
          for (int i = 0; i < valuePos; i++) {
            linestream >> value;
          }
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string path = kProcDirectory + kVersionFilename;
  string kernel = LinuxParser::FindKeyValuePair<string>(path, "Linux", 2);
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// DONE: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string path = kProcDirectory + kMeminfoFilename;
  int memTotal = LinuxParser::FindKeyValuePair<int>(path, "MemTotal:");
  int memFree = LinuxParser::FindKeyValuePair<int>(path, "MemFree:");
  if (memTotal && memFree) {
    return ((float)memTotal - memFree) / memTotal;
  }
  return 0.0;
}

// DONE: Read and return the system uptime
long LinuxParser::UpTime() {
  string path = kProcDirectory + kUptimeFilename;
  long uptime = 0, idletime = 0;
  string line;
  std::ifstream filestream(path);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> uptime >> idletime;
  }
  return uptime;
}

// DONE: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  return LinuxParser::ActiveJiffies() + LinuxParser::IdleJiffies();
}

// DONE: Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  string path = kProcDirectory + to_string(pid) + kStatFilename;
  long utime = 0, stime = 0, cutime = 0, cstime = 0;
  string value;
  std::ifstream filestream(path);
  if (filestream.is_open()) {
    for (int i = 0; i < 14; i++) {
      filestream >> value;
    }
    utime = std::stoi(value);
    filestream >> value;
    stime = std::stoi(value);
    filestream >> value;
    cutime = std::stoi(value);
    filestream >> value;
    cstime = std::stoi(value);
  }
  return (utime + stime + cutime + cstime);
}

// DONE: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  vector<string> data = LinuxParser::CpuUtilization();
  if (data.size() > 0) {
    long user = std::stoi(data[1]);
    long nice = std::stoi(data[2]);
    long system = std::stoi(data[3]);
    long irq = std::stoi(data[6]);
    long softirq = std::stoi(data[7]);
    long steal = std::stoi(data[8]);
    return user + nice + system + irq + softirq + steal;
  }
  return 0;
}

// DONE: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<string> data = LinuxParser::CpuUtilization();
  if (data.size() > 0) {
    long idle = std::stoi(data[4]);
    long iowait = std::stoi(data[5]);
    return idle + iowait;
  }
  return 0;
}

// DONE: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  string path = kProcDirectory + kStatFilename;
  std::ifstream filestream(path);
  vector<string> processorData = {};
  if (filestream.is_open()) {
    processorData = {std::istream_iterator<string>{filestream},
                     std::istream_iterator<string>{}};
  }
  return processorData;
}

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string path = kProcDirectory + kStatFilename;
  int totalProcesses = LinuxParser::FindKeyValuePair<int>(path, "processes");
  return totalProcesses;
}

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string path = kProcDirectory + kStatFilename;
  int runningProcesses =
      LinuxParser::FindKeyValuePair<int>(path, "procs_running");
  return runningProcesses;
}

// DONE: Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  string path = kProcDirectory + to_string(pid) + kCmdlineFilename;
  string command = "";
  std::ifstream filestream(path);
  if (filestream.is_open()) {
    std::getline(filestream, command);
  }
  return command;
}

// DONE: Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  string path = kProcDirectory + to_string(pid) + kStatusFilename;
  long RamKB = 0;
  RamKB = LinuxParser::FindKeyValuePair<long>(path, "VmSize:");
  return Format::kB2MB(RamKB);
}

// DONE: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string path = kProcDirectory + to_string(pid) + kStatusFilename;
  string uid = LinuxParser::FindKeyValuePair<string>(path, "Uid:");
  return uid;
}

// DONE: Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string path = kPasswordPath;
  string line;
  string uid = LinuxParser::Uid(pid);
  string user = "";
  std::ifstream filestream(path);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      if (line.find(uid)) {
        std::replace(line.begin(), line.end(), ':', ' ');
        std::istringstream linestream(line);
        linestream >> user;
        return user;
      }
    }
  }
  return user;
}

// DONE: Read and return the time the process started after system boot.
long LinuxParser::StartTime(int pid) {
  string path = kProcDirectory + to_string(pid) + kStatFilename;
  string startTimeTicks;
  int startTimeSeconds = 0;
  std::ifstream filestream(path);
  if (filestream.is_open()) {
    for (int i = 0; i < 22; i++) {
      filestream >> startTimeTicks;
    }
    startTimeSeconds = std::stoi(startTimeTicks) / sysconf(_SC_CLK_TCK);
  }
  return startTimeSeconds;
}