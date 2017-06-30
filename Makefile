CC = gcc
CFLAGS = -Wall -Iinclude/
LDFLAGS =-lglfw -lGL -ldl -lm
OBJ = main.o glad.o utils.o render.o

all: pong

pong: $(OBJ)
	$(CC) $^ -o $@ $(LDFLAGS)

%.o: src/%.c
	$(CC) -c $< -o $@ $(CFLAGS)

web:
	emcc src/*.c -Iinclude/ -o game.html -s USE_GLFW=3
