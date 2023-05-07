#include <basics.h>

namespace Mainctr {
  void getDecision() {
    command_p1 = Command::Move;
    command_p2 = Command::Move;
    direction_p1 = Direction::LU;
    direction_p2 = Direction::N;
  }
}