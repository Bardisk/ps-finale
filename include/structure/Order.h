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

struct AttentionOrder;
struct Task;

struct Dish {
  std::optional<int> chop;
  std::optional<std::pair<Cooker::CookerKind, int>> heat;
  Location boxDestination;
  DirectionKind boxDirection;
  AttentionOrder *parent;
  Dish() {}
  Dish(std::string name);

  //helper functions
  Cooker::CookerKind heatKind() {
    assert(heat.has_value());
    return heat.value().first;
  }
  Location heatDestination();
  DirectionKind heatDirection();
  int heatTime() {
    assert(heat.has_value());
    return heat.value().second;
  }
  int chopTime() {
    assert(chop.has_value());
    return chop.value();
  }
};

namespace Game {
  extern std::vector<Dish> dishList;
}

struct AttentionOrder
{
  // int price;
  std::vector<Dish> requirement;
  std::vector<bool> completed;
  int completeCnt;

  // Location targetPlateDestination;
  // DirectionKind targetPlateDirection;
  int targetPlate;

  enum State {
    GetPlate,
    Prepare,
    Surve,
    Done
  } curState;

  AttentionOrder() {}

  AttentionOrder(const Order &order) :
    completeCnt(0),
    targetPlate(-1),
    curState(GetPlate)
  {
    for (auto target : order.requirement) {
      requirement.push_back(Dish(target));
      completed.push_back(0);  
    }
  }

  std::vector<Task *> taskPool;
  void appendTask(Task *taskpointer) {
    taskPool.push_back(taskpointer);
  }

  std::optional<Task *> getPlate();

  ~AttentionOrder();
};

namespace Game {
  extern bool attentionInitialized;
  extern AttentionOrder attentionOrderList[ATTEN_ORDER_NR];
}

#endif