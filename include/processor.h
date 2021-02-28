#ifndef PROCESSOR_H
#define PROCESSOR_H
#include <string>
#include <vector>

class Processor {
 public:
  float Utilization();

 private:
  std::vector<std::string> cpu_{};
  float idle_{0.0};
  float nonidle_{0.0};
};

#endif
