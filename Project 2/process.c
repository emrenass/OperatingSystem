//
// Created by emrenas on 11/10/17.
//

#include <stddef.h>
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>
#include "process.h"



int getNextburst(process *pro) {
    if (NULL == pro) {
        return -1;
    }

    burst* temp[MAX_BURST];
    for (int i = 0; i < MAX_BURST; i++) {
        temp[i] = pro->arr[i];
    }
    for (int i = 0; i < MAX_BURST; i++) {
        if(temp[i] == NULL) {
            break;
        }
        if (temp[i]->time != 0) {
            return i;
        }
    }
    return -1;
}

int isFinished(process *pro) {
    if (NULL == pro) {
        return 0;
    }

    burst* temp[MAX_BURST];
    for (int i = 0; i < MAX_BURST; i++) {
        temp[i] = pro->arr[i];
    }
    for (int i = 0; i < MAX_BURST; i++) {
        if(temp[i] == NULL) {
            break;
        }
        if(temp[i]->time !=0) {
            return 0;
        }
    }
    return 1;
}

int getIOTime(process *pro) {
    if (pro == NULL) {
        return -1;
    }
    int returnValue = 0;
    for (int i = 0; i < MAX_BURST; i++) {
        if (pro->arr[i] == NULL) {
            break;
        }
        if (strcmp(pro->arr[i]->burstType, "io")==0) {
            returnValue += pro->arr[i]->time;
        }
    }
    return returnValue;
}

int getCPUTime(process *pro) {
    if (pro == NULL) {
        return -1;
    }
    int returnValue = 0;
    for (int i = 0; i < MAX_BURST; i++) {
        if (pro->arr[i] == NULL) {
            break;
        }
        if (strcmp(pro->arr[i]->burstType, "cpu")==0) {
            returnValue += pro->arr[i]->time;
        }
    }
    return returnValue;
}


