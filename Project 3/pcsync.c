#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "student.h"
#include <pthread.h>

int BUFFER_SIZE;
struct student **buffer;
int *bufferCount;
pthread_mutex_t *mutex;
pthread_cond_t *condc;
pthread_cond_t *condp;
FILE *out;
char inputName[255];
char outputName[255];
int isFinished(struct student *arr, int len) {
    for (int i = 0; i < len; i++) {
        if (arr[i].sid != 0) {
            return 0;
        }
    }
    return 1;
}

int findEmpty(struct student *arr, int len) {
    for (int i = 0; i < len; i++) {
        if (arr[i].sid == 0) {
            return i;
        }
    }
    return -1;
}

int findOccupied(struct student *arr, int len) {
    for (int i = 0; i < len; i++) {
        if (arr[i].sid != 0) {
            return i;
        }
    }
    return -1;
}

void *producer(void *param) {
    struct producerParam *para = (struct producerParam *) param;

    FILE *fp = fopen(inputName, "rb");
    if (fp == NULL) {
        printf("Invalid File");
        exit(1);
    }
    char buf[255]="";
    struct student *std;
    struct student *array = (struct student*)malloc(para->size * sizeof(struct student));
    int j = 0;
    //while ((read = getline(&line, &len, fp)) != -1) {
    //printf("Produce %d girdi", para->id);
    while (fgets(buf, sizeof(buf), fp) != NULL) {
        buf[strlen(buf) - 1] = '\0';
        //part = strtok(line, " ");
        //part = strtok(buf, " ");
        //int index = atoi(part);
        size_t length = strlen(buf);
        if (length != 0) {
            int index;
            int sid;
            char firstname[64];
            char lastname[64];
            float cgpa;
            sscanf(buf, "%d %d %s %s %f\n", &index, &sid, firstname, lastname, &cgpa);
            if (index == para->id) {
                std = malloc(sizeof(struct student));
                std->sid = sid;
                strcpy(std->firstname, firstname);
                strcpy(std->lastname, lastname);
                std->cgpa = cgpa;
                array[j].sid = std->sid;
                strcpy(array[j].firstname, std->firstname);
                strcpy(array[j].lastname, std->lastname);
                array[j].cgpa = std->cgpa;
                free(std);
                j++;
            }
        }
        for (int i = j; i < para->size; i++) {
            array[i].cgpa = 0;
            array[i].sid = 0;
        }


    }
    para->arr = array;
    pthread_mutex_lock(&mutex[para->id]);
    for (int i = 0; i < para->size; i++) {
        while (bufferCount[para->id] == BUFFER_SIZE) {
            pthread_cond_signal(&condc[para->id]);
            pthread_cond_wait(&condp[para->id], &mutex[para->id]);

        }
        if (bufferCount[para->id] == -2) {
            bufferCount[para->id] = 0;
        }
        if (para->arr[i].sid != 0) {
            int empty = findEmpty(buffer[para->id], BUFFER_SIZE);
            if (para->arr[i].sid != 0) {
                buffer[para->id][empty].sid = para->arr[i].sid;
                strcpy(buffer[para->id][empty].firstname, para->arr[i].firstname);
                strcpy(buffer[para->id][empty].lastname, para->arr[i].lastname);
                buffer[para->id][empty].cgpa = para->arr[i].cgpa;
                //printf("Sended: %d --- %d\n", para->id, para->arr[i].sid);
                para->arr[i].sid = 0;
                bufferCount[para->id]++;
                pthread_cond_signal(&condc[para->id]);

            }

            int finish = isFinished(para->arr, para->size);
            if (finish == 1) {
                pthread_cond_signal(&condc[para->id]);
                break;
            }


        }

    }
	free(array);
	fclose(fp);
    bufferCount[para->id] = -10;
    pthread_cond_signal(&condc[para->id]);
    pthread_mutex_unlock(&mutex[para->id]);
    pthread_exit(NULL);
}

void *consumer(void *param) {
    struct consumerParam *para = (struct consumerParam *) param;
    //printf("Consumer %d girdi", para->id);
    struct student* array = malloc(para->sumOfStudents * sizeof(struct student));
    for (int i = 0; i < para->sumOfStudents; i++) {
        array[i].sid = 0;
    }
    int index = 0;
    for (int i = 0; i < para->n; i++) {
        pthread_mutex_lock(&mutex[i]);
        for (int j = 0; j < para->sizeOfProducer[i]; j++) {

            while (((bufferCount[i] == -2) || bufferCount[i] == 0) && bufferCount[i] != -10) {
                /*if (isFinished(buffer[i]) == 1) {
                    break;
                }
                ;*/
                /*if (bufferCount[i] == -10) {
                    break;
                }*/
                pthread_cond_signal(&condp[i]);
                pthread_cond_wait(&condc[i], &mutex[i]);
                //if (bufferCount[i] == -1) {
                //    break;
                //}

            }

            int occupied = findOccupied(buffer[i], BUFFER_SIZE);
            if (occupied == -1) {
                break;
            }
            if (buffer[i][occupied].sid != 0) {
                //printf("%d --- %d\n", i, buffer[i][occupied].sid);
                array[index] = buffer[i][occupied];
                buffer[i][occupied].sid = 0;
                index++;
                bufferCount[i]--;
            }
        }
        pthread_cond_signal(&condp[i]);
        pthread_mutex_unlock(&mutex[i]);
    }
    //printf("çıkıyom\n");
    for (int i = 0; i < (para->sumOfStudents) -1; i++) {
        for (int j = i+1; j < para->sumOfStudents; j++) {
            if (array[i].sid > array[j].sid) {
                struct student temp = array[i];
                array[i] = array[j];
                array[j] = temp;
            }
        }
    }

    for (int i = 0; i < para->sumOfStudents; i++) {
        //printf("Sorted: Sid - %d\n", array[i].sid);
        if (array[i].sid != 0) {
            fprintf(out, "%d %s %s %.2f\n", array[i].sid, array[i].firstname, array[i].lastname, array[i].cgpa);
        }
    }
	free(array);
    pthread_exit(NULL);
}


