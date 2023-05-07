#ifndef MAINCTR
#define MAINCTR
#include <task.h>

namespace Mainctr {
  enum StateType{
    MOVE,
    FETCH_IND,
    FILL_PLATE,
    FETCH_PLATE,
    FETCH_DIRTY,
    SURVE,
    GO_WASH,
    WASH_PLATE,
    WAIT,
    CHECK,
    NONE,
  };
  enum TargetType {
    Wash,
    Plate,
    Fetch,
    Order,
  };
  extern StateType state_p1, state_p2;
  extern StateType nxtstate_p1, nxtstate_p2;

  extern Command::CommandType command_p1, command_p2;
  extern Direction::DirectionKind direction_p1, direction_p2;
  inline std::string respond() {
    std::stringstream ss;
    ss << Command::encode(command_p1) << " " << Direction::encode(direction_p1) << '\n' << Command::encode(command_p2) << " " << Direction::encode(direction_p2) << '\n';
    return ss.str();
  }

  void getDecision();
}
#endif