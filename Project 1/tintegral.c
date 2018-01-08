

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include "function.h"

struct numbersBetween {
    double lowerLimit;
    double upperLimit;
    double deltaX;
    double subInterval;
    double result;

};
double sum;

void *runner(void *param) {
    struct numbersBetween *nums = (struct numbersBetween*)param;
    double function = 0.0f;
    double result = 0;
    double y;
    for (int i = 0; i <= nums->subInterval; i++) {
        if (i == 0) {
            y = compute_f(nums->lowerLimit);
            function = nums->lowerLimit;
            result += y;
        } else if (i == nums->subInterval) {
            y = compute_f(nums->upperLimit);
            function = nums->upperLimit;
            result += y;
        } else if (i != nums->subInterval) {
            y = 2*compute_f(function);
            result += y;
        }
        function += nums->deltaX;

    }
    result = result * (nums->deltaX / 2);
    nums->result = result;
    pthread_exit((void*) nums);
}

int main(int argc, char *argv[])
{
   
    int upper, lower, numberOfThread, subInterval;
    double deltaChild, deltaX;
    upper = atoi(argv[2]);
    lower = atoi(argv[1]);
    numberOfThread = atoi(argv[4]);
    subInterval = atoi(argv[3]);
	if((numberOfThread > 1000 || numberOfThread < 0) || (subInterval < 0)) {
		printf("Invalid argument\n");
		exit(1);
	}
    deltaChild = ((double)(upper-lower)/(numberOfThread));
    deltaX = (double)(upper-lower)/(numberOfThread*subInterval);
    double sumResult = 0;
    struct numbersBetween *nums;
    pthread_t tid[(int)numberOfThread];
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    double tempLower = lower;
    for (int i = 0; i < numberOfThread; i++) {
        nums = malloc(sizeof(struct numbersBetween));
        nums->deltaX = deltaX;
        nums->subInterval = subInterval;
        if (i == 0) {
            nums->lowerLimit = lower;
        }
        if (i != 0) {
            nums->lowerLimit = tempLower+deltaChild;
        }
        nums->upperLimit = nums->lowerLimit+deltaChild;
        deltaX = (nums->upperLimit-nums->lowerLimit)/(subInterval);
        pthread_create(&tid[i], &attr, runner, nums);
        tempLower = nums->lowerLimit;

    }
    struct numbersBetween *n;
    for (int i = 0; i < numberOfThread; i++) {
        pthread_join(tid[i], (void *) &n);
        sumResult += n->result;
    }
    free(nums);
	printf ("%lf\n", sumResult);
}
