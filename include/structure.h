#ifndef STRUCTURE_H__
#define STRUCTURE_H__

#include <basics.h>

#include <structure/Ingredient.h>
#include <structure/Order.h>
#include <structure/Entity.h>
#include <structure/Recipe.h>
#include <structure/Player.h>

namespace Game{
  extern int totTime;
  extern int randomizeSeed;
  extern int remainFrame, fund;
  extern int currentFrame;
  extern Ingredient ingrdList[];
  extern Recipe recipList[];
  extern Order orderList[];
  extern Ordertemplate totodList[];
  extern Player playrList[];
  extern Entity enttyList[];
  extern int ingrdCnt, recipCnt, orderCnt, enttyCnt, playrCnt, totodCnt;
  
  extern Location ingrdDestination[INGRD_NR_MAX];
  extern Direction::DirectionKind ingrdDirection[INGRD_NR_MAX];
  extern Location washDestination, cleanDestination, surveDestination, dirtyDestination, plateDestination, indDestination;
  extern Direction::DirectionKind washDirection, cleanDirection, surveDirection, dirtyDirection, plateDirection, indDirection;
  extern std::unordered_map<std::string, int> madeFrom, madeFor;
  extern std::unordered_set<std::string> items;
  extern std::unordered_map<std::string, std::vector<int>> ingrdPlace;
  extern Location chopDestination, potDestination, panDestination;
  extern Direction::DirectionKind chopDirection, potDirection, panDirection;

  extern std::vector<Direction::DirectionKind> plateDirectionList;
  extern std::vector<Location> plateDestinationList;
  extern std::set<int> readyPlates, absentPlates;
  extern int readyPlateCnt;
  extern int dirtyPlateCnt;
  extern int cleanPlateCnt;
  extern std::optional<int> inWayPlateCnt;

  extern int attentionMaxCnt;
  extern int poolDirtyCnt, serveDirtyCnt;
  extern int panTime, potTime;
}

#endif