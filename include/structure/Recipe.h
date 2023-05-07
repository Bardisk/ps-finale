#ifndef RECIPE
#define RECIPE
#include <structure.h>

struct Recipe
{
  int time;
  std::string nameBefore, nameAfter;
  int src, dst;
  Cooker::CookerKind kind;
  friend std::istream& operator>> (std::istream &is, Recipe &recipe) {
    is >> recipe.time >> recipe.nameBefore;
    std::string kindstr;
    is >> kindstr;
    recipe.kind = Cooker::decode(kindstr);
    is >> recipe.nameAfter;
    return is;
  }
};

#endif