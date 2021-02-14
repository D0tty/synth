CC = gcc
CPPFLAGS = -Iinclude -D_GNU_SOURCE=1 -D_REENTRANT
CFLAGS = -g -pedantic -W -Wall -Wextra
LDLIBS = -lSDL -lpthread -lm

COMMON_OBJ = src/instruments.o \
             src/notes.o       \
             src/sdl_helpers.o

SONG_PLAYER_OBJ = src/audio.o       \
                  src/song_player.o \

SONG_PLAYER_BIN = song_player

SAMPLES_DIR = ./samples

all: $(SONG_PLAYER_BIN)

$(SONG_PLAYER_BIN): $(COMMON_OBJ) $(SONG_PLAYER_OBJ)
	$(CC) $(LDLIBS) $(COMMON_OBJ) $(SONG_PLAYER_OBJ) -o $(SONG_PLAYER_BIN)

clean:
	$(RM) $(COMMON_OBJ) $(SONG_PLAYER_BIN) $(SONG_PLAYER_OBJ)

tetris: $(SONG_PLAYER_BIN)
	./$(SONG_PLAYER_BIN) $(SAMPLES_DIR)/tetris.txt

detail: $(SONG_PLAYER_BIN)
	./$(SONG_PLAYER_BIN) $(SAMPLES_DIR)/details.txt

instru: $(SONG_PLAYER_BIN)
	./$(SONG_PLAYER_BIN) $(SAMPLES_DIR)/instruments.txt

.PHONY : all clean tetris instru detail
