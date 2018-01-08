//
// Created by emrenas on 11/11/17.
//

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include "CPU.h"
int currentTime = 0;
int targetedLatency = 0;
int lastPID=-1;
int same = 0;
int timeSlice = 0;
int timeShifted = 0;
int idleWriten = 0;
int timeProcessWriten = 0;
int sumTime = -1;
process* last;
int beforeIdlePid = -1;
int eventProcess = 0;

int contextSwitch() {
    int arr = 0;
    int io = 0;
    arr = checkNewArrival();
    io = checkIOqueue();
    return (arr || io);
}

int incrementTime(int by) {
    int returnValue = by;
    setTimeSlice();
    double delta;
    updateMinVruntime();
    if(run->pro != NULL) {
        double rate = ((float)prio_to_weight[20] / (float)(prio_to_weight[run->pro->prio]));
        int index = getNextburst(run->pro);
        if(index != -1) {
            if(run->pro->arr[index]->time <= by) {
                returnValue = run->pro->arr[index]->time;
                delta  = returnValue * 1000000 * rate;
                delta  = returnValue * 1000000 * rate;
                run->pro->vruntime += (int)delta;
                run->pro->arr[index]->time = 0;
            }
            else {
                run->pro->arr[index]->time -= returnValue;
                delta  = returnValue * 1000000 * rate;
                run->pro->vruntime += (int)delta;
            }
        }

    }
    currentTime = currentTime+returnValue;
    node* temp = IOqueue->head;
    for (int i = 0; i < IOqueue->size; i++) {
        int IOindex = getNextburst(temp->pro);
        if(temp->pro->arr[IOindex]->time <= returnValue) {
            temp->pro->arr[IOindex]->time = 0;
        }
        else {
            temp->pro->arr[IOindex]->time -= returnValue;
        }
        temp = temp->next;
    }
    return returnValue;
}
void setTimeSlice() {
    int NR = run->list->size;
    int total = 0;
    node* temp = run->list->head;
    for (int i = 0; i < run->list->size; i++) {
        total += prio_to_weight[temp->pro->prio];
        temp = temp->next;
    }
    if (run->pro != NULL) {
        total += prio_to_weight[run->pro->prio];
    }

    if (NR < 20) {
        targetedLatency = 200;
    } else {
        targetedLatency = 10* NR;
    }
    if (run->pro != NULL) {
        int processLoad = prio_to_weight[run->pro->prio];
        timeSlice = targetedLatency * (int) (processLoad / total);
        if (timeSlice < 100) {
            timeSlice = 100;
        }
        if (run->list->head == NULL) {
            int index = getNextburst(run->pro);
            if (index != -1) {
                if (strcmp(run->pro->arr[index]->burstType, "cpu") == 0) {
              //      timeSlice = run->pro->arr[index]->time;
                }
            }
        }
    }
}
int updateVRuntime(process *pro) {
    if (pro == NULL) {
        return -1;
    }

    int processLoad = prio_to_weight[pro->prio];
    int delta = (int) (prio_to_weight[20] / processLoad);
    return delta;
}

int checkNewArrival() {
    int arrived = 0;
    process *newArrival = getHead(orderedStart);
    for (int i = 0; newArrival != NULL; i++) {
        if (newArrival->startTime <= currentTime) {
            process *temp = retrieve(orderedStart);
            temp->vruntime = run->min_vruntime - 1000000;
            insert(run->list, temp);
            arrived = 1;
            newArrival = getHead(orderedStart);
        } else {
            break;
        }
    }
    return arrived;
}

int checkIOqueue() {

    int arrived = 0;
    for (node *temp = IOqueue->head; temp != NULL; temp = temp->next) {

        int index = getNextburst(temp->pro);
        if (index != -1 && index != 0) {
            if (strcmp(temp->pro->arr[index - 1]->burstType, "io") == 0) {
                if (temp->pro->arr[index - 1]->time <= 0) {
                    if (temp->pro->vruntime < run->min_vruntime - targetedLatency) {
                        temp->pro->vruntime = run->min_vruntime - targetedLatency;
                    }
                    //printf("time:       %d pid: %d ended io burst and vruntime is set to: %ld\n", currentTime,
                           //temp->pro->pid,
                           //temp->pro->vruntime);
                    for (finishNode* temp = finislist->head; temp != NULL; temp = temp->next) {
                        temp->pro->iscameFromIO = 1;
                        temp->pro->timeToGoIO = currentTime;
                    }

                    insert(run->list, temp->pro);
                    delete(IOqueue, temp->pro);
                    updateMinVruntime();
                    arrived = 1;

                }
            }
        }
    }
    return arrived;
}

