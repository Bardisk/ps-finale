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
    std::string s;
    is >> player.position >> player.velocity >> player.resume_time >> s;
    if (s == ";") {
      Entity entityTmp;
      is >> entityTmp;
      player.entity = entityTmp;
    }    
    else player.entity = std::nullopt;
  }
};
