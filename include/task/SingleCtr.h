#ifndef SINGLECTR
#define SINGLECTR

#include <task.h>

struct SingleCtr {
  SingleCtr *mate;
  Player *me;
  Task *task;
  bool routeLocked;
  
  // Operation operation;
  std::string getDecistion();
};

#endif