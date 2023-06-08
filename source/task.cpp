#include <basics.h>

int Task::timingCnt = 0;

// #include <functional>
// std::function taskComparer = [] (Task *a, Task* b) -> bool {return a->priority < b->priority;};

using Cooker::CookerKind;

std::priority_queue<Task> Task::globTaskHeap;
std::unordered_set<Task *> taskSet;

using Mainctr::Operation;

Task::State Task::nextState(Task::State now) {
  switch (now)
  {
  case PreMove:
    now = Pick;
    break;
  case Pick:
    now = Move;
    break;
  case Move:
    now = Put;
    break;
  case Put:
    now = Operate;
    break;
  case Operate:
    now = Wait;
    break;
  case Wait:
    now = Done;
    break;
  default:
    assert(0);
    break;
  }
  check:
  switch (now)
  {
  case Pick:
    if (!needPick) {
      now = Move;
      goto check;
    }
    break;
  case Move:
    if (source == target) {
      now = Put;
      goto check;
    }
    break;
  case Put:
    if (!needPut) {
      now = Operate;
      goto check; 
    }
    break;
  case Operate:
    if (!needOperation) {
      now = Wait;
      goto check;
    }
    break;
  case Wait:
    if (!needWait) {
      now = Done;
      goto check;
    }
    break;
  case Done:
    break;
  default:
    assert(0);
    break;
  }
  return now;
}

std::optional<Operation> Task::getDesicion() {
  // if ()
  Location selfLocation = parent->me->position;
  restart:
  switch (curState)
  {
  //Prepare
  case PreMove:
    assert(selfLocation == source);
    curState = nextState(PreMove);
    goto restart;

  //Pick, if necessary
  case Pick:
    operation = Operation(Command::Access, pickDirection);
    curState = nextState(Pick);
    return operation;
    break;

  //Move, until get to the destination
  case Move:
    assert(route.dst.isvalid());
    if (operation.command != Command::Move)
      operation = Operation(Command::Move, Direction::N);
    //on arrival
    if (parent->routeLocked || route.dst == (Location) parent->me->position) {
      //route is end
      if (parent->me->velocity.abs() < eps){
        if (route.dst == target) {
          //next state
          curState = nextState(Move);
        } else {
          //find next Route
          assert(StaticPath::routeTable[route.dst][target].has_value());
          assert(!StaticPath::routeTable[route.dst][target].value().empty());
          route = StaticPath::routeTable[route.dst][target].value()[0];
          if (parent->mate->task->curState == Task::Move) {
            if (parent->mate->routeLocked) {
              if (route.isin((Location) parent->mate->me->position)) {
                parent->routeLocked = true; //mutual dead clock;
                assert(0);
                return Operation(Command::Move, Direction::N);
              }
            }
            else {
              auto intersetRes = route.intersect(parent->mate->task->route);
              if (intersetRes.has_value()) {
                if (intersetRes.value() == route.src) {
                  parent->routeLocked = true;
                  return Operation(Command::Move, Direction::N);
                }
                route = Route(
                  route.src,
                  intersetRes.value(),
                  route.direction
                );
              }
            }
          } else {
            if (route.isin((Location) parent->mate->me->position)) {
              parent->routeLocked = true;
              return Operation(Command::Move, Direction::N);
            }
          }
          parent->routeLocked = false;
        }
        goto restart;
      }
      //try to stop
      else operation = Operation(Command::Move, Direction::N);
    }
    //go on
    else operation = Operation(Command::Move, route.direction);
    return operation; 
    break;

  //Put, if necessary
  case Put:
    operation = Operation(Command::Access, putDirection);
    curState = nextState(Put);
    return operation;
    break;
  
  //Operate, if necessary
  case Operate:
    operation = Operation(Command::Operate, operateDirection);
    curState = nextState(Operate);
    return operation;
    break;

  //Wait, if necessary and until timeout is 0
  case Wait:
    if (--timeout)
      return operation;
    else {
      curState = nextState(Wait);
      return operation;
    }
    break;
  
  //Done
  case Done:
    return std::nullopt;
  default:
    break;
  }
  return std::nullopt;
}

namespace DepGraph {
  CookerKind firstCook, secondCook;
  Task *generateTask(int orderPriority, Order *) {
    return NULL;
  }
}

StaticPath::OrientedLocation Location::getOriented(Direction::OrientationKind orientation) const {
  return StaticPath::OrientedLocation(*this, orientation);
}

