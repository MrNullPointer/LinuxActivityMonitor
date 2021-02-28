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
  float MemoryUtilization() const;
  long int UpTime() const;
  int TotalProcesses() const;
  int RunningProcesses() const;
  std::string Kernel() const;
  std::string OperatingSystem() const;

 private:
  Processor cpu_ = {};
  std::vector<Process> processes_ = {};
};

#endif
