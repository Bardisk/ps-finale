#ifndef ROUTE
#define ROUTE

#include <plane.h>

using Direction::DirectionKind;

struct Route
{
  Location src, dst;
  DirectionKind direction;
  int timeout;
  Route(Location _src, Location _dst, DirectionKind _d)
    : src(_src), dst(_dst), direction(_d), timeout(0) {}
  bool isin(const Location &location) const {
    assert(Direction::encode(direction).size() < 2);
    switch (direction)
    {
    case Direction::R:
      return location.x == src.x && (location.y <= dst.y) && (location.y >= src.y);
    case Direction::L:
      return location.x == src.x && (location.y >= dst.y) && (location.y <= src.y);
    case Direction::D:
      return location.y == src.y && (location.x <= dst.x) && (location.x >= src.x);
    case Direction::U:
      return location.y == src.y && (location.x >= dst.x) && (location.x <= src.x);
    default:
      break;
    }
    Log("INVALID ROUTE OF (%d, %d) to (%d, %d)", src.x, src.y, dst.x, dst.y);
    assert(0);
    return false;
  }
  bool compare(const Location &a, const Location &b) const {
    assert(Direction::encode(direction).size() < 2);
    // assert(isin(a));
    // assert(isin(b));
    // assert(a != b);
    switch (direction)
    {
    case Direction::R:
    case Direction::RU:
    case Direction::RD:
      return a.y < b.y;
    case Direction::L:
    case Direction::LU:
    case Direction::LD:
      return a.y > b.y;
    case Direction::D:
      return a.x < b.x;
    case Direction::U:
      return a.x > b.x;
    default:
      break;
    }
    assert(0);
    return false;
  }
  std::optional<Location> intersect(const Route &b) const {
    assert(dst.isvalid());
    assert(b.dst.isvalid());
    assert(src.isvalid());
    assert(b.src.isvalid());
    assert(direction != Direction::N);
    assert(b.direction != Direction::N);
    //only straight route permitted
    assert(Direction::encode(direction).size() < 2);
    //b cannot contain a's source
    assert(!b.isin(src));

    Location intersection;

    //same orientation
    if (Direction::getOrientation(direction) == Direction::getOrientation(b.direction)) {
      //parallel
      if ((Direction::getOrientation(direction) == Direction::Horizonal) ? (src.x != b.src.x) : (src.y != b.src.y))
        return std::nullopt;

      //no intersection
      if (!isin(b.src) && !isin(b.dst))
        return std::nullopt;

      //return the first's upstream
      return compare(b.dst, b.src) ?
        b.dst[Direction::getrev(direction)]
      : b.src[Direction::getrev(direction)];
    }

    if (Direction::getOrientation(direction) == Direction::Horizonal)
      intersection = Location(src.x, b.src.y);
    else intersection = Location(b.src.x, src.y);

    if (isin(intersection) && b.isin(intersection)) {
      assert(intersection != src);
      return intersection[Direction::getrev(direction)];
    }
    else return std::nullopt;
  }
  
};


#endif