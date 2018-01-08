//
// Created by emrenas on 11/11/17.
//

#ifndef CPU_H
#define CPU_H

#include "runqueue.h"

typedef struct schedulerStructure {
    int currentTime;
    runqueue* run;
    double targetedLatency;
}scheduler;

char* event;
char* lastEvent;
int eventCounter;
scheduler* sched;
extern int currentTime;
extern int NR;
extern int lastPID;
extern int targetedLatency;
extern int timeSlice;
void setTimeSlice();
int incrementTime(int by);
int checkIOqueue();
int checkNewArrival();
void updateTimer(FILE *fp);
int updateVRuntime(process* pro);
void setVariables();
int contextSwitch();

#endif
