CC = gcc
CPPFLAGS = -Iinclude -D_GNU_SOURCE=1 -D_REENTRANT
CFLAGS = -g -pedantic -W -Wall -Wextra
LDLIBS = -lSDL -lpthread -lm

OBJ = src/main.o    \
      src/audio.o   \
      src/notes.o

BIN = main

SAMPLES_DIR = ./samples

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(LDLIBS) $(OBJ) -o $(BIN)

clean:
	$(RM) $(BIN) $(OBJ)

tetris: $(BIN)
	./$(BIN) $(SAMPLES_DIR)/tetris.txt

detail: $(BIN)
	./$(BIN) $(SAMPLES_DIR)/details.txt

instru: $(BIN)
	./$(BIN) $(SAMPLES_DIR)/instruments.txt

.PHONY : all clean tetris instru detail
