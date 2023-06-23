OBJS=cg_player.c \
	main.c
	
CC=gcc

ICFLAGS=-ISDL2-2.26.5/i686-w64-mingw32/include/SDL2
ICFLAGS+=-ISDL2_mixer-2.6.3/i686-w64-mingw32/include/SDL2
LDFLAGS=-LSDL2-2.26.5/i686-w64-mingw32/lib
LDFLAGS+=-LSDL2_mixer-2.6.3/i686-w64-mingw32/lib
LIBS=-lmingw32 -lSDL2main -lSDL2 -lSDL2_mixer

OBJ_NAME=CGplayer

all:$(OBJS)
	$(CC) $(OBJS) $(ICFLAGS) $(LDFLAGS) $(LIBS) -o $(OBJ_NAME)