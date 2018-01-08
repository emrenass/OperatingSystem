

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include "function.h"

#define READ_END 0
#define WRITE_END 1

int main(int argc, char *argv[])
{

	double y;
	int upper, lower, numberOfChild, subInterval;
    double deltaChild, deltaX;

	upper = atoi(argv[2]);
    lower = atoi(argv[1]);
    numberOfChild = atoi(argv[4]);
    subInterval = atoi(argv[3]);
	if((numberOfChild > 50 || numberOfChild < 0) || (subInterval < 0)) {
		printf("Invalid argument\n");
		exit(1);
	}
    deltaChild = ((double)(upper-lower)/(numberOfChild));
    deltaX = (double)(upper-lower)/(numberOfChild*subInterval);
    int fd[numberOfChild][2];
    double result = 0;
    for (int i = 0; i < numberOfChild; ++i) {
        pipe(fd[i]);
    }


	double childLower = lower;
	double childUpper;
	for (int i = 0; i < numberOfChild; i++) {
		if (i != 0) {
			childLower += deltaChild;
		}
		childUpper = childLower+deltaChild;
		deltaX = (childUpper-childLower)/(subInterval);
		double function = 0.0f;
		if (fork() == 0) {
			//close(fd[i][READ_END]);
			double childResult = 0;
			for (int j = 0; j <= subInterval; j++) {
				if (j == 0) {
					y = compute_f(childLower);
					function = childLower;
					childResult += y;
				}
				else if (j == subInterval) {
					y = compute_f(childUpper);
					function = childUpper;
					childResult += y;
				}
				else if (j != subInterval){
					y = 2*compute_f(function);
					childResult += y;
				}
				function += deltaX;
			}
			childResult = childResult * (deltaX / 2);
			close(fd[i][READ_END]);
			write(fd[i][WRITE_END], &childResult, sizeof(double));
			close(fd[i][WRITE_END]);
			exit(0);
		}
	}

	for (int i = 0; i < numberOfChild; i++) {
		wait(NULL);
	}
    double n;
    for (int i = 0; i < numberOfChild; ++i) {
        //close(fd[i][WRITE_END]);
        read(fd[i][READ_END], &n, sizeof(double));
        result += n;
        //close(fd[i][READ_END]);
    }
	/*close(fd[WRITE_END]);
	double n;
	while((read(fd[READ_END], &n, sizeof(double)) > 0)){
		result += n;
	}*/
	printf ("%lf\n", result);

}
