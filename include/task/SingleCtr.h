#ifndef SINGLECTR
#define SINGLECTR

#include <task.h>

struct SingleCtr {
  SingleCtr *mate;
  Player *me;
  Task task;
  
  // Operation operation;
  std::string getDecistion();
};

#endif