# SDLDIR = /usr/lib/x86_64-linux-gnu
# LIBS = $(SDLDIR)/libSDL2.a  $(SDLDIR)/libSDL2_mixer.a $(SDLDIR)/libSDL2_image.a $(SDLDIR)/libSDL2_ttf.a  -ldl -lpthread -lfreetype
LIBS = -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf
FLAG = -g -Wall -O3 --std=c++98
CC = clang++

slidepuzzle: core.o gui.o window_icon.o
	$(CC)  $(FLAG) -o $@ core.o gui.o window_icon.o $(LIBS)
	# g++  -o slidepuzzle core.o gui.o window_icon.o slidepuzzle.res windows/x86/lib/libSDL2_image.a windows/x86/lib/libSDL2_ttf.a windows/x86/lib/libSDL2.a windows/x86/lib/libSDL2_mixer.a -L windows/x86/bin -lvorbisfile-3 -lmingw32 -lSDL2main -lfreetype-6 -mwindows -Wl,--no-undefined -lm -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lshell32 -lversion -luuid -static-libgcc

eight: eight.o
	$(CC) $(FLAG) -o $@ eight.o

eight.o: eight.cpp
	$(CC) $(FLAG) -c eight.cpp

core.o: core.cpp slidepuzzle.h
	$(CC) $(FLAG) -c core.cpp
gui.o: gui.cpp slidepuzzle.h
	$(CC) $(FLAG) -c gui.cpp
window_icon.o: window_icon.cpp slidepuzzle.h
	$(CC) $(FLAG) -c window_icon.cpp
# slidepuzzle.res: slidepuzzle.rc
	# windres slidepuzzle.rc -O coff -o slidepuzzle.res

clean:
	rm *.o slidepuzzle *.res
	rm eight
