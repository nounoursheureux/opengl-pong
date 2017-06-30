CC = gcc
CFLAGS = -Wall -Iinclude/
LDFLAGS =-lglfw -lGL -ldl -lm
OBJ = main.o glad.o utils.o render.o

all: pong

pong: $(OBJ)
	$(CC) $^ -o $@ $(LDFLAGS)

%.o: src/%.c
	$(CC) -c $< -o $@ $(CFLAGS)
