LIBS = /usr/local/lib/libSDL2.a /usr/local/lib/libSDL2_image.a /usr/local/lib/libSDL2_ttf.a  -ldl -lpthread
FLAG = -g -Wall
CC = g++

slidepuzzle: core.o gui.o window_icon.o
	$(CC)  $(FLAG) -o $@ core.o gui.o window_icon.o $(LIBS)

core.o: core.cpp slidepuzzle.h
	$(CC) $(FLAG) -c core.cpp
gui.o: gui.cpp slidepuzzle.h
	$(CC) $(FLAG) -c gui.cpp
window_icon.o: window_icon.cpp slidepuzzle.h
	$(CC) $(FLAG) -c window_icon.cpp

clean:
	rm *.o slidepuzzle
