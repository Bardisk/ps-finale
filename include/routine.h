#ifndef ROUTINE_H__
#define ROUTINE_H__

#include <basics.h>

// read initially
void init_read();

// read a frame
// @return whether to skip
bool frame_read(int nowFrame);

#endif