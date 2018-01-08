#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

double ran_expo(double mean){
    double u;
    double lambda = 1/mean;
    u = rand() / (RAND_MAX + 1.0);

    return -log(1- u) / lambda;
}

int randomPrio() {
    int u = rand() % 40;
    return u;
}

int main(int argc, char** argv) {
    srand(time(NULL));
    /*int N = 10;
    int avgStart = 100;
    int avgBurst = 10;
    int avgCPU = 70;
    int avgIO = 200;
    char* file = "dene"; */

    int N = atoi(argv[1]);
    int avgStart = atoi(argv[2]);
    int avgBurst = atoi(argv[3]);
    int avgCPU = atoi(argv[4]);
    int avgIO = atoi(argv[5]);
    char* file = argv[6];
    FILE *fp = fopen(file, "wb+");
    for (int i = 0; i < N; ++i) {
        int start = (int)ran_expo(avgStart);
        fprintf(fp, "%d start %d prio %d\n", i, start, randomPrio());
        int burstCPU = (int)ran_expo(avgBurst);
        burstCPU = burstCPU/2+1;
        int burstIO = burstCPU -1;
        for (int j = 0; j < burstCPU; ++j) {
            int cpu = 0;
            while (cpu == 0) {
                cpu = (int)ran_expo(avgCPU);
            }

            int io = 0;
            while (io == 0) {
                io = (int)ran_expo(avgIO);
            }
            fprintf(fp, "%d cpu %d\n", i, cpu);
            if (burstIO != 0) {
                fprintf(fp, "%d io %d\n", i, io);
                burstIO--;
            }
        }
        fprintf(fp, "%d end\n", i);

    }
    return 0;
}
