//
// Created by emrenas on 11/10/17.
//
#ifndef LINKEDLIST_H
#define LINKEDLIST_H
#include "process.h"
#include "finishProcess.h"
#define STARTTIME 1
#define PID 2
typedef struct nodeStructure{
    process* pro;
    struct nodeStructure* next;
}node;

typedef struct finishNodeStructure{
    finishedProcess* pro;
    struct finishNodeStructure* next;
}finishNode;

typedef struct listStructure{
    node* head;
    int size;
}linkedlist;

typedef struct finishListStructure{
    finishNode* head;
    int size;
}finishlinkedlist;

finishlinkedlist* finislist;
linkedlist* orderedStart;
linkedlist* IOqueue;
void bubbleSortForFinishList();
void insertFinish(finishlinkedlist *list, finishedProcess* pro);
void insert(linkedlist *list,process* pro);
process* retrieve(linkedlist *list);
void printlist(linkedlist *list);
process* getHead(linkedlist *list);
void orderByStartTime(linkedlist *list);
void updateIOqueue(linkedlist* list, int by);
void delete(linkedlist* list, process* pro);

#endif