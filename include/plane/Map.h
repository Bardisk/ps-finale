#ifndef MAP
#define MAP
#include <plane.h>

namespace Map{
  template <typename T, int N_SCALE, int M_SCALE>
  struct Matrix {
    T data[N_MAX * N_SCALE][M_MAX * M_SCALE];
    // Matrix() {}
    struct LocalLocation : public Location {
      // Matrix *father;
      LocalLocation(int _x=0, int _y=0) : Location(_x, _y) {}
      LocalLocation(Location location) : Location(location) {} 
      LocalLocation operator ++(int) {
        if (y < m * M_SCALE - 1) y++;
        else x++, y=0;
        return *this;
      }
      LocalLocation& operator ++() {
        if (y < m * M_SCALE - 1) y++;
        else x++, y=0;
        return *this;
      }
      LocalLocation operator *() const { return (*this); }
      bool isvalid() const {
        return x >= 0 && x < n * N_SCALE && y >= 0 && y < m * M_SCALE;
      }
    };
    T& operator [](LocalLocation localLocation) {
      if (!localLocation.isvalid()){
        Log("now loc: %d %d", localLocation.x, localLocation.y);
        assert(0);
      }
      return data[localLocation.x][localLocation.y];
    }
    friend std::istream& operator>> (std::istream &is, LocalLocation &localLocation) {
      is >> localLocation.y >> localLocation.x;
      return is;
    }

    //stl_container-like
    using iterator = LocalLocation;
    LocalLocation begin() {
      return LocalLocation(0, 0);
    }
    LocalLocation end() {
      return LocalLocation(n * N_SCALE, 0);
    }
    void clear() {
      // just do nothing
      // memset(data, 0, sizeof(data));
      return ;
    }
  };
  extern Matrix<char, 1, 1> charmap;
  extern Matrix<Tile::TileKind, 1, 1> tileMap;
};
#endif