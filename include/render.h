#ifndef PONG_RENDER_H
#define PONG_RENDER_H
#include "stb_image.h"
#include "stb_truetype.h"

typedef struct Texture {
    unsigned int id;
    int width, height;
} Texture;

typedef struct Glyph {
    int codepoint;
    int xoffset;
    int yoffset;
    int width;
    int height;
} Glyph;

typedef struct Font {
    stbtt_fontinfo font;
    Texture texture;
    Glyph* glyphs;
} Font;

// Shaders
unsigned int LoadShaderFromSource(int type, const char* source);
unsigned int LoadShaderFromFile(int type, const char* path);
unsigned int CreateShaderProgram(unsigned int vertex, unsigned int fragment);

// Textures
Texture LoadTextureFromFile(const char* path);
Texture LoadTextureFromMemory(unsigned char* data, int width, int height);

// Fonts
Font LoadFontFromFile(const char* path, int size);
Font LoadFontFromMemory(unsigned char* data, int size);

#endif
