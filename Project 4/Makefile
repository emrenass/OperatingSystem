

all:  libmyfs.a  app

libmyfs.a:  	myfs.c
	gcc -Wall -c myfs.c
	ar -cvq  libmyfs.a myfs.o
	ranlib libmyfs.a

app: 	app.c
	gcc -Wall -o app app.c  -L. -lmyfs

clean: 
	rm -fr *.o *.a *~ a.out app createdisk formatdisk
