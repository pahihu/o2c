#ifndef __Time_
#define __Time_

#include "_OGCC.h"
#include "Unix.h"



typedef struct Unix_timeval Time_Time;


extern void Time_Reset (struct Unix_timeval (* _time));
extern void Time_Add (struct Unix_timeval _a, struct Unix_timeval _b, struct Unix_timeval (* _result));
extern void Time_Sub (struct Unix_timeval _a, struct Unix_timeval _b, struct Unix_timeval (* _result));
extern void Time_Mult (struct Unix_timeval _a, LONGREAL _m, struct Unix_timeval (* _result));
extern INTEGER Time_Cmp (struct Unix_timeval _a, struct Unix_timeval _b);
extern void Time_GetSysTime (struct Unix_timeval (* _time));
extern void Time_Since (struct Unix_timeval _start, struct Unix_timeval (* _elapsed));

extern void _init_Time (void);

#endif
