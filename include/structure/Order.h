#ifndef ORDER
#define ORDER
#include <structure.h>

struct Ordertemplate
{
  int validFrame;
  int price;
  int frequency;
  std::vector<std::string> requirement;
  //to-do change to integerlist
  friend std::istream& operator >> (std::istream& is, Ordertemplate &order) {
    is >> order.validFrame >> order.price >> order.frequency;
    std::string s;
    std::getline(is, s);
    std::stringstream tmp(s);
    //critical bug!!!
    order.requirement.clear();
    while (tmp >> s) {
      order.requirement.push_back(s);
    }
    return is;
  }
};

struct Order
{
  int validFrame;
  int price;
  std::vector<std::string> requirement;
  //to-do change to integerlist
  friend std::istream& operator >> (std::istream& is, Order &order) {
    is >> order.validFrame >> order.price;
    std::string s;
    std::getline(is, s);
    std::stringstream tmp(s);
    //critical bug!!!
    order.requirement.clear();
    while (tmp >> s) {
      order.requirement.push_back(s);
    }
    return is;
  }
};
#endif