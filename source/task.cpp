#include <basics.h>

namespace Mainctr {
  using Direction::DirectionKind;
  using Command::CommandType;

  CommandType command_p1, command_p2;
  DirectionKind direction_p1 = Direction::N, direction_p2 = Direction::N;
  StateType state_p1 = NONE, state_p2 = NONE;
  Location target_p1, target_p2;

  StateType nxtstate_p1 = NONE, nxtstate_p2 = NONE;
  CommandType nxt_command_p1, nxt_command_p2;
  DirectionKind nxt_direction_p1, nxt_direction_p2;

  bool firstplate = true;
  int timeout_p1=0, timeout_p2=0;

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
    
    switch (state_p1)
    {
    case NONE:
      state_p1 = FETCH_IND;
      command_p1 = Command::Move;
      direction_p1 = Direction::N;
      break;
    
    case FETCH_IND:
      // if (loc1 == Game::indDestination) {
      //   state_p1 = FILL_PLATE;
      //   command_p1 = Command::Access;
      //   direction_p1 = Game::indDirection;
      // }
      // else {
        state_p1 = MOVE;
        target_p1 = Game::indDestination;
        command_p1 = Command::Move;
        direction_p1 = Direction::N;
        nxtstate_p1 = ;
        nxt_command_p1 = Command::Access;

      // }
      break;
    
    case CHOP:

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
    
    case STOP:
      Log("try to stop at (%d, %d)", target_p1.x, target_p1.y);
      
      break;

    case MOVE:
      Log("MOVING to target (%d, %d)", target_p1.x, target_p1.y);
      
      //on arrival
      if (target_p1 == loc1) {
        if (Game::playrList[0].velocity.abs() < eps)
          direction = Direction::N;
        else state_p1 = nxtstate_p1;  //get to nxt state
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