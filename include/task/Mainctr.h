#ifndef MAINCTR
#define MAINCTR
#include <task.h>

namespace Mainctr {
  enum StateType{
    MOVE,
    INTERACT,
    WAITFOR,
  }state_p1, state_p2;
  
  Command::CommandType command_p1, command_p2;
  Direction::DirectionKind direction_p1, direction_p2;
  std::string respond() {
    std::stringstream ss;
    ss << Command::encode(command_p1) << " " << Direction::encode(direction_p1) << '\n' << Command::encode(command_p2) << Direction::encode(direction_p2) << '\n';
    return ss.str();
  }

  void getDecision() {
    command_p1 = Command::Move;
    command_p2 = Command::Move;
    direction_p1 = Direction::LU;
    direction_p2 = Direction::N;
  }
}
#endif