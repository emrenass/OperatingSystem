//
// Created by emrenas on 11/10/17.
// This class is implemantion of linkedlist for processes
//

#include "linkedlist.h"
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>


void initList(linkedlist** list) {
    *list = malloc(sizeof(linkedlist));
}

void insert(linkedlist* list,process* proInsert){
    if (list == NULL) {
        initList(list);
    }
    if (proInsert == NULL) {
        return;
    }
    process* pro=proInsert;
    if (list->head == NULL) {
        list->head = malloc(sizeof(node));
        list->head->pro = pro;
        list->head->next = NULL;
        list->size++;
    } else if (list->head->pro->vruntime > pro->vruntime) {
        node *temp = malloc(sizeof(temp));
        temp->next = list->head->next;
        temp->pro = list->head->pro;
        list->head->pro = pro;
        list->head->next = temp;
        list->size++;
    }
    else {
        node *temp = list->head;
        while (pro->vruntime >= temp->pro->vruntime && temp->next != NULL) {
            temp = temp->next;
            if (temp->next == NULL || pro->vruntime < temp->next->pro->vruntime) {
                break;
            }
        }
        node *temp2Node = malloc(sizeof(node));
        temp2Node->pro = pro;
        temp2Node->next = temp->next;
        temp->next = temp2Node;
        list->size++;
    }
}

void insertFinish(finishlinkedlist* list,finishedProcess* proInsert){
    if (finislist == NULL) {
        finislist = malloc(sizeof(finishlinkedlist));
    }
    if (proInsert == NULL) {
        return;
    }
    finishedProcess* pro=proInsert;
    if (finislist->head == NULL) {
        finislist->head = malloc(sizeof(node));
        finislist->head->pro = pro;
        finislist->head->next = NULL;
        finislist->size++;
    } else if (finislist->head->pro->pid > pro->pid) {
        finishNode *temp = malloc(sizeof(temp));
        temp->next = finislist->head->next;
        temp->pro = finislist->head->pro;
        finislist->head->pro = pro;
        finislist->head->next = temp;
        finislist->size++;
    }
    else {
        finishNode *temp = finislist->head;
        while (pro->pid >= temp->pro->pid && temp->next != NULL) {
            temp = temp->next;
            if (temp->next == NULL || pro->pid < temp->next->pro->pid) {
                break;
            }
        }
        finishNode *temp2Node = malloc(sizeof(node));
        temp2Node->pro = pro;
        temp2Node->next = temp->next;
        temp->next = temp2Node;
        finislist->size++;
    }
}

void bubbleSortForFinishList() {
    int length = finislist->size;
    finishedProcess *arr[length];
    finishNode *temp = finislist->head;
    for (int i = 0; i < length; i++) {
        arr[i] = malloc(sizeof(finishedProcess));
        arr[i] = temp->pro;
        temp = temp->next;
    }

    //bubble sort
    for (int i = 0; i < length - 1; i++) {
        for (int j = i + 1; j < length; j++) {
            if (arr[i]->pid > arr[j]->pid) {
                finishedProcess *t = arr[i];
                arr[i] = arr[j];
                arr[j] = t;
            }
        }
    }
    temp = finislist->head;
    for (int k = 0; k < length; ++k) {
        temp->pro = arr[k];
        temp = temp->next;
    }
}

/*void insertFinish(finishedLinkedlist* list,finishNode* proInsert){
    if (list == NULL) {
        finishList = malloc(sizeof(finishList));
    }
    if (proInsert == NULL) {
        return;
    }
    finishNode* pro=proInsert;
    if (list->head == NULL) {
        finishList->head = malloc(sizeof(node));
        finishList->head = pro;
        finishList->head->next = NULL;
        finishList->size++;
    } else if (list->head->pid < pro->pid) {
        finishedNode *temp = malloc(sizeof(temp));
        temp->next = list->head->next;
        temp. = list->head->pro;
        list->head->pro = pro;
        list->head->next = temp;
        list->size++;
    }
    else {
        node *temp = list->head;
        while (pro->vruntime >= temp->pro->vruntime && temp->next != NULL) {
            temp = temp->next;
            if (temp->next == NULL || pro->vruntime < temp->next->pro->vruntime) {
                break;
            }
        }
        node *temp2Node = malloc(sizeof(node));
        temp2Node->pro = pro;
        temp2Node->next = temp->next;
        temp->next = temp2Node;
        list->size++;
    }
}*/