void updateTimer(FILE *fp) {
    if (IOqueue == NULL) {
        IOqueue = malloc(sizeof(linkedlist));
        IOqueue->size = 0;
        IOqueue->head = NULL;
    }

    int currentWeight = 0;
    int totalWeight = 0;
    updateWeights(&currentWeight, &totalWeight);

    //Get arrived processes to linkedlist of runqueue
    /*process* newArrival = getHead(orderedStart);
    for (int i = 0; newArrival != NULL ; i++) {
        if(newArrival->startTime<=currentTime) {
            insert(run->list, retrieve(orderedStart));
            newArrival = getHead(orderedStart);
        }
        else {
            break;
        }
    }*/
    contextSwitch();

    if (run->pro == NULL) {
        run->pro = retrieve(run->list);
    }

    if (run->pro == NULL) {
        event = "idle";
        if (idleWriten == 0) {
            beforeIdlePid = 1;
            eventCounter = currentTime;
            //sumTime = -1;
            idleWriten = 1;
            incrementTime(1);
            return;
        }
    } else if (run->pro != NULL) {

        if (event == "idle" && idleWriten == 1) {
            if (eventProcess == 1) {
                fprintf(fp, "%d %d\n", lastPID, sumTime);
                eventProcess = 0;
            }
            fprintf(fp, "idle %d\n", currentTime - eventCounter);
            sumTime = -1;
            event = "!idle";
            idleWriten = 0;
            eventCounter = currentTime;
        }
    }
    if (run->pro != NULL) {
        eventProcess = 1;
        if (run->pro->pid == lastPID) {
            same = 1;
        } else {
            same = 0;
        }
        if (beforeIdlePid == 1) {
            same = 0;
            beforeIdlePid = -1;
            sumTime =-1;
        }
        if (same == 0) {
            for (finishNode *temp = finislist->head; temp != NULL; temp = temp->next) {
                if (temp->pro->pid == run->pro->pid) {
                    if(temp->pro->isInstantRun == -1) {
                        if (currentTime - temp->pro->startTime == 0) {
                            temp->pro->isInstantRun = 1;
                        } else {
                            temp->pro->isInstantRun = 0;
                        }
                    }
                    if (temp->pro->isInstantRun == 0) {
                        temp->pro->firstTimeOnCPU = currentTime;
                        temp->pro->waitForResponse += currentTime-temp->pro->startTime;
                        temp->pro->isInstantRun = 1;
                    }
                    if (temp->pro->iscameFromIO == 1) {
                        temp->pro->waitForResponse += currentTime - temp->pro->timeToGoIO;
                        temp->pro->iscameFromIO = 0;
                    }
                }
            }
                if (sumTime != -1) {
                    /*for (finishNode *temp = finislist->head; temp != NULL; temp = temp->next) {
                        if (temp->pro->pid == lastPID) {
                            if (temp->pro->isInstantRun == 0) {
                                temp->pro->waitForResponse += currentTime - temp->pro->startTime;
                                temp->pro->isInstantRun = 1;
                            }

                        }
                    }*/
                    fprintf(fp, "%d %d\n", lastPID, sumTime);
                    if (isFinished(getHead(run->list))) {
                        for (finishNode *temp = finislist->head; temp != NULL; temp = temp->next) {
                            if (temp->pro->pid == getHead(run->list)->pid) {
                                temp->pro->finishTime = currentTime;
                                temp->pro->waitTime = currentTime - getHead(run->list)->startTime - temp->pro->ioTime
                                        - temp->pro->cpuTime;
                                temp->pro->turnaround = currentTime - temp->pro->startTime;
                                temp->pro->responseTime = (float)temp->pro->waitForResponse / temp->pro->numberOfWait;
                            }
                        }
                        delete(run->list, getHead(run->list));
                    }
                }
            sumTime = 0;
        }
        int index = getNextburst(run->pro);
        if (index != -1) {
            setTimeSlice();
            int time = run->pro->arr[index]->time;
            if (strcmp(run->pro->arr[index]->burstType, "cpu") == 0) {
                if (!same) {
                    //printf("time:         %d pid: %d started in cpu\n", currentTime, run->pro->pid);
                    //printf("time:         %d pid: %d started cpu burst\n", currentTime, run->pro->pid);
                    //printf("time:         %d pid: %d timeslice: %d\n", currentTime, run->pro->pid, timeSlice);
                    lastPID = run->pro->pid;
                }
                timeShifted = 1;
                for (int i = 0; i < timeSlice;) {
                    int value = incrementTime(1);
                    i+= value;
                    setTimeSlice();
                    sumTime+=value;
                    //printf("time:         %d pid: %d timer ticked; vruntime=%ld\n", currentTime, run->pro->pid,
                           //run->pro->vruntime);

                    if (run->pro->arr[index]->time <= 0) {
                        index = getNextburst(run->pro);
                        if (index != -1 && strcmp(run->pro->arr[index]->burstType, "io") == 0) {

                            //printf("time:       %d pid: %d ended cpu burst\n", currentTime, run->pro->pid);
                            //printf("time:       %d pid: %d started io burst\n", currentTime, run->pro->pid);
                            for (finishNode *temp = finislist->head; temp != NULL; temp = temp->next) {
                                if (temp->pro->pid == run->pro->pid) {
                                    temp->pro->numberOfWait++;
                                }
                            }
                            //fprintf(fp, "%d\n", run->pro->pid);
                            //lastPID = run->pro->pid;
                            insert(IOqueue, run->pro);
                            run->pro = NULL;
                        } else {
                            insert(run->list, run->pro);
                            run->pro = NULL;
                        }
                        break;
                    }
                    if (contextSwitch() == 1) {
                        //fprintf(fp, "%d\n", currentTime - eventCounter);
                        //printf("time:       %d pid: %d removed from cpu - context switch\n", currentTime,
                               //run->pro->pid);
                        //printf("time:       %d pid: %d ended cpu burst\n", currentTime, run->pro->pid);
                        break;
                    }
                }
                if (run->pro != NULL) {
                    //printf("time:       %d pid: %d removed from cpu - burst ended\n", currentTime,
                           //run->pro->pid);
                }
                insert(run->list, run->pro);
                run->pro = NULL;
                /*if (time > timeSlice) {
                    //printf("time:         %d pid: %d timer ticked; vruntime=%lf\n",currentTime, run->pro->pid, run->pro->vruntime);
                    incrementTime(timeSlice);
                    //run->pro->vruntime = run->pro->vruntime + targetedLatency*updateVRuntime(run->pro);
                    run->pro->vruntime += timeSlice;
                    run->pro->arr[index]->time = run->pro->arr[index]->time - timeSlice;
                }
                else
                {
                    //printf("time:         %d pid: %d timer ticked; vruntime=%lf\n",currentTime, run->pro->pid, run->pro->vruntime);
                    incrementTime(time);
                    //run->pro->vruntime = run->pro->vruntime + time*updateVRuntime(run->pro);
                    run->pro->vruntime += time;
                    run->pro->arr[index]->time = 0;
                }*/
            }

        }

    }
    if (run->pro != NULL && isFinished(run->pro)) {
        for (finishNode *temp = finislist->head; temp != NULL; temp = temp->next) {
            if (temp->pro->pid == run->pro->pid) {
                temp->pro->finishTime = currentTime;
                temp->pro->waitTime = currentTime - temp->pro->startTime - temp->pro->ioTime
                                      - temp->pro->cpuTime;
                temp->pro->turnaround = currentTime - temp->pro->startTime;
                temp->pro->responseTime = (float)temp->pro->waitForResponse / temp->pro->numberOfWait;
            }
        }
        /*//printf("time:      %d pid: %d removed from cpu - finished\n", currentTime, run->pro->pid);
        //printf("time:      %d pid: %d finished process\n", currentTime, run->pro->pid);*/
        run->pro = NULL;
    }

    if (timeShifted == 0) {
        incrementTime(1);

    }
    contextSwitch();
    if (run->pro == NULL && run->list->head == NULL && orderedStart->head == NULL && IOqueue->head == NULL) {
        fprintf(fp, "%d %d\n", lastPID, sumTime);
    }
    timeShifted = 0;
    lastEvent = event;
}


/*void updateTimer(){
int NR = sched->run->list->size;
int currentWeight = 0;
int totalWeight = 0;
updateWeights(&currentWeight, &totalWeight);
double timeSlice = NR*currentWeight/totalWeight;
int timePassedForCurrent;
for (int i = 0; i < timeSlice; i++) {
    sched->currentTime++;
    if (sched->run->pro != NULL) {
        return;
    }
    timePassedForCurrent = sched->run->pro->vruntime++;
    updateMinVruntime();
    if (sched->run->pro != NULL) {
        NR++;
    }
    if (NR > 20) {
        sched->targetedLatency = 10*NR;
    }
    else {
        sched->targetedLatency = 200;
    }
}
if (run->list->size > 0) {
    run->pro = retrieve(run->list);
}
}*/