namespace StaticPath{
  //Avoid ado
  using Direction::DirectionKind;
  using Map::Matrix;
  using Direction::OrientationKind;

  //More abstraction...
  using RouteList = std::optional<std::vector<Route>>;
  using MartixRoute = Matrix<RouteList, 1, 1>;
  using MartixDistance = Matrix<int, 1, 1>;

  Matrix<MartixRoute, 1, 1> routeTable;
  Matrix<MartixDistance, 1, 1> disTable;
  
  Matrix<bool, 1, 1> abilityMap;
  
  OrientedMatrix<int> visMap;
  std::queue<OrientedLocation> q;
  std::queue<OrientedLocation> toClear;
  OrientedMatrix<OrientedLocation> fromPoint;
  OrientedMatrix<DirectionKind> fromDirection;

  void getSingleSourceBFS(Location src) {
    Log("From (%d, %d)", src.x, src.y);
    if (!Path::abilityMap[src]) return ;
    assert(q.empty());
    while (!q.empty()) q.pop();
    assert(toClear.empty());
    while (!toClear.empty()) toClear.pop();
    assert(!visMap[src.getOriented(Direction::Horizonal)]);
    assert(!visMap[src.getOriented(Direction::Vertical)]);

    //OMG, bullshit!
    visMap[src.getOriented(Direction::Horizonal)]
    = visMap[src.getOriented(Direction::Vertical)] = 1;
    q.push(src.getOriented(Direction::Horizonal));
    q.push(src.getOriented(Direction::Vertical));
    toClear.push(src.getOriented(Direction::Horizonal));
    toClear.push(src.getOriented(Direction::Vertical));
    
    while (!q.empty()) {
      auto now = q.front(); q.pop();
      Log("processing x: %d y: %d, orient: %d", now.location.x, now.location.y, (int) now.orientation);
      for (int i = 0; i < Direction::Direction_NR; i++) {
        DirectionKind direction = (DirectionKind) i;
        //only go straight
        if (!Direction::isStraight(direction))
          continue;
        OrientationKind originalOrientation = now.orientation;
        OrientationKind nowOrientation = Direction::getOrientation(direction);
        //decide next point
        OrientedLocation to = (nowOrientation == originalOrientation) ?
          now.location[direction].getOriented(originalOrientation)    //within layer
        : now.location.getOriented(nowOrientation);                   //across layer
        //if illegel, abort
        if (!to.location.isvalid())
          continue;

        // Log("Get Location (%d %d), orient: %d", to.location.x, to.location.y, (int) to.orientation);

        //if CAN BE visited and is NOT be visited, update
        if (Path::abilityMap[to.location] && !visMap[to]) {
          Log("Pass Location (%d %d), orient: %d Val: %d", to.location.x, to.location.y, (int) to.orientation, visMap[now] + 1);
          fromDirection[to] = direction;
          fromPoint[to] = now;
          visMap[to] = visMap[now] + 1;
          q.push(to);
          toClear.push(to);
        }
      }
    }

    Log("Pass BFS routing");

    for (auto location : visMap) {
      if (location == src) {
        routeTable[src][location] = std::nullopt;
        continue;
      }
      int minCost = 0x3f3f3f3f;
      OrientationKind minOrientation = Direction::None;
      for (int i = 0; i < Direction::StraightCount; i++) {
        OrientationKind orientation = (OrientationKind) i;
        OrientedLocation orientedLocation = location.getOriented(orientation);
        if (visMap[orientedLocation]) {
          if (visMap[orientedLocation] < minCost) {
            minCost = visMap[orientedLocation];
            minOrientation = orientation;
          }
        }
      }
      // Log("MINCOST %d", minCost);
      disTable[src][location] = minCost;
      if (minCost != 0x3f3f3f3f) {
        std::stack<DirectionKind> allRoute;
        OrientedLocation now = location.getOriented(minOrientation);
        assert(now.location != src);
        while (now.location != src) {
          if (now.location != fromPoint[now].location)
            allRoute.push(fromDirection[now]);
          now = fromPoint[now];
        }
        DirectionKind direction = Direction::N;
        Location routeSrc = src, routeTarget = src;
        routeTable[src][location] = std::vector<Route>();
        while (routeTarget != location) {
          //direction changes
          if (direction != Direction::N && direction != allRoute.top()) {
            routeTable[src][location].value().push_back(Route(routeSrc, routeTarget, direction));
            routeSrc = routeTarget;
          }
          direction = allRoute.top();
          allRoute.pop();
          routeTarget = routeTarget[direction];
        }
        assert(routeTable[src][location].has_value());
        routeTable[src][location].value().push_back(Route(routeSrc, routeTarget, direction));
      }
      else {
        routeTable[src][location] = std::nullopt; //easy
        // Log("Unreachable (%d, %d)", location.x, location.y);
      }
    }

    while (!q.empty()) q.pop();
    while (!toClear.empty()) {
      visMap[toClear.front()] = 0;
      toClear.pop();
    }
  }

}