int main(int argc, char *argv[]) {
    int numberOfProducers;
    if (argc == 4) {
        BUFFER_SIZE = 100;
        numberOfProducers = atoi(argv[1]);
        strcpy(inputName, argv[2]);
        strcpy(outputName, argv[3]);
    } else if (argc == 5) {
        numberOfProducers = atoi(argv[1]);
        BUFFER_SIZE = atoi(argv[2]);
        strcpy(inputName, argv[3]);
        strcpy(outputName, argv[4]);
    }
    out = fopen(outputName, "wb+");
    pthread_t tid[numberOfProducers + 1];
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    //struct student **stdArray = (struct student **) malloc(numberOfProducers * sizeof(struct student));
    /*for (int i = 0; i < numberOfProducers; i++) {
        stdArray[i] = (struct student *) malloc(MAX_STUDENT * sizeof(struct student));
    }*/
    char buf[255]="";
    int numberOfStudent[numberOfProducers];
    for (int i = 0; i < numberOfProducers; i++) {
        numberOfStudent[i] = 0;
    }
    FILE *fp = fopen(inputName, "rb");
    while (fgets(buf, sizeof(buf), fp) != NULL) {
        buf[strlen(buf) - 1] = '\0';
        size_t length = strlen(buf);
        if (length != 0) {
            int index;
            int sid;
            char firstname[30];
            char lastname[30];
            float cgpa;
            sscanf(buf, "%d %d %s %s %f\n", &index, &sid, firstname, lastname, &cgpa);
            numberOfStudent[index]++;
        }
    }

    buffer = (struct student **) malloc(numberOfProducers * sizeof(struct student));
    for (int i = 0; i < numberOfProducers; i++) {
        buffer[i] = (struct student *) malloc(BUFFER_SIZE * sizeof(struct student));
        for (int j = 0; j < 0; j++) {
            buffer[i][j].cgpa = 0;
			buffer[i][j].sid = 0;
        }
    }

    bufferCount = (int *) malloc(numberOfProducers * sizeof(int));
    for (int i = 0; i < numberOfProducers; i++) {
        bufferCount[i] = -2;
    }

    mutex = (pthread_mutex_t *) malloc(numberOfProducers * sizeof(pthread_mutex_t));
    for (int i = 0; i < numberOfProducers; i++) {
        pthread_mutex_init(&mutex[i], NULL);
    }

    condc = (pthread_cond_t *) malloc(numberOfProducers * sizeof(pthread_cond_t));
    for (int i = 0; i < numberOfProducers; i++) {
        pthread_cond_init(&condc[i], NULL);
    };
    condp = (pthread_cond_t *) malloc(numberOfProducers * sizeof(pthread_cond_t));
    for (int i = 0; i < numberOfProducers; i++) {
        pthread_cond_init(&condp[i], NULL);
    };

    struct consumerParam *cParam = malloc(sizeof(struct consumerParam));
    cParam->id = numberOfProducers;
    cParam->n = numberOfProducers;
    cParam->sizeOfProducer = malloc(numberOfProducers*sizeof(int));
    cParam->sumOfStudents = 0;
    for (int i = 0; i < numberOfProducers; i++) {
        cParam->sizeOfProducer[i] = numberOfStudent[i];
        cParam->sumOfStudents = cParam->sumOfStudents + numberOfStudent[i];
    }
    pthread_create(&tid[numberOfProducers], &attr, consumer, cParam);

    struct producerParam **pParam = malloc(numberOfProducers * sizeof(struct student));
    for (int i = 0; i < numberOfProducers; ++i) {
            pParam[i] = malloc(sizeof(struct producerParam));
            pParam[i]->arr = NULL;
            pParam[i]->id = i;
            pParam[i]->size = numberOfStudent[i];
            pthread_create(&tid[i], &attr, producer, pParam[i]);
    }

    /*for (int i = 0; i <  numberOfProducers; i++)
        for (j = 0; j < MAX_STUDENT; j++)
            printf("%d ", stdArray[i][j].sid);*/
    //struct student* result;
    for (int i = 0; i < numberOfProducers; ++i) {
        pthread_join(tid[i],NULL);

    }
    //struct student* result;
    pthread_join(tid[numberOfProducers], NULL);

	fclose(fp);
    fclose(out);
    for (int i = 0; i < numberOfProducers; i++) {
        free(buffer[i]);
    }
	for (int i = 0; i < numberOfProducers; i++) {
        free(pParam[i]);
    }
	free(pParam);
    free(cParam->sizeOfProducer);
	free(cParam);
    free(bufferCount);
    free(mutex);
    free(condc);
    free(condp);

    //printf("Hello, World!\numberOfProducers");
    return 0;
}
