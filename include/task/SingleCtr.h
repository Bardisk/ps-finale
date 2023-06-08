#ifndef SINGLECTR
#define SINGLECTR

#include <task.h>

struct SingleCtr {
  int num;
  SingleCtr *mate;
  Player *me;
  Task *task;
  bool routeLocked;
  bool taskLocked;
  
  // Operation operation;
  std::string getDecistion();

  //Find a new Task
  Task *newTask();
};

#endif