namespace GameCtr {
  SingleCtr player1, player2;
  void init() {
    player1 = (SingleCtr) {
      .num = 1,
      .mate = &player2,
      .me = Game::playrList + 0,
      .task = NULL,
      .routeLocked = false,
      .taskLocked = false
    };
    player2 = (SingleCtr) {
      .num = 2,
      .mate = &player1,
      .me = Game::playrList + 1,
      .task = NULL,
      .routeLocked = false,
      .taskLocked = false
    };
  }
  std::string respond() {
    std::stringstream ss;
    ss << player1.getDecistion() << player2.getDecistion();
    return ss.str();
  }
}

std::string SingleCtr::getDecistion() {
  assert(task->parent == this);

  //No task, find one
  if (!task) {
    task = newTask();
  }
  //being locked
  if (!task) {
    taskLocked = true;
    Log("taskLocked %d", num);
    return Operation(Command::Move, Direction::N).encode();
  }
  taskLocked = false;
  auto taskRes = task->getDesicion();
  //task has ended
  if (!taskRes.has_value()) {
    assert(task->curState == Task::Done);
    //release Task Lock
    if (!task->isReturn()) {
      Map::lockMap[task->target] = false;
      Log("openmap (%d %d) by", task->target.x, task->target.y, num);
    }
    //no following task
    if (!task->next) {
      //update the states
      if (task->dish.has_value()) {
        task->dish.value()->ended = true;
      }
      if (task->attod.has_value()) {
        assert(task->isSettlePlate());
        switch(task->attod.value()->curState) {
          case AttentionOrder::GetPlate: {
            assert(task->attod.value()->getPlateReleased);
            task->attod.value()->curState = AttentionOrder::Prepare;
            break;
          }
          case AttentionOrder::Serve: {
            assert(task->attod.value()->serveReleased);
            task->attod.value()->curState = AttentionOrder::Done;
            *(task->attod.value()) = AttentionOrder(Game::orderList[Game::attentionMaxCnt - 1]);
            break;
          }
          default:
            Log("NOT VALID ATTOD STATUS %d", (int) task->attod.value()->curState);
            assert(0);
        }
      }
      delete task;
      task = newTask();
      if (!task) {
        taskLocked = true;
        Log("taskLocked %d", num);
        return Operation(Command::Move, Direction::N).encode();
      }
      assert(!taskLocked);
      taskRes = task->getDesicion();
    } else {
      Task *nextTask = task->next;
      if (task->usePan() || task->usePot()) {
        taskSet.insert(nextTask);
        delete task;
        task = newTask();
        if (!task) {
          taskLocked = true;
          Log("taskLocked %d", num);
          return Operation(Command::Move, Direction::N).encode();
        }
        taskRes = task->getDesicion();
      }
      else {
        //bug
        if (nextTask->source == task->target && (nextTask->isReturn() || !Map::lockMap[nextTask->target])) {
          delete task;
          task = nextTask;
          assert(nextTask->target.isvalid());
          if (!task->isReturn()) {
            Map::lockMap[task->target] = true;
            Log("lockmap (%d %d) by %d", task->target.x, task->target.y, num);
          }
          assert(!taskLocked);
          taskRes = task->getDesicion();
        }
        else {
          taskSet.insert(nextTask);
          delete task;
          task = newTask();
          if (!task) {
            taskLocked = true;
            Log("taskLocked %d", num);
            return Operation(Command::Move, Direction::N).encode();
          }
          taskRes = task->getDesicion();
        }
      }
    }
  }
  assert(taskRes.has_value());
  return taskRes.value().encode();
}

