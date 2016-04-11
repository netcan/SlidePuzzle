LIBS = /usr/local/lib/libSDL2.a /usr/local/lib/libSDL2_image.a /usr/local/lib/libSDL2_ttf.a  -ldl -lpthread
FLAG = -g -Wall
CC = g++

slidepuzzle: core.o gui.o
	$(CC)  $(FLAG) -o $@ core.o gui.o $(LIBS)

core.o: core.cpp slidepuzzle.h
	$(CC) $(FLAG) -c core.cpp
gui.o: gui.cpp slidepuzzle.h
	$(CC) $(FLAG) -c gui.cpp

clean:
	rm *.o slidepuzzle
