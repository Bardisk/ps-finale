#ifndef DIRECTION
#define DIRECTION
#include <plane.h>

namespace Direction {
  const int dx[] = { 1,  1,  1, -1, -1, -1,  0,  0,  0};
  const int dy[] = {-1,  1,  0, -1,  1,  0, -1,  1,  0};
  const std::string directionName[] = {"LD", "RD", "D", "LU", "RU", "U", "L", "R", " "};
  enum DirectionKind {
    LD = 0,
    RD,
    D,
    LU,
    RU,
    U,
    L,
    R,
    N,
  };
  const int Direction_NR = N;
  inline DirectionKind decode(std::string directionName) {
    if (directionName == "LD")
      return LD;
    if (directionName == "RD")
      return RD;
    if (directionName == "D")
      return D;
    if (directionName == "LU")
      return LU;
    if (directionName == "RU")
      return RU;
    if (directionName == "U")
      return U;
    if (directionName == "L")
      return L;
    if (directionName == "R")
      return R;
    return N;
  }
  inline DirectionKind getrev(DirectionKind direction) {
    switch (direction)
    {
    case LU:
      return RD;
    case L:
      return R;
    case R:
      return L;
    case U:
      return D;
    case D:
      return U;
    case LD:
      return RU;
    case RU:
      return LD;
    case RD:
      return LU; 
    default:
      break;
    }
    return N;
  }
  inline std::string encode(DirectionKind direction) {
    return directionName[direction];
  }
}
#endif