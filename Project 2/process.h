//
// Created by emrenas on 11/10/17.
// this class is implemantion of processes
//
#ifndef PROCESS_H
#define PROCESS_H
#define MAX_BURST 101


typedef struct burstStructure{
    char* burstType;
    int time;
}burst;
typedef struct processStructure{
    int pid;
    int prio;
    long vruntime;
    int startTime;
    burst* arr[MAX_BURST];
}process;

int getIOTime(process* pro);
int getCPUTime(process* pro);
int getNextburst(process* pro);
int isFinished(process* pro);
#endif
