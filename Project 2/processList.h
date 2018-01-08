//
// Created by emrenas on 11/12/17.
//

#ifndef PROCESSLIST_H
#define PROCESSLIST_H

#include "process.h"

typedef struct processListStructure {
    process* pro;
    struct processListStructure* next;
}processList;

processList* head;

#endif
