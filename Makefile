CC = gcc
INCLUDE_DIR = include/

BINARIES = $(patsubst src/%.c,bin/%.o, $(wildcard src/*.c)) \
		   $(patsubst %.c,bin/%.o, $(wildcard *.c))           

COMPILE_FLAGS = -Wall -Werror -pedantic -std=c99 -g -D__USE_MINGW_ANSI_STDIO

.PHONY: all clean
all: app.exe

app.exe: $(BINARIES)
	$(CC) -o $@ -I $(INCLUDE_DIR) $(COMPILE_FLAGS) $^

bin/%.o: src/%.c
	mkdir -p bin 
	$(CC) -o $@ -I $(INCLUDE_DIR) $(COMPILE_FLAGS) $^ -c 

clean:
	rm -rf bin/ app.exe