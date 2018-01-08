//
// Created by emrenas on 11/14/17.
//

#ifndef CFS_FINISHPROCESS_H
#define CFS_FINISHPROCESS_H
typedef struct finishedProcessStructure{
    int pid;
    int prio;
    int ioTime;
    int cpuTime;
    int startTime;
    int numberOfWait;
    int waitTime;
    int cameFromIO;
    int waitForResponse;
    int isInstantRun;
    int iscameFromIO;
    int timeToGoIO;
    int firstTimeOnCPU;
    int finishTime;
    int turnaround;
    float responseTime;
}finishedProcess;
#endif //CFS_FINISHPROCESS_H