Task *SingleCtr::newTask() {
  int k = 2;
  //repeat twice
  while (k--) {
    for (int i = 0; i < Game::attentionMaxCnt; i++) {
      switch (Game::attentionOrderList[i].curState)
      {
      case AttentionOrder::Done:
        assert(0);
      case AttentionOrder::Serve:
        if (!Game::attentionOrderList[i].serveReleased) {
          Game::attentionOrderList[i].serveOrder();
        }
        break;
      case AttentionOrder::GetPlate:
        if (!Game::attentionOrderList[i].getPlateReleased) {
          Game::attentionOrderList[i].getPlate();
        }
        break;
      case AttentionOrder::Prepare: {
        bool flag = false;
        for (auto dish : Game::attentionOrderList[i].requirement) {
          if (!dish.released) {
            dish.getTask();
          }
          if (!dish.ended)
            flag = true;
        }
        if (!flag) Game::attentionOrderList[i].curState = AttentionOrder::Serve;
        break;
      }
      default:
        assert(0);
      }
    }
  }
  

  int minDist = 0x3f3f3f3f;
  Task *minTask = NULL;
  for (auto avTask : taskSet) {
    assert(avTask != NULL);
    if (avTask->usePan() && !Game::panTime) continue;
    if (avTask->usePot() && !Game::potTime) continue;
    
    if (avTask->isPickDirtyPlate() && !(Game::poolDirtyCnt == 0 && Game::serveDirtyCnt > 0)) {
      if (Game::inWayPlateCnt.has_value()) continue;
      if (Game::poolDirtyCnt > 0) {
        if (Map::lockMap[avTask->target]) continue;
        if (minDist > 0) {
          minDist = 0;
          minTask = avTask;
        }
      } else if (Game::serveDirtyCnt == 0) {
        continue;
      } else {
        assert(0);
        if (Map::lockMap[avTask->source]) continue;
        if (avTask->source == (Location) me->position && Map::lockMap[avTask->target]) continue;
        if (StaticPath::disTable[(Location) me->position][avTask->source] < minDist) {
          minDist = StaticPath::disTable[(Location) me->position][avTask->source];
          minTask = avTask;
        }
      }
    } else {
      if (Map::lockMap[avTask->source]) continue;
      if (avTask->source == (Location) me->position && Map::lockMap[avTask->target]) continue;
      if (StaticPath::disTable[(Location) me->position][avTask->source] < minDist) {
        minDist = StaticPath::disTable[(Location) me->position][avTask->source];
        minTask = avTask;
      }
    }
  }

  //pick this task
  if (minTask) {
    //fetch this task
    taskSet.erase(minTask);
    if (minTask->isPickDirtyPlate() && !(Game::poolDirtyCnt == 0 && Game::serveDirtyCnt > 0)) {
      if (Game::poolDirtyCnt > 0) {
        assert(minTask->target == Game::washDestination);
        Task *washPlate = new Task(
          (Location) me->position,               //src
          Game::washDestination,        //dst
          true,                        //operate
          false,                         //pick
          false,                          //put
          true,                         //wait
          180,                   //time
          0,                            //prior
          Direction::N,          //putD
          Direction::N,                 //pickD
          Game::washDirection           //operateD
        );
        washPlate->next = minTask->next;
        washPlate->parent = this;
        assert(!Map::lockMap[washPlate->target]);
        Map::lockMap[washPlate->target] = true;
        Log("lockmap (%d %d) by %d", washPlate->target.x, washPlate->target.y, num);
        delete minTask;
        return washPlate;
      } else if (Game::serveDirtyCnt > 0) {
        assert(0);
      } else assert(0);
    }
    assert(!Map::lockMap[minTask->source]);
    if (minTask->source == (Location) me->position) { //no need to move to source
      assert(!Map::lockMap[minTask->target]);
      minTask->parent = this;
      Map::lockMap[minTask->target] = true;
      Log("lockmap (%d %d) by %d", minTask->target.x, minTask->target.y, num);
      return minTask;
    } else { //need to move to source
      //just move
      Task *move = new Task((Location) me->position, minTask->source);
      Map::lockMap[move->target] = true;
      Log("lockmap (%d %d) by %d", move->target.x, move->target.y, num);
      //follow by real task
      move->next = minTask;
      move->parent = this;
      return move;
    }
  }

  return minTask;
}

