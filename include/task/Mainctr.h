#ifndef MAINCTR
#define MAINCTR
#include <task.h>

namespace Mainctr {
  using Direction::DirectionKind;
  using Command::CommandType;
  enum StateType{
    MOVE,
    STOP,
    FETCH_IND,
    CHOP,
    FILL_PLATE,
    FETCH_PLATE,
    FETCH_DIRTY,
    POT,
    PAN,
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

  struct Operation {
    CommandType command;
    DirectionKind direction;
    Operation(CommandType _c, DirectionKind _d) : command(_c), direction(_d) {}
    std::string encode() {
      std::stringstream ss;
      ss << Command::encode(command) << " " << Direction::encode(direction) << '\n';
      return ss.str();
    }
  };

  extern StateType state_p1, state_p2;
  extern StateType nxtstate_p1, nxtstate_p2;

  extern CommandType command_p1, command_p2;
  extern DirectionKind direction_p1, direction_p2;

  inline std::string respond() {
    std::stringstream ss;
    ss << Command::encode(command_p1) << " " << Direction::encode(direction_p1) << '\n' << Command::encode(command_p2) << " " << Direction::encode(direction_p2) << '\n';
    return ss.str();
  }

  void getDecision();
}
#endif