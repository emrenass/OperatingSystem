
all: cfs loadgen


process.o: process.c
	gcc -c -g -o process.o process.c

processList.o: processList.c
	gcc -c -g -o processList.o processList.c

finishProcess.o: finishProcess.c
	gcc -c -g -o finishProcess.o finishProcess.c

linkedlist.o: linkedlist.c
	gcc -c -g -o linkedlist.o linkedlist.c  

runqueue.o: runqueue.c
	gcc -c -g -o runqueue.o runqueue.c

CPU.o: CPU.c 
	gcc -c -g -o CPU.o CPU.c
   
cfs: cfs.c CPU.o process.o processList.o finishProcess.o linkedlist.o runqueue.o
	gcc -g -o cfs CPU.o process.o processList.o finishProcess.o linkedlist.o runqueue.o cfs.c

loadgen: loadgen.c
	gcc -g -o loadgen loadgen.c -lm

clean:
	/bin/rm -fr *~ *.o cfs loadgen

