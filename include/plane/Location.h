//integer coordinators, for fixed object
#ifndef LOCATION
#define LOCATION
#include <plane.h>

struct Location {
  int x, y;
  Location(int _x=0, int _y=0) : x(_x), y(_y) {}
  Location operator [] (Direction::DirectionKind direction) const {
    return Location(
      x + Direction::dx[direction],
      y + Direction::dy[direction]
    );
  }
  Location operator ++(int) {
    if (y < Map::m) {
      //newline
      y++; x = 0;
    }
    else x++;
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