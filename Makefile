objects = servo.o remoteCtl.o

main:$(objects)
	gcc -o run $(objects) -lwiringPi

remoteCtl.o:servo.h
servo.o:servo.h

$(objects):
.PHONY : clean
clean:
	rm  $(objects) run
