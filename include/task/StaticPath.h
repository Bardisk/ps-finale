#ifndef STATICPATH
#define STATICPATH

#include <task.h>

using Direction::DirectionKind;
using Map::Matrix;
using Direction::OrientationKind;

namespace StaticPath{
  
  using RouteList = std::vector<Route>;
  using MartixRoute = Matrix<RouteList, 1, 1>;

  extern Matrix<MartixRoute, 1, 1> routeTable;
  
  extern Matrix<bool, 1, 1> abilityMap;
  
  struct OrientedLocation {
    Location location;
    OrientationKind orientation;

    OrientedLocation(Location _l, OrientationKind _o)
      : location(_l), orientation(_o) {}
  };
  
  template <typename T>
  struct OrientedMatrix {
    Matrix<T, 1, 1> data[Direction::StraightCount];
    T& operator [] (OrientedLocation orientedLocation) {
      return data[orientedLocation.orientation][orientedLocation.location];
    }
    Matrix<T, 1, 1>::LocalLocation begin() {
      return data[0].begin();
    }
    Matrix<T, 1, 1>::LocalLocation end() {
      return data[0].end();
    }
  };

  extern OrientedMatrix<int> visMap;
  extern std::queue<OrientedLocation> q;
  extern std::queue<OrientedLocation> toClear;

  void getSingleSourceBFS(Location src);

}

#endif