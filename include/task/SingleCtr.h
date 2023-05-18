#ifndef SINGLECTR
#define SINGLECTR

#include <task.h>

struct SingleCtr {
  Player *me, *mate;
  Task task;
  
  // Operation operation;
  std::string getDecistion();
};

#endif