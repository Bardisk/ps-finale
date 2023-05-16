#ifndef INGREDIENT
#define INGREDIENT
#include <structure.h>

struct Ingredient
{
  Location location;
  int price;
  std::string name;
  int type;
  friend std::istream& operator>> (std::istream &is, Ingredient &ingredient) {
    is >> ingredient.location >> ingredient.name >> ingredient.price;
    return is;
  }
};
#endif