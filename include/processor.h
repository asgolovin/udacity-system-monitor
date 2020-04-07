#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();

  // DONE: Declare any necessary private members
 private:
  long prevActive_{0}, prevIdle_{0};
};

#endif