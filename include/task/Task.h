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
  std::optional<AttentionOrder *> attod;

  enum State {
    PreMove,
    Pick,
    Move,
    Put,
    Operate,
    Wait,
    Done
  }curState;

  std::string encode(State state) {
    switch (state)
    {
    case PreMove:
      return "PREMOVE";
    case Pick:
      return "PICK";
    case Move:
      return "MOVE";
    case Put:
      return "PUT";
    case Operate:
      return "OPERATE";
    case Wait:
      return "WAIT";
    case Done:
      return "DONE";
    default:
      break;
    }
    assert(0);
  }

  // Task() {}
  Task(Location src=Location(-1, -1), Location dst=Location(), bool operate=false, bool pick=false, bool put=false, bool wait=false, int time=0, int prior=0, DirectionKind putD=Direction::N, DirectionKind pickD=Direction::N, DirectionKind operateD=Direction::N) :
    source(src),
    target(dst),
    route(Location(-1, -1), src, Direction::N),
    parent(NULL),
    // isPickPlate(false),
    plate_num(std::nullopt),
    dish(std::nullopt),
    attod(std::nullopt),
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
    if (src != Location(-1, -1) && source != target) {
      if (!StaticPath::routeTable[source][target].has_value()) {
        Log("WARN: NO ROUTE FROM (%d, %d) to (%d, %d)", source.x, source.y, target.x, target.y);
      }
      assert(StaticPath::routeTable[source][target].has_value());
      assert(!StaticPath::routeTable[source][target].value().empty());
    }
    if (needOperation && needPut) {
      assert(operateDirection == putDirection);
    }
    
    // if (source == target) 
    // else route = StaticPath::routeTable[source][target].value()[0];
  }

  //helper functions
  bool usePot();
  bool usePan();
  bool useChop();
  bool isPickDirtyPlate();
  bool isSettlePlate();
  bool isSurve();
  bool isReturn();

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