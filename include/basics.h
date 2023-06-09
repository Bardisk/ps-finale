#ifndef BASICS_H__
#define BASICS_H__

#undef NDEBUG

#include <string>
#include <vector>
#include <iostream>
#include <cstdio>
#include <cassert>
#include <optional>
#include <sstream>
#include <stack>
#include <cstdarg>
#include <queue>
#include <cstring>
#include <queue>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <cmath>

#define N_MAX 25
#define M_MAX 25
#define INGRD_NR_MAX 25
#define RECIP_NR_MAX 25
#define TOTOD_NR_MAX 25
#define ORDER_NR_MAX 25
#define PLAYR_NR_MAX 2
#define ENTTY_NR_MAX 20
#define ATTEN_ORDER_NR 3

const double eps=1e-5;

inline void Log(const char *fmt, ...) {
#ifdef NDEBUG
  return ;
#endif
  char buf[1024];
  va_list arglist;
  va_start(arglist, fmt);
  vsprintf(buf, fmt, arglist);
  va_end(arglist);
  std::cerr << buf << std::endl;
}

#include <kind.h>
#include <plane.h>
#include <structure.h>
#include <task.h>
#include <routine.h>

#endif