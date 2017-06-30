CC = clang
CFLAGS = -Wall -Iinclude/
LDFLAGS =-lglfw -lGL -ldl -lm -lgraphene-1.0
OBJ = main.o glad.o utils.o render.o

all: pong

pong: $(OBJ)
	$(CC) $^ -o $@ $(LDFLAGS)

%.o: src/%.c
	$(CC) -c $< -o $@ $(CFLAGS)
