### C++ Arguments
CC=g++glut

### Program Arguments
NAME=holesinthevoid

all: 
	g++glut holesinthevoid.cc
clean:
	rm -rf *.o
