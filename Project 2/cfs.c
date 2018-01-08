#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <memory.h>
#include "CPU.h"
int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Invalid Call Check Arguments\n");
        exit(1);
    }

    char* workLoadFile = argv[1];
    char *outputFile = argv[2];
    run = malloc(sizeof(run));
    run->list = malloc(sizeof(linkedlist));
    run->list->head = NULL;
    run->list->size = 0;

    orderedStart = malloc(sizeof(linkedlist));
    orderedStart->head = NULL;
    char * line;
    size_t len;
    ssize_t read;
    char* splitted;

    char *array[MAX_BURST];


    FILE *fp = fopen(workLoadFile, "rb" );
    FILE *fpO = fopen (outputFile, "wb+");

    //FILE *fp = fopen( "/home/emrenas/CLionProjects/CFS/dene", "rb" );
    //FILE *fpO = fopen ("/home/emrenas/CLionProjects/CFS/output", "wb+");
    if (fp == NULL || fpO == NULL) {
        printf("File error\n");
        exit(1);
    }
    fseek(fp, 0, SEEK_END); // seek to end of file
    long sizeOfFile = ftell(fp); // get current file pointer
    fseek(fp, 0, SEEK_SET); // seek back to beginning of file
    while ((read = getline(&line, &len, fp)) != -1) {
        if (line[strlen(line) - 1] == '\n')
            line[strlen(line) - 1] = ' ';
        int i=0;
        //printf("Line: %s", line);
        array[i] = strtok(line, " ");

        while (array[i] != NULL) {
            array[++i] = strtok(NULL, " ");
        }
        i=0;
        while (array[i] != NULL) {
            int temppid;
            process* pro = NULL;
            node *temp = orderedStart->head;
            if (isdigit(*array[0])) {
                temppid = atoi(array[0]);
                if (orderedStart->head == NULL) {
                    pro = NULL;
                }
                else {
                    for (int k = 0; k<orderedStart->size; k++) {
                        if (temp->pro->pid == temppid) {
                            pro = temp->pro;
                            break;
                        }
                        temp = temp->next;
                    }
                }
                if(pro == NULL) {
                    pro = malloc(sizeof(process));
                    pro->pid = temppid;
                    insert(orderedStart, pro);
                }
                if(array[i] != NULL && temp != NULL) {
                    if (strcmp(array[i], "start") == 0)
                        temp->pro->startTime = atoi(array[i + 1]);
                    if (strcmp(array[i], "prio") == 0)
                        temp->pro->prio = atoi(array[i + 1]);
                    if (strcmp(array[i], "cpu") == 0) {
                        for (int n = 0; n < MAX_BURST; n++) {
                            if (temp->pro->arr[n] == NULL) {
                                temp->pro->arr[n] = malloc(sizeof(burst));
                                temp->pro->arr[n]->burstType = "cpu";
                                temp->pro->arr[n]->time = atoi(array[i + 1]);
                                break;
                            }
                        }
                    }
                    if (strcmp(array[i], "io") == 0) {
                        for (int n = 0; n < MAX_BURST; n++) {
                            if (temp->pro->arr[n] == NULL) {
                                temp->pro->arr[n] = malloc(sizeof(burst));
                                temp->pro->arr[n]->burstType = "io";
                                temp->pro->arr[n]->time = atoi(array[i + 1]);
                                break;
                            }
                        }
                    }
                }
            }
            i++;
        }
    }

    orderByStartTime(orderedStart);

    fclose(fp);
	//line = malloc(1);
    /*if (line)
        free(line);*/
    while (run->pro != NULL || run->list->head != NULL || orderedStart->head != NULL || IOqueue->head != NULL) {
        updateTimer(fpO);
		
    }
    fclose(fpO);
    if (line)
        free(line);
    bubbleSortForFinishList();

    for (finishNode *temp = finislist->head; temp != NULL; temp=temp->next) {
        printf("%d %d %d %d %d %d %f\n", temp->pro->pid, temp->pro->prio, temp->pro->startTime,
                temp->pro->finishTime, temp->pro->turnaround, temp->pro->waitTime, temp->pro->responseTime);
    }
	return 0;
}
