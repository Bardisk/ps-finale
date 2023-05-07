#ifndef PLAYER
#define PLAYER
#include <structure.h>

struct Player
{
  Position position;
  Velocity velocity;
  int resume_time;
  std::optional <Entity> entity;
  friend std::istream& operator >> (std::istream& is, Player &player) {
    is >> player.position >> player.velocity;
    std::string s;
    std::getline(is, s);
    std::stringstream tmp(s);
    tmp >> player.resume_time;
    if ((tmp >> s) && s == ";") {
      Entity entityTmp;
      tmp >> entityTmp;
      player.entity = entityTmp;
    }
    else player.entity = std::nullopt;
    return is;
  }
};
#endif