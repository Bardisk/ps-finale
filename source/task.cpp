#include <basics.h>

namespace Mainctr {
  Command::CommandType command_p1, command_p2;
  Direction::DirectionKind direction_p1 = Direction::N, direction_p2 = Direction::N;
  StateType state_p1 = NONE, state_p2 = NONE;
  Location target_p1, target_p2;
  StateType nxtstate_p1 = NONE, nxtstate_p2 = NONE;
  bool firstplate = true;
  int timeout_p1=0, timeout_p2=0;

  void getDecision() {
    Direction::DirectionKind direction;
    Location loc1(Game::playrList[0].position);
    Location loc2(Game::playrList[1].position);
    Log("Current loc1 (%d, %d) loc2 (%d, %d)", loc1.x, loc1.y, loc2.x, loc2.y);
    // Path::abilityMap[loc1] = false;
    Path::abilityMap[loc2] = false;
    Log("Last direction : %s", Direction::encode(direction_p1).c_str());
    Log("Now velocity %.2lf", Game::playrList[0].velocity.abs());

    Location plateLocation;
    Direction::DirectionKind plateDirection;
    
    switch (state_p1)
    {
    case NONE:
      state_p1 = FETCH_IND;
      command_p1 = Command::Move;
      direction_p1 = Direction::N;
      break;
    case FETCH_IND:
      if (loc1 == Game::indDestination) {
        state_p1 = FILL_PLATE;
        command_p1 = Command::Access;
        direction_p1 = Game::indDirection;
      }
      else {
        state_p1 = MOVE;
        target_p1 = Game::indDestination;
        command_p1 = Command::Move;
        direction_p1 = Direction::N;
        nxtstate_p1 = FETCH_IND;
      }
      break;
    case FILL_PLATE:
      if (firstplate) {
        plateLocation = Game::plateDestination;
        plateDirection = Game::plateDirection;
      }
      else {
        plateLocation = Game::cleanDestination;
        plateDirection = Game::cleanDirection;
      }
      if (loc1 == plateLocation) {
        state_p1 = FETCH_PLATE;
        command_p1 = Command::Access;
        direction_p1 = plateDirection;
      }
      else {
        state_p1 = MOVE;
        target_p1 = plateLocation;
        command_p1 = Command::Move;
        direction_p1 = Direction::N;
        nxtstate_p1 = FILL_PLATE;
      }
      break;
    case FETCH_PLATE:
      Log("FETCHING PLATE");
      if (firstplate) {
        firstplate = false;
        plateLocation = Game::plateDestination;
        plateDirection = Game::plateDirection;
      }
      else {
        plateLocation = Game::cleanDestination;
        plateDirection = Game::cleanDirection;
      }
      state_p1 = SURVE;
      command_p1 = Command::Access;
      direction_p1 = plateDirection;
      break;
    case SURVE:
      Log("IM SURVING");
      if (loc1 == Game::surveDestination) {
        state_p1 = WAIT;
        timeout_p1 = 300;
        nxtstate_p1 = FETCH_DIRTY;
        command_p1 = Command::Access;
        direction_p1 = Game::surveDirection;
      }
      else {
        state_p1 = MOVE;
        target_p1 = Game::surveDestination;
        command_p1 = Command::Move;
        direction_p1 = Direction::N;
        nxtstate_p1 = SURVE;
      }
      break;
    case FETCH_DIRTY:
      if (loc1 == Game::dirtyDestination) {
        state_p1 = GO_WASH;
        command_p1 = Command::Access;
        direction_p1 = Game::dirtyDirection;
      }
      else {
        state_p1 = MOVE;
        target_p1 = Game::dirtyDestination;
        command_p1 = Command::Move;
        direction_p1 = Direction::N;
        nxtstate_p1 = FETCH_DIRTY;
      }
      break;
    case GO_WASH:
      if (loc1 == Game::washDestination) {
        Log("GET SINK");
        state_p1 = WASH_PLATE;
        command_p1 = Command::Access;
        direction_p1 = Game::washDirection;
      }
      else {
        state_p1 = MOVE;
        target_p1 = Game::washDestination;
        command_p1 = Command::Move;
        direction_p1 = Direction::N;
        nxtstate_p1 = GO_WASH;
      }
      break;
    case WASH_PLATE:
      state_p1 = WAIT;
      timeout_p1 = 300;
      nxtstate_p1 = FETCH_IND;
      command_p1 = Command::Operate;
      direction_p1 = Game::washDirection;
      break;
    case MOVE:
      Log("MOVING to target (%d, %d)", target_p1.x, target_p1.y);
      //on arrival
      if (loc1 == target_p1) {
        if (Game::playrList[0].velocity.abs() < eps)
          direction = Direction::N;
        else state_p1 = nxtstate_p1;
        break;
      }

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

    case WAIT:
      command_p1 = Command::Move;
      direction_p1 = Direction::N;
      if (!(--timeout_p1)) {
        state_p1 = nxtstate_p1;
      }
      break;
    case CHECK:
      state_p1 = WAIT;
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
  Map::Matrix <bool, 1, 1> abilityMap;
  Map::Matrix <bool, 1, 1> visMap;

  Map::Matrix <Location, 1, 1> fromPoint;
  Map::Matrix <Direction::DirectionKind, 1, 1> fromDirection;

  std::queue <Location> q;
  std::queue <Location> toClear;
  Direction::DirectionKind getDirectionBFS(Location src, Location dst) {
    // Direction::DirectionKind res = Direction::N;
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
        Direction::DirectionKind direction = (Direction::DirectionKind) i;
        //only go straight
        if (Direction::encode(direction).size() > 1)
          continue;
        Location to = now[direction];
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