void orderByStartTime(linkedlist *list){
    if (list->size <= 0) {
        return;
    }
    int length = list->size;
    process *arr[length];
    node *temp = list->head;
    for (int i = 0; i < length; i++) {
        arr[i] = malloc(sizeof(process));
        arr[i] = temp->pro;
        temp = temp->next;
    }

    //bubble sort
    for (int i = 0; i < length - 1; i++) {
        for (int j = i + 1; j < length; j++) {
            if (arr[i]->startTime > arr[j]->startTime) {
                process *t = arr[i];
                arr[i] = arr[j];
                arr[j] = t;
            }
        }
    }
    temp = list->head;
    for (int k = 0; k < length; ++k) {
        temp->pro = arr[k];
        finishedProcess *fpro = malloc(sizeof(finishedProcess));
        fpro->pid = temp->pro->pid;
        fpro->prio = temp->pro->prio;
        fpro->startTime = temp->pro->startTime;
        fpro->ioTime = getIOTime(temp->pro);
        fpro->cpuTime = getCPUTime(temp->pro);
        fpro->finishTime = -1;
        fpro->responseTime = -1;
        fpro->turnaround = -1;
        fpro->waitTime = -1;
        fpro->numberOfWait = 1;
        fpro->cameFromIO = -1;
        fpro->waitForResponse = 0;
        fpro->isInstantRun = -1;
        fpro->iscameFromIO = 0;
        fpro->timeToGoIO = 0;
        fpro->firstTimeOnCPU = 0;
        insertFinish(finislist, fpro);
        temp = temp->next;
    }
}

process* retrieve(linkedlist *list) {
    if (list->head == NULL) {
        return NULL;
    }
    if (list->head->next == NULL) {
        process *pro = list->head->pro;
        list->head = NULL;
        list->size--;
        return pro;
    }
    else {
        process *pro = list->head->pro;
        list->head = list->head->next;
        list->size--;
        return pro;
    }
}


void delete(linkedlist* list, process* pro){
    if (list->head == NULL) {
        return;
    }
    node* temp = list->head;
    if(pro->pid == temp->pro->pid) {
        list->head = list->head->next;
        list->size--;
    }
    else {
        for (temp = list->head; temp->next != NULL; temp = temp->next) {
            if (temp->next->pro->pid == pro->pid) {
                node* deleten = temp->next;
                temp->next = temp->next->next;
                free (deleten);
                list->size--;
                break;

            }
        }
    }

}

void printlist(linkedlist *list){
    if (list->head == NULL) {
        return;
    }
    else {
        for (node *temp = list->head; temp != NULL; temp = temp->next) {
            printf("Process %d, vruntime: %ld\n", temp->pro->pid, temp->pro->vruntime);
        }
    }
}

process* getHead(linkedlist *list) {
    if (list->head == NULL) {
        return NULL;
    } else {
        return list->head->pro;
    }
}

void updateIOqueue(linkedlist* list, int by) {
    if(list->head == NULL) {
        return;
    }
    for (node *temp = list->head; temp != NULL; temp=temp->next) {
        int index = getNextburst(temp->pro);
        if (strcmp(temp->pro->arr[index]->burstType, "io") == 0) {
            if(temp->pro->arr[index]->time > by) {
                temp->pro->arr[index]->time = temp->pro->arr[index]->time - by;
            }
            else {
                temp->pro->arr[index]->time = 0;
            }
        }
    }
}




