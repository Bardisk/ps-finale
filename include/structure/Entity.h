#ifndef ENTITY
#define ENTITY
#include <structure.h>

struct Entity
{
  Position position;
  Container::ContainerKind containerKind;
  //to-do change to integer
  std::vector<std::string> entityList;
  bool processing;
  int currentFrame, totalFrame, sum;
  bool crashed, overcooked;
  friend std::istream& operator >> (std::istream& is, Entity &entity) {
    is >> entity.position;
    entity.processing = false;
    entity.containerKind = Container::None;

    std::string s;
    std::getline(is, s);
    std::stringstream tmp(s);
    bool beforeColon = true;
    while (tmp >> s) {
      if (s == "@") {
        entity.crashed = true;
        continue;
      }
      if (s == "*") {
        entity.overcooked = true;
        continue;
      }
      if (s == ":") {
        beforeColon = false;
        continue;
      }
      if (s == ";") {
        tmp >> entity.currentFrame >> s >> entity.totalFrame;
        entity.processing = true;
        break;
      }
      if (beforeColon) {
        assert(entity.containerKind == Container::None);
        entity.containerKind = Container::decode(s);
        if (entity.containerKind == Container::DirtyPlates) 
          tmp >> entity.sum;
      }
      else entity.entityList.push_back(s);
    }
  }
};

#endif