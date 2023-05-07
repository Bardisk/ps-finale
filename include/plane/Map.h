#ifndef MAP
#define MAP
#include <plane.h>

namespace Map{
  template <typename T, int N_SCALE, int M_SCALE>
  struct Matrix {
    T data[N_MAX * N_SCALE][M_MAX * M_SCALE];
    struct LocalLocation : public Location {
      Matrix *father;
      LocalLocation(int _x=0, int _y=0, Matrix *_f=NULL) : Location(_x, _y), father(_f) {}
      LocalLocation(Location location) : Location(location) {} 
      LocalLocation operator ++(int) {
        if (y < m * M_SCALE) {
          y++;
          x=0;
        }
        else x++;
        return *this;
      }
      LocalLocation& operator ++() {
        if (y < m * M_SCALE) {
          y++;
          x=0;
        }
        else x++;
        return *this;
      }
      T& operator *() const { LocalLocation tmp = (*this); return *father[tmp]; }
      bool isvalid() const {
        return x >= 0 && x < n * N_SCALE && y >= 0 && y < m * M_SCALE;
      }
    };
    T& operator [](LocalLocation localLocation) {
      assert(localLocation.isvalid());
      return data[localLocation.x][localLocation.y];
    }
    friend std::istream& operator>> (std::istream &is, LocalLocation &localLocation) {
      is >> localLocation.y >> localLocation.x;
      return is;
    }

    //stl_container-like
    using iterator = LocalLocation;
    LocalLocation begin() {
      return LocalLocation(0, 0, this);
    }
    LocalLocation end() {
      return LocalLocation(n * N_SCALE, 0, this);
    }
  };
  extern Matrix<char, 1, 1> charmap;
  extern Matrix<Tile::TileKind, 1, 1> tileMap;
};
#endif