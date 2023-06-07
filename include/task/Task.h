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
  // bool isPickPlate;
  std::optional<int> plate_num;
  std::optional<Dish *> dish;

  enum State {
    PreMove,
    Pick,
    Move,
    Put,
    Operate,
    Wait,
    Done
  }curState;

  // Task() {}
  Task(Location src=Location(-1, -1), Location dst=Location(), bool operate=false, bool pick=false, bool put=false, bool wait=false, int time=0, int prior=0, DirectionKind putD=Direction::N, DirectionKind pickD=Direction::N, DirectionKind operateD=Direction::N) :
    source(src),
    target(dst),
    route(Location(), src, Direction::N),
    parent(NULL),
    // isPickPlate(false),
    plate_num(std::nullopt),
    dish(std::nullopt),
    needOperation(operate),
    needPick(pick),
    needPut(put),
    needWait(wait),
    putDirection(putD),
    pickDirection(pickD),
    operateDirection(operateD),
    timeout(time),
    priority(prior),
    operation(Command::Move, Direction::N),
    next(NULL),
    curState(PreMove)
  {
    if (src != Location(-1, -1)) {
      assert(StaticPath::routeTable[source][target].has_value());
      assert(!StaticPath::routeTable[source][target].value().empty());
    }
    if (needOperation && needPut) {
      assert(operateDirection == putDirection);
    }
    
    // if (source == target) 
    // else route = StaticPath::routeTable[source][target].value()[0];
  }

  bool usePot();
  bool usePan();
  bool isPickDirtyPlate();

  bool needOperation, needPick, needPut, needWait;
  DirectionKind putDirection;
  DirectionKind pickDirection;
  DirectionKind operateDirection;
  int timeout, priority;
  Operation operation;
  
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

  State nextState(State now);
  std::optional<Operation> getDesicion();
  
};

#endif