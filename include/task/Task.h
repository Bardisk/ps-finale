#ifndef TASK
#define TASK
#include <task.h>

using Command::CommandType;
using Direction::DirectionKind;
using Mainctr::Operation;

struct SingleCtr;

struct Task {
  Location source, target;
  //FetchInd (-> Chop) (-> Cook) -> Fill
  //FetchAndServe -> WashPlate (-> MovePlate)
  
  //MoveObject
  //NeedOperation?
  //NeedWait?

  static std::priority_queue<Task> globTaskHeap;
  Route route;
  SingleCtr *parent;

  enum State {
    PreMove,
    Pick,
    Move,
    Put,
    Operate,
    Wait,
    Done
  }curState;

  bool needOperation, needWait;
  int timeout, priority;
  
  bool operator < (const Task& b) const {
    return priority < b.priority;
  }

  Task *next;
  void issueNextTask() const {
    assert(next);
    globTaskHeap.push(*next);
  }

  static int timingCnt;
  static Task *timingTask[128];
  static void timCheck() {
    for (int i = 0; i < timingCnt; i++)
      if (!(--(timingTask[i]->timeout)))
        timingTask[i]->issueNextTask();
    return ;
  }

  std::optional<Operation> getDesicion() {
    switch (curState)
    {
    case PreMove:
      
      break;
    case Put:
      break;
    case Pick:
      break;
    case Wait:
      break;

    case Operate:
      break;
    case Move:
      break;
    default:
      break;
    }
    return std::nullopt;
  }
  
};

#endif