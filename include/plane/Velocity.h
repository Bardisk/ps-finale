#ifndef VELOCITY
#define VELOCITY
#include <plane.h>

struct Velocity
{
  double x, y;
  Velocity(double _x=0, double _y=0) : x(_x), y(_y) {}
  friend Position operator * (const double &time, const Velocity &v) {
    return Position(time * v.x, time * v.y);
  }
  friend std::istream& operator>> (std::istream &is, Velocity &velocity) {
    is >> velocity.y >> velocity.x;
    return is;
  }
};
#endif