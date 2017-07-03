#ifndef PONG_RENDER_H
#define PONG_RENDER_H
#include "stb_image.h"
#include "stb_truetype.h"
#include "minimath.h"

typedef struct Texture {
    unsigned int id;
    int width, height;
} Texture;

typedef struct Glyph {
    int codepoint;
    MiniRecti texture_rect; // in pixels, not normalized
    int xoffset;
    int yoffset;
    float advance;
    int lsb;
} Glyph;

typedef struct Font {
    stbtt_fontinfo info;
    Texture texture;
    Glyph* glyphs;
    size_t glyphs_num;
    float scale;
} Font;

// Shaders
unsigned int LoadShaderFromSource(int type, const char* source);
unsigned int LoadShaderFromFile(int type, const char* path);
unsigned int CreateShaderProgram(unsigned int vertex, unsigned int fragment);
void BeginShader(unsigned int shader);
void EndShader();

// Textures
Texture LoadTextureFromFile(const char* path);
Texture LoadTextureFromMemory(unsigned char* data, int width, int height);
void UnloadTexture(Texture texture);

// Fonts/Text
Font LoadFontFromFile(const char* path, int size);
Font LoadFontFromMemory(unsigned char* data, int size);
void UnloadFont(Font font);
void DrawText(Font font, const char* text, float x, float y);

void SetProjViewMatrix(MiniMatrix mat);

#endif
