#pragma once
#include <plane.h>

struct Position
{
  double x, y;
  Position(double _x=0, double _y=0) : x(_x), y(_y) {}
  Position operator - () const {
    return Position(-x, -y);
  }
  Position& operator += (const Position &b) {
    x += b.x;
    y += b.y;
    return (*this);
  }
  Position& operator -= (const Position &b) {
    return (*this) += (-b);
  }
  Position operator + (const Position &b) const {
    Position res(*this);
    return res += b;
  }
  Position operator - (const Position &b) const {
    return (*this) + (-b);
  }
  friend std::istream& operator>> (std::istream &is, Position &position) {
    is >> position.y >> position.x;
  }
  bool isvalid() const {
    return x >= 0 && x < Map::n && y >= 0 && y < Map::m;
  }
};
