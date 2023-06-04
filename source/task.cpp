#include <basics.h>

int Task::timingCnt = 0;

// #include <functional>
// std::function taskComparer = [] (Task *a, Task* b) -> bool {return a->priority < b->priority;};

using Cooker::CookerKind;

std::priority_queue<Task> Task::globTaskHeap;

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
    if (operation.command != Command::Move)
      operation = Operation(Command::Move, Direction::N);
    //on arrival
    if (route.dst == (Location) parent->me->position) {
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
      // disTable[src][location] = minCost;
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
  std::string respond() {
    std::stringstream ss;
    ss << player1.getDecistion() << player2.getDecistion();
    return ss.str();
  }
}

std::string SingleCtr::getDecistion() {
  // assert(task.parent == this);
  // auto taskRes = task.getDesicion();
  // //need to fetch new task
  // while (!taskRes.has_value()) {
  //   assert(!Task::globTaskHeap.empty());
  //   task = Task::globTaskHeap.top();
  //   Task::globTaskHeap.pop();
  //   task.parent = this;
  //   taskRes = task.getDesicion();
  // }
  // return taskRes.value().encode();
  return "";
}

Dish::Dish(std::string name) {
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

//return a task to getPlate
//if null, plate has been got
std::optional<Task *> AttentionOrder::getPlate() {
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
    // targetPlate = Game::absentPlates.begin();
    // Game::absentPlates.erase(Game::absentPlates.begin());
    // // curState = Prepare;
    // Task *putPlate = new Task(
    //   src = Game::cleanDestination,
    //   dst = Game::plateDestinationList[targetPlate],
    //   operate = true,
    //   pick = true,
    //   put = true,
    //   wait = 
    // );
  }
}