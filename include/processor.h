#ifndef PROCESSOR_H
#define PROCESSOR_H
#include <string>

class Processor {
 public:
  float Utilization();  // TODO: See src/processor.cpp

  // TODO: Declare any necessary private members
 private:
  std::string line;
  std::string cpu;
  long user;
  long nice;
  long system;
  long idle;
  long iowait;
  long irq;
  long sortirq;
  long steal;
  long preActive;
  long preIdle;
};

#endif