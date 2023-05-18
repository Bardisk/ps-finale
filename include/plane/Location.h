//integer coordinators, for fixed object
#ifndef LOCATION
#define LOCATION
#include <plane.h>

namespace StaticPath {
  struct OrientedLocation;
}

struct Location {
  int x, y;
  Location(int _x=0, int _y=0) : x(_x), y(_y) {}
  Location(Position _p, Velocity velocity=Velocity(), Direction::DirectionKind towards=Direction::N)
    : x(std::round(_p.x - 0.5 - (0.5 - 0.44 * (velocity.abs())  / (MAX_VELOCITY)) * Direction::dx[towards])+eps)
    , y(std::round(_p.y - 0.5 - (0.5 - 0.44 * (velocity.abs())  / ( MAX_VELOCITY)) * Direction::dy[towards])+eps)
  {}
  Location operator [] (Direction::DirectionKind direction) const {
    return Location(
      x + Direction::dx[direction],
      y + Direction::dy[direction]
    );
  }
  Location upstream (Direction::DirectionKind direction) const {
    return Location(
      x - Direction::dx[direction],
      y - Direction::dy[direction]
    );
  }

  StaticPath::OrientedLocation getOriented(Direction::OrientationKind orientation) const;

  Location operator ++(int) {
    if (y < Map::m - 1) {
      y++;
    }
    else {x++; y=0;}
    return *this;
  }
  friend std::istream& operator>> (std::istream &is, Location &location) {
    is >> location.y >> location.x;
    return is;
  }
  operator Position() const {
    return Position(x + 0.5, y + 0.5);
  }
  bool operator == (const Location &b) const {
    return x == b.x && y == b.y;
  }
  bool operator != (const Location &b) const {
    return !((*this) == b);
  }
  bool isvalid() const {
    return x >= 0 && x < Map::n && y >= 0 && y < Map::m;
  }
};
#endif