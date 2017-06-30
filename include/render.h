#ifndef PONG_RENDER_H
#define PONG_RENDER_H
#include "stb_image.h"

typedef struct {
    unsigned int id;
    int width, height;
} Texture;

// Shaders
unsigned int LoadShaderFromSource(int type, const char* source);
unsigned int LoadShaderFromFile(int type, const char* path);
unsigned int CreateShaderProgram(unsigned int vertex, unsigned int fragment);

// Textures
Texture LoadTextureFromMemory(unsigned char* data, int width, int height);
Texture LoadTextureFromFile(const char* path);

#endif
