#pragma once
#include <task.h>

namespace Command {
  enum CommandType {
    Move,
    Operate,
    Access,
  };
  inline std::string encode (CommandType command) {
    switch (command)
    {
    case Move:
      return "Move";
    case Operate:
      return "Interact";
    case Access:
      return "PutOrPick";
    default:
      assert(0);
    }
  }
}