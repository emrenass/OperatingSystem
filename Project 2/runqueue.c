//
// Created by emrenas on 11/10/17.
//

#include <stddef.h>
#include "runqueue.h"

void updateCurrentProsses() {
    if (run->list->size > 0) {
        run->pro = retrieve(run->list);
    }
}

void updateMinVruntime() {
    /*long vruntime;
    process* firstnode;
    if (run->list->size > 0){ // if there is a ready process
        firstnode = getHead(run->list);  //  firstnode is the one that has smallest vrtuntime in the tree
        if(run->pro != NULL && run->pro->vruntime < firstnode->vruntime)
            vruntime = run->pro->vruntime;
        else
            vruntime = firstnode->vruntime;
    }
    else {
        vruntime = run->pro->vruntime;
        if (run->min_vruntime > vruntime) {
            run->min_vruntime = run->min_vruntime;
        }
        else {
            run->min_vruntime = vruntime;
        }
    }*/

    long vruntime;
    process* firstnode;
    if (run->list->size > 0){ // if there is a ready process
        firstnode = getHead(run->list);  //  firstnode is the one that has smallest vrtuntime in the tree
        if(run->min_vruntime < firstnode->vruntime)
            vruntime = run->min_vruntime;
        else
            vruntime = firstnode->vruntime;
        run->min_vruntime = vruntime;
    }
}

void updateWeights(int *pro, int *total) {
    if (*pro == NULL) {
        return;
    }
    pro = prio_to_weight[run->pro->prio];
    node* temp = run->list->head;
    for (int i = 0; i < run->list->size; i++) {
        *total += prio_to_weight[temp->pro->prio];
        temp = temp->next;
    }
}

