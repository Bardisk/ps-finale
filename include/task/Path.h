#ifndef PATH
#define PATH

#include <task.h>

namespace Path {
  extern Map::Matrix <bool, 1, 1> abilityMap;
  extern Map::Matrix <Location, 1, 1> fromPoint;
  Direction::DirectionKind getDirectionBFS(Location src, Location dst);
}

#endif