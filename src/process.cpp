#include "process.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// Constructor
Process::Process(int pid) {
  pid_ = pid;
  cpuUtilization_ = LinuxParser::CpuUtilization(pid_);
  command_ = LinuxParser::Command(pid_);
  ram_ = LinuxParser::Ram(pid_);
  user_ = LinuxParser::User(pid_);
  uptime_ = LinuxParser::UpTime(pid_);
}

// TODO: Return this process's ID
int Process::Pid() const { return pid_; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() const { return cpuUtilization_; }

// TODO: Return the command that generated this process
string Process::Command() const { return command_; }

// TODO: Return this process's memory utilization
string Process::Ram() const { return ram_; }

// TODO: Return the user (name) that generated this process
string Process::User() const { return user_; }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() const { return uptime_; }

// Overload "less than" comparison operator for Process objects
// bool Process::operator<(Process const& a[[maybe_unused]]) const { return
// true; }
bool Process::operator<(Process const& a) const {
  return a.CpuUtilization() < this->cpuUtilization_;
}
