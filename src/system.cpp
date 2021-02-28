
#include "system.h"

#include <unistd.h>

#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "format.h"
#include "linux_parser.h"
#include "process.h"
#include "processor.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

Processor& System::Cpu() { return cpu_; }

vector<Process>& System::Processes() {
  processes_.clear();
  const vector<int>& pids = LinuxParser::Pids();
  for (int i : pids) {
    Process process(i);
    processes_.push_back(process);
  }
  std::sort(processes_.begin(), processes_.end());
  return processes_;
}

std::string System::Kernel() const { return LinuxParser::Kernel(); }

float System::MemoryUtilization() const {
  return LinuxParser::MemoryUtilization();
}

std::string System::OperatingSystem() const {
  return LinuxParser::OperatingSystem();
}

int System::RunningProcesses() const { return LinuxParser::RunningProcesses(); }

int System::TotalProcesses() const { return LinuxParser::TotalProcesses(); }

long int System::UpTime() const { return LinuxParser::UpTime(); }