Dish::Dish(std::string name) {
  ended = false;
  Log("Prepare %s", name.c_str());
  if (Game::ingrdPlace.find(name) == Game::ingrdPlace.end()) {
    assert(Game::madeFrom.find(name) != Game::madeFrom.end());
    if (Game::recipList[Game::madeFrom[name]].kind == Cooker::Chop) {
      chop = Game::recipList[Game::madeFrom[name]].time;
      heat = std::nullopt;
      Log("Dish %s first time: %d", name.c_str(), chop.value());
      name = Game::recipList[Game::madeFrom[name]].nameBefore;
    }
    else {
      chop = std::nullopt;
      heat = std::make_pair(Game::recipList[Game::madeFrom[name]].kind, Game::recipList[Game::madeFrom[name]].time);
      name = Game::recipList[Game::madeFrom[name]].nameBefore;
      if (Game::ingrdPlace.find(name) == Game::ingrdPlace.end()) {
        chop = Game::recipList[Game::madeFrom[name]].time;
        Log("Dish %s first time: %d (under)", name.c_str(), chop.value());
        name = Game::recipList[Game::madeFrom[name]].nameBefore;
      }
    }
  }
  else {
    heat = std::nullopt;
    chop = std::nullopt;
  }

  assert(Game::ingrdPlace[name].size() > 0);
  boxDestination = Game::ingrdDestination[Game::ingrdPlace[name][0]];
  boxDirection = Game::ingrdDirection[Game::ingrdPlace[name][0]];
}

Location Dish::heatDestination() {
  return (heatKind() == Cooker::Pan ? Game::panDestination : Game::potDestination);
}

DirectionKind Dish::heatDirection() {
  return (heatKind() == Cooker::Pan ? Game::panDirection : Game::potDirection);
}

AttentionOrder::~AttentionOrder() {
  for (auto taskpointer : taskPool) {
    delete taskpointer;
  }
}

bool Task::usePan() {
  return needOperation && needPut && !needWait && target == Game::panDestination && putDirection == Game::panDirection;
}

bool Task::usePot() {
  return needOperation && needPut && !needWait && target == Game::potDestination && putDirection == Game::potDirection;
}

bool Task::isPickDirtyPlate() {
  return needOperation && needPick && needWait && needPut && source == Game::dirtyDestination && target == Game::washDestination && putDirection == Game::washDirection && pickDirection == Game::dirtyDirection;
}

bool Task::isReturn() {
  return !needOperation && !needPick && needPut && !needWait && (target == Game::panDestination || target == Game::potDestination) && (putDirection == Game::panDirection || putDirection == Game::potDirection);
}

bool Task::isSettlePlate() {
  return !needOperation && needPick && !needWait && needPut && source == Game::cleanDestination && pickDirection == Game::cleanDirection;
}

//Hardcoder will never lose
Task *Dish::getTask() {
  assert(!released);
  released = true;
  if (chop.has_value()) {
    Task *getIngrdToChop = new Task(
      boxDestination,               //src
      Game::chopDestination,        //dst
      true,                        //operate
      true,                        //pick
      true,                          //put
      true,                         //wait
      chopTime(),                   //time
      0,                            //prior
      Game::chopDirection,          //putD
      boxDirection,                 //pickD
      Game::chopDirection           //operateD
    );
    taskSet.insert(getIngrdToChop);
    if (heat.has_value()) {
      Task *getChopToHeat = new Task (
        Game::chopDestination,               //src
        Game::chopDestination,        //dst
        true,                        //operate
        true,                         //pick
        true,                          //put
        false,                         //wait
        0,                   //time
        0,                            //prior
        heatDirection(),          //putD
        Game::chopDirection,                 //pickD
        heatDirection()           //operateD
      );
      getIngrdToChop->next = getChopToHeat;
      Task *getHeatToPlate = new Task (
        heatDestination(),               //src
        Game::plateDestinationList[parent->targetPlate],        //dst
        false,                        //operate
        true,                         //pick
        true,                          //put
        false,                         //wait
        0,                   //time
        0,                            //prior
        Game::plateDirectionList[parent->targetPlate],          //putD
        heatDirection(),                 //pickD
        Direction::N           //operateD
      );
      getChopToHeat->next = getHeatToPlate;
      Task *returnHeat = new Task(
        Game::plateDestinationList[parent->targetPlate],               //src
        heatDestination(),        //dst
        false,                        //operate
        false,                         //pick
        true,                          //put
        false,                         //wait
        0,                   //time
        0,                            //prior
        heatDirection(),          //putD
        Direction::N,                 //pickD
        Direction::N           //operateD
      );
      returnHeat->dish = this;
      getHeatToPlate->next = returnHeat;
    }
    else {
      Task *getChopToPlate = new Task (
        Game::chopDestination,               //src
        Game::plateDestinationList[parent->targetPlate],        //dst
        false,                        //operate
        true,                         //pick
        true,                          //put
        false,                         //wait
        0,                   //time
        0,                            //prior
        Game::plateDirectionList[parent->targetPlate],          //putD
        Game::chopDirection,                 //pickD
        Direction::N           //operateD
      );
      getChopToPlate->dish = this;
      getIngrdToChop->next = getChopToPlate;
    }
  } else {
    if (heat.has_value()) {
      Task *getIngrdToHeat = new Task(
        boxDestination,               //src
        heatDestination(),        //dst
        true,                        //operate
        true,                         //pick
        true,                          //put
        false,                         //wait
        0,                            //time
        0,                            //prior
        heatDirection(),          //putD
        boxDirection,                 //pickD
        heatDirection()           //operateD
      );
      taskSet.insert(getIngrdToHeat);
      Task *getHeatToPlate = new Task(
        heatDestination(),               //src
        Game::plateDestinationList[parent->targetPlate],        //dst
        false,                        //operate
        true,                         //pick
        true,                          //put
        false,                         //wait
        0,                   //time
        0,                            //prior
        Game::plateDirectionList[parent->targetPlate],          //putD
        heatDirection(),                 //pickD
        Direction::N           //operateD
      );
      getIngrdToHeat->next = getHeatToPlate;
      Task *returnHeat = new Task(
        Game::plateDestinationList[parent->targetPlate],               //src
        heatDestination(),        //dst
        false,                        //operate
        false,                         //pick
        true,                          //put
        false,                         //wait
        0,                   //time
        0,                            //prior
        heatDirection(),          //putD
        Direction::N,                 //pickD
        Direction::N           //operateD
      );
      returnHeat->dish = this;
      getHeatToPlate->next = returnHeat;
    }
    else {
      Task *getIngrdToPlate = new Task(
        boxDestination,               //src
        Game::plateDestinationList[parent->targetPlate],        //dst
        false,                        //operate
        true,                         //pick
        true,                          //put
        false,                         //wait
        0,                            //time
        0,                            //prior
        Game::plateDirectionList[parent->targetPlate],          //putD
        boxDirection,                 //pickD
        Direction::N           //operateD
      );
      getIngrdToPlate->dish = this;
      taskSet.insert(getIngrdToPlate);
    }
  }
  return NULL;
}

