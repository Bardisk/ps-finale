#include <basics.h>

namespace Mainctr {
  Command::CommandType command_p1, command_p2;
  Direction::DirectionKind direction_p1, direction_p2;
  StateType state_p1, state_p2;

  void getDecision() {
    command_p1 = Command::Move;
    command_p2 = Command::Move;
    direction_p1 = Direction::LU;
    direction_p2 = Direction::N;
  }
}