LIBS = /usr/local/lib/libSDL2.a  /usr/local/lib/libSDL2_mixer.a /usr/local/lib/libSDL2_image.a /usr/local/lib/libSDL2_ttf.a  -ldl -lpthread -lfreetype
FLAG = -g -Wall -O3
CC = g++

slidepuzzle: core.o gui.o window_icon.o
	$(CC)  $(FLAG) -o $@ core.o gui.o window_icon.o $(LIBS)
	# g++ -o slidepuzzle core.o gui.o window_icon.o -L windows/x86/bin -lmingw32 -lSDL2 -lSDL2main -lSDL2_image -lSDL2_ttf -mwindows -Wl,--no-undefined -lm -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lshell32 -lversion -luuid -static-libgcc

core.o: core.cpp slidepuzzle.h
	$(CC) $(FLAG) -c core.cpp
gui.o: gui.cpp slidepuzzle.h
	$(CC) $(FLAG) -c gui.cpp
window_icon.o: window_icon.cpp slidepuzzle.h
	$(CC) $(FLAG) -c window_icon.cpp

clean:
	rm *.o slidepuzzle
