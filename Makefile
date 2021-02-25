objects = servo.o remoteCtl.o

remote:$(objects)
	gcc $(objects) -o remote.o -lwiringPi

remoteCtl.o:remoteCtl.c servo.h
	gcc -c remoteCtl.c

servo.o:servo.c servo.h
	gcc -c servo.c 

$(objects):
.PHONY : clean
clean:
	rm  $(objects) remote.o
