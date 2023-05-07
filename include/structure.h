#ifndef STRUCTURE_H__
#define STRUCTURE_H__

#include <basics.h>

#include "structure/Ingredient.h"
#include "structure/Order.h"
#include "structure/Entity.h"
#include "structure/Recipe.h"
#include "structure/Player.h"

namespace Game{
  extern int totTime;
  extern int randomizeSeed;
  extern int remainFrame, fund;
  extern int currentFrame;
  extern Ingredient ingrdList[];
  extern Recipe recipList[];
  extern Order totodList[], orderList[];
  extern Player playrList[];
  extern Entity enttyList[];
  extern int ingrdCnt, recipCnt, orderCnt, enttyCnt, playrCnt, totodCnt;
}

#endif