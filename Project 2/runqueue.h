//
// Created by emrenas on 11/10/17.
//
#ifndef RUNQUEUE_H
#define RUNQUEUE_H

#include "linkedlist.h"

typedef struct runqueue {
    linkedlist* list;
    process* pro;
    long min_vruntime;
    int numberOfProcess;
    int load;
}runqueue;
runqueue* run;
void updateWeights(int *pro, int* total);
void updateCurrentProsses();
void updateMinVruntime();

static const int prio_to_weight[40] = {
        /* 40 */    88761,  71755,  56483,  46273,  36291,
        /* 35 */    29154,  23254,  18705,  14949,  11916,
        /* 30 */    9548,   7620,   6100,   4904,   3906,
        /* 25 */    3121,   2501,   1991,   1586,   1277,
        /* 20 */    1024,   820,    655,    526,    423,
        /* 15 */    335,    272,    215,    172,    137,
        /* 10 */    110,    87,     70,     56,     45,
        /* 5  */    36,     29,     23,     18,     15
};

#endif