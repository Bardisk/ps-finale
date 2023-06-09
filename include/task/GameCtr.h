#ifndef GAMECTR
#define GAMECTR
#include <task.h>

namespace GameCtr {
  using Mainctr::Operation;
  extern SingleCtr player1, player2;
  void init();
  std::string respond();
  extern std::vector<Task *> taskList;
}

#endif