#include <basics.h>

int Task::timingCnt = 0;

// #include <functional>
// std::function taskComparer = [] (Task *a, Task* b) -> bool {return a->priority < b->priority;};

using Cooker::CookerKind;

std::priority_queue<Task> Task::globTaskHeap;

std::string SingleCtr::getDecistion() {
  assert(task.parent == this);
  auto taskRes = task.getDesicion();
  //need to fetch new task
  while (!taskRes.has_value()) {
    assert(!Task::globTaskHeap.empty());
    task = Task::globTaskHeap.top();
    Task::globTaskHeap.pop();
    task.parent = this;
    taskRes = task.getDesicion();
  }
  return taskRes.value().encode();
}

using Mainctr::Operation;

std::optional<Operation> Task::getDesicion() {
  // if ()
  restart:
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
    if (operation.command != Command::Move)
      operation = Operation(Command::Move, Direction::N);
    //on arrival
    if (route.dst == parent->me->position) {
      //stopped
      if (parent->me->velocity.abs() < eps){
        if (route.dst == target) {
          if (needPut) {
            curState = Put;
            goto restart;
          }
          if (needOperation){
            curState = Operate;
            goto restart;
          }
          if (needWait) {
            curState = Wait;
            goto restart;
          }
        } else {
          assert(StaticPath::routeTable[route.dst][target].has_value());
          assert(!StaticPath::routeTable[route.dst][target].value().empty());
          route = StaticPath::routeTable[route.dst][target].value()[0];
          goto restart;
        }
        //find next Route
      }
      else operation = Operation(Command::Move, Direction::N);
    }
    return operation; 
    break;
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

namespace Mainctr {
  using Direction::DirectionKind;
  using Command::CommandType;

  CommandType command_p1, command_p2;
  DirectionKind direction_p1 = Direction::N, direction_p2 = Direction::N;
  StateType state_p1 = NONE, state_p2 = NONE;
  Location target_p1, target_p2;

  StateType nxtstate_p1 = NONE, nxtstate_p2 = NONE;
  
  CommandType nxt_command_p1 = Command::Move, nxt_command_p2 = Command::Move;
  DirectionKind nxt_direction_p1 = Direction::N, nxt_direction_p2 = Direction::N;

  CommandType wait_nxt_command_p1 = Command::Move, wait_nxt_command_p2 = Command::Move;
  DirectionKind wait_nxt_direction_p1 = Direction::N, wait_nxt_direction_p2 = Direction::N;

  std::string needed_p1;

  Cooker::CookerKind firstCook, secondCook;
  int firstTime_p1, secondTime_p1, firstTime_p2, secondTime_p2;

  bool firstplate_p1 = true, firstplate_p2 = false;
  int timeout_p1=0, timeout_p2=0;
  bool cooked = false;
  // int nxt_timeout_p1=0, nxt_timeout_p2=0;
  Order order_p1;
  int curorder_cnt_p1 = 0;

  void getDecision() {
    DirectionKind direction;
    
    //get location
    Location loc1(Game::playrList[0].position, Game::playrList[0].velocity, command_p1 == Command::Move ? direction_p1 : Direction::N);
    Location loc2(Game::playrList[1].position, Game::playrList[1].velocity, command_p2 == Command::Move ? direction_p2 : Direction::N);
    // #define pos1 Game::playrList[0].position
    // #define pos2 Game::playrList[1].position

    Log("Current loc1 (%d, %d) loc2 (%d, %d)", loc1.x, loc1.y, loc2.x, loc2.y);
    // Path::abilityMap[loc1] = false;
    Path::abilityMap[loc2] = false;
    Log("Last direction : %s", Direction::encode(direction_p1).c_str());
    Log("Now velocity %.2lf", Game::playrList[0].velocity.abs());

    Location plateLocation;
    DirectionKind plateDirection;
    
    using Cooker::CookerKind;

    switch (state_p1)
    {
    case NONE:
      order_p1 = Game::orderList[0];
      curorder_cnt_p1 = 0;
      state_p1 = FETCH_IND;
      command_p1 = Command::Move;
      direction_p1 = Direction::N;
      break;
    
    case FETCH_IND:
      state_p1 = MOVE;
      cooked = false;
      needed_p1 = order_p1.requirement[curorder_cnt_p1++];
      if (Game::ingrdPlace.find(needed_p1) == Game::ingrdPlace.end()) {
        assert(Game::madeFrom.find(needed_p1) != Game::madeFrom.end());
        if (Game::recipList[Game::madeFrom[needed_p1]].kind == Cooker::Chop) {
          firstCook = Cooker::Chop;
          firstTime_p1 = Game::recipList[Game::madeFrom[needed_p1]].time;
          Log("FIRSTTIME: %d", firstTime_p1);
          secondCook = Cooker::None;
          needed_p1 = Game::recipList[Game::madeFrom[needed_p1]].nameBefore;
        }
        else {
          firstCook = Cooker::None;
          secondCook = Game::recipList[Game::madeFrom[needed_p1]].kind;
          secondTime_p1 = Game::recipList[Game::madeFrom[needed_p1]].time;
          needed_p1 = Game::recipList[Game::madeFrom[needed_p1]].nameBefore;
          if (Game::ingrdPlace.find(needed_p1) == Game::ingrdPlace.end()) {
            firstCook = Cooker::Chop;
            firstTime_p1 = Game::recipList[Game::madeFrom[needed_p1]].time;
            Log("FIRSTTIME UNDER: %d", firstTime_p1);
            needed_p1 = Game::recipList[Game::madeFrom[needed_p1]].nameBefore;
          }
        }
        // assert(0);
      }
      else {
        firstCook = Cooker::None;
        secondCook = Cooker::None;
      }

      assert(Game::ingrdPlace[needed_p1].size() > 0);
      target_p1 = Game::ingrdDestination[Game::ingrdPlace[needed_p1][0]];
      nxt_command_p1 = Command::Access;
      nxt_direction_p1 = Game::ingrdDirection[Game::ingrdPlace[needed_p1][0]];
      nxtstate_p1 = firstCook == Cooker::None ? FILL_PLATE : CHOP;

      command_p1 = Command::Move;
      direction_p1 = Direction::N;
        
      break;
    
    case CHOP:
      state_p1 = MOVE;
      target_p1 = Game::chopDestination;
      nxtstate_p1 = CHOPPING;
      nxt_command_p1 = Command::Access;
      nxt_direction_p1 = Game::chopDirection;

      command_p1 = Command::Move;
      direction_p1 = Direction::N;
      break;

    case CHOPPING:
      state_p1 = WAIT;
      timeout_p1 = firstTime_p1;
      Log("FIRSTTIME WITHIN: %d", firstTime_p1);

      nxtstate_p1 = FILL_PLATE;

      nxt_command_p1 = Command::Access;
      nxt_direction_p1 = Game::chopDirection;

      command_p1 = Command::Operate;
      direction_p1 = Game::chopDirection;
      break;

    case FILL_PLATE:
      if (firstplate_p1) {
        plateLocation = Game::plateDestination;
        plateDirection = Game::plateDirection;
      }
      else {
        plateLocation = Game::cleanDestination;
        plateDirection = Game::cleanDirection;
      }

      state_p1 = MOVE;

      if (cooked || secondCook == Cooker::None) {
        target_p1 = plateLocation;
        nxt_command_p1 = Command::Access;
        nxt_direction_p1 = plateDirection;
        nxtstate_p1 = cooked ? RETURN_COOK : FETCH_PLATE;
      }
      else {
        if (secondCook == Cooker::Pan) {
          target_p1 = Game::panDestination;
          nxt_command_p1 = Command::Access;
          nxt_direction_p1 = Game::panDirection;
          nxtstate_p1 = COOK;
        } else {
          target_p1 = Game::potDestination;
          nxt_command_p1 = Command::Access;
          nxt_direction_p1 = Game::potDirection;
          nxtstate_p1 = COOK;
        }
      }
      
      command_p1 = Command::Move;
      direction_p1 = Direction::N;

      break;
    
    case TO_PLATE:
      if (curorder_cnt_p1 < order_p1.requirement.size()) {
        Log("%d %d NOT SATIS", curorder_cnt_p1, order_p1.requirement.size());
        command_p1 = Command::Move;
        direction_p1 = Direction::N;
        state_p1 = FETCH_IND;
        break;
      }
      if (firstplate_p1) {
        plateLocation = Game::plateDestination;
        plateDirection = Game::plateDirection;
      }
      else {
        plateLocation = Game::cleanDestination;
        plateDirection = Game::cleanDirection;
      }
      state_p1 = MOVE;

      target_p1 = plateLocation;
      nxt_command_p1 = Command::Move;
      nxt_direction_p1 = Direction::N;
      nxtstate_p1 = FETCH_PLATE;

      command_p1 = Command::Move;
      direction_p1 = Direction::N;
      break;

    case FETCH_PLATE:
      if (curorder_cnt_p1 < order_p1.requirement.size()) {
        Log("FOR NEW DISH");
        command_p1 = Command::Move;
        direction_p1 = Direction::N;
        state_p1 = FETCH_IND;
        break;
      }
      Log("FETCHING PLATE");
      if (firstplate_p1) {
        firstplate_p1 = false;
        plateLocation = Game::plateDestination;
        plateDirection = Game::plateDirection;
      }
      else {
        plateLocation = Game::cleanDestination;
        plateDirection = Game::cleanDirection;
      }
      state_p1 = MOVE;

      // if (secondCook == Cooker::None) {
      target_p1 = Game::surveDestination;
      nxt_command_p1 = Command::Access;
      nxt_direction_p1 = Game::surveDirection;
      nxtstate_p1 = FETCH_DIRTY;
      // }
      // else {
      //   if (secondCook == Cooker::Pan) {
      //     target_p1 = Game::panDestination;
      //     nxt_command_p1 = Command::Access;
      //     nxt_direction_p1 = Game::panDirection;
      //     nxtstate_p1 = COOK;
      //   } else {
      //     target_p1 = Game::potDestination;
      //     nxt_command_p1 = Command::Access;
      //     nxt_direction_p1 = Game::potDirection;
      //     nxtstate_p1 = COOK;
      //   }
      // }

      command_p1 = Command::Access;
      direction_p1 = plateDirection;
      break;
    
    case COOK:
      command_p1 = Command::Operate;
      direction_p1 = (secondCook == Cooker::Pan ? Game::panDirection : Game::potDirection);

      state_p1 = WAIT;
      timeout_p1 = secondTime_p1;
      nxt_command_p1 = Command::Access;
      nxt_direction_p1 = (secondCook == Cooker::Pan ? Game::panDirection : Game::potDirection);

      nxtstate_p1 = FILL_PLATE;
      cooked = true;
      break;
    
    case RETURN_COOK:
      command_p1 = Command::Move;
      direction_p1 = Direction::N;

      state_p1 = MOVE;
      target_p1 = (secondCook == Cooker::Pan ? Game::panDestination : Game::potDestination);
      nxt_command_p1 = Command::Access;
      nxt_direction_p1 = (secondCook == Cooker::Pan ? Game::panDirection : Game::potDirection);
      nxtstate_p1 = TO_PLATE;

      cooked = false;
      break;

    case SURVE:
      state_p1 = MOVE;
      target_p1 = Game::surveDestination;
      nxt_command_p1 = Command::Access;
      nxt_direction_p1 = Game::surveDirection;
      nxtstate_p1 = FETCH_DIRTY;

      command_p1 = Command::Move;
      direction_p1 = Direction::N;
      break;
    case FETCH_DIRTY:
      state_p1 = MOVE;

      target_p1 = Game::dirtyDestination;
      nxtstate_p1 = GO_WASH;
      nxt_command_p1 = Command::Access;
      nxt_direction_p1 = Game::dirtyDirection;

      timeout_p1 = 300;
      wait_nxt_command_p1 = Command::Access;
      wait_nxt_direction_p1 = Game::dirtyDirection;

      command_p1 = Command::Move;
      direction_p1 = Direction::N;
      break;

    case GO_WASH:
      state_p1 = MOVE;

      target_p1 = Game::washDestination;
      nxtstate_p1 = WASH_PLATE;
      nxt_command_p1 = Command::Access;
      nxt_direction_p1 = Game::washDirection;

      // timeout_p1 = 200;
      // wait_nxt_command_p1 = Command::Move;
      // wait_nxt_direction_p1 = Direction::N;

      command_p1 = Command::Move;
      direction_p1 = Direction::N;

      break;
    
    case WASH_PLATE:
      state_p1 = WAIT;
      timeout_p1 = 200;

      nxtstate_p1 = NONE;

      nxt_command_p1 = Command::Access;
      nxt_direction_p1 = Direction::N;

      command_p1 = Command::Operate;
      direction_p1 = Game::washDirection;
      
      break;
    
    assert(0);

    case WAIT:
      command_p1 = Command::Move;
      direction_p1 = Direction::N;
      if (!(--timeout_p1)) {
        Log("TIME UP");
        state_p1 = nxtstate_p1;
        Log("%d", (int) state_p1);
        command_p1 = nxt_command_p1;
        direction_p1 = nxt_direction_p1;
        nxt_command_p1 = Command::Move;
        nxt_direction_p1 = Direction::N;
      }
      break;

    case MOVE:
      Log("MOVING to target (%d, %d)", target_p1.x, target_p1.y);

      if (command_p1 != Command::Move)
        command_p1 = Command::Move, direction_p1 = Direction::N;

      //on arrival
      if (target_p1 == loc1) {
        if (Game::playrList[0].velocity.abs() > eps)
          direction_p1 = Direction::N;
        else { //get to nxt state
          command_p1 = nxt_command_p1;
          direction_p1 = nxt_direction_p1;
          if (!timeout_p1) {
            state_p1 = nxtstate_p1;
            nxt_command_p1 = Command::Move;
            nxt_direction_p1 = Direction::N;
          } else {
            state_p1 = WAIT;
            nxt_command_p1 = wait_nxt_command_p1;
            nxt_direction_p1 = wait_nxt_direction_p1;
            wait_nxt_command_p1 = Command::Move;
            wait_nxt_direction_p1 = Direction::N;
          }
        }
        break;
      }

      Log("NOT ARRIVE");

      //get direction
      direction = Path::getDirectionBFS(loc1, target_p1);
      if (direction != direction_p1) {
        //stop firstly
        if (Game::playrList[0].velocity.abs() > eps)
          direction_p1 = Direction::N;
        else direction_p1 = direction;
      }
      else direction_p1 = direction;
      break;

    //no use for now
    case CHECK:
      state_p1 = WAIT;
      break;
    
    //no use for now
    case STOP:
      Log("try to stop at (%d, %d)", target_p1.x, target_p1.y);
      break;

    default:
      break;
    }

    // command_p1 = Command::Move;
    // direction_p1 = Direction::LU;

    command_p2 = Command::Move;
    direction_p2 = Direction::N;
  }
}

namespace Path {
  using Direction::DirectionKind;

  Map::Matrix <bool, 1, 1> abilityMap;
  Map::Matrix <bool, 1, 1> visMap;

  Map::Matrix <Location, 1, 1> fromPoint;
  Map::Matrix <DirectionKind, 1, 1> fromDirection;

  std::queue <Location> q;
  std::queue <Location> toClear;
  DirectionKind getDirectionBFS(Location src, Location dst) {
    // DirectionKind res = Direction::N;
    assert(q.empty());
    while (!q.empty()) q.pop();
    assert(toClear.empty());
    while (!toClear.empty()) toClear.pop();
    assert(!visMap[src]);
    assert(!visMap[dst]);

    if (src == dst)
      return Direction::N;
    visMap[src] = true;
    q.push(src);
    toClear.push(src);
    
    while (!q.empty()) {
      Location now = q.front(); q.pop();
      for (int i = 0; i < Direction::Direction_NR; i++) {
        DirectionKind direction = (DirectionKind) i;
        //only go straight
        if (Direction::encode(direction).size() > 1)
          continue;
        Location to = now[direction];
        if (!to.isvalid())
          continue;
        if (abilityMap[to] && !visMap[to]) {
          fromDirection[to] = direction;
          fromPoint[to] = now;
          visMap[to] = true;
          if (to == dst)
            goto end;
          q.push(to);
          toClear.push(to);
        }
      }
    }
    end:
    while (!q.empty()) q.pop();
    while (!toClear.empty()) {
      visMap[toClear.front()] = false;
      assert(toClear.front() != dst);
      toClear.pop();
    }
    if (!visMap[dst])
      return Direction::N;
    visMap[dst] = false;
    Location nxt = dst;
    while (fromPoint[nxt] != src)
      nxt = fromPoint[nxt];
    return fromDirection[nxt];
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

  Matrix<MartixRoute, 1, 1> routeTable;
  
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