Task *AttentionOrder::serveOrder() {
  assert(!serveReleased);
  serveReleased = true;
  assert(curState == Serve);
  Task *servePlate = new Task(
    Game::plateDestinationList[targetPlate],               //src
    Game::surveDestination,        //dst
    false,                        //operate
    true,                         //pick
    true,                          //put
    false,                         //wait
    0,                            //time
    0,                            //prior
    Game::surveDirection,          //putD
    Game::plateDirectionList[targetPlate],                 //pickD
    Direction::N           //operateD
  );
  servePlate->attod = this;
  taskSet.insert(servePlate);
  curState = Done;
  return servePlate;
}

//return a task to getPlate
//if null, plate has been got
std::optional<Task *> AttentionOrder::getPlate() {
  assert(!getPlateReleased);
  getPlateReleased = true;
  assert(curState == GetPlate);
  assert(targetPlate == -1);
  //already have a plate (COLD START)
  if (Game::readyPlateCnt > 0) {
    assert(Game::readyPlateCnt == Game::readyPlates.size());
    Game::readyPlateCnt--;
    // targetPlate = Game::readyPlates.begin();
    // Game::absentPlates.insert(*Game::readyPlates.begin());
    Game::readyPlates.erase(Game::readyPlates.begin());
    curState = Prepare;
    return std::nullopt;
  }
  else {
    assert(Game::absentPlates.size() > 0);
    targetPlate = *Game::absentPlates.begin();
    Game::absentPlates.erase(Game::absentPlates.begin());
    // curState = Prepare;
    // leave the branch to run-time
    // if (pool)
    Task *putPlate = new Task(
      Game::dirtyDestination,               //src
      Game::washDestination,        //dst
      true,                        //operate
      true,                        //pick
      true,                          //put
      true,                         //wait
      180,                          //time
      0,                            //prior
      Game::washDirection,          //putD
      Game::dirtyDirection,                 //pickD
      Game::washDirection           //operateD
    );
    taskSet.insert(putPlate);
    Task *settlePlate = new Task(
      Game::cleanDestination,
      Game::plateDestinationList[targetPlate],
      false,
      true,
      true,
      false,
      0,
      0,
      Game::plateDirectionList[targetPlate],
      Game::cleanDirection,
      Direction::N
    );
    settlePlate->attod = this;
    putPlate->next = settlePlate;
    return putPlate;
  }
}