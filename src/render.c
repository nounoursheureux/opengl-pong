#define STB_TRUETYPE_IMPLEMENTATION
#include "render.h"
#include "utils.h"
#include "glad/glad.h"

unsigned int LoadShaderFromSource(int type, const char* source)
{
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    int success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        int logsize;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logsize);
        char* infolog = (char*)malloc(logsize);
        glGetShaderInfoLog(shader, logsize, NULL, infolog);
        if (type == GL_VERTEX_SHADER) {
            fprintf(stderr, "Error in vertex shader: %s\n", infolog);
        } else if (type == GL_FRAGMENT_SHADER) {
            fprintf(stderr, "Error in fragment shader: %s\n", infolog);
        } else {
            fprintf(stderr, "Error in unknown shader: %s\n", infolog);
        }
        return 0;
    }    

    return shader;
}

unsigned int LoadShaderFromFile(int type, const char* path)
{
    char* source = (char*)ReadFile(path);
    if (!source) return 0;

    unsigned int shader = LoadShaderFromSource(type, source);
    free(source);

    return shader;
}

unsigned int CreateShaderProgram(unsigned int vertex, unsigned int fragment)
{
    unsigned int program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);
    
    return program;
}

Texture LoadTextureFromMemory(unsigned char* data, int width, int height)
{
    unsigned int id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    // configure
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    return (Texture){id, width, height};
}

Texture LoadTextureFromFile(const char* path)
{
    int width, height, channels;
    unsigned char* data = stbi_load(path, &width, &height, &channels, 4);

    if (data == NULL) {
        fprintf(stderr, "Failed to load texture: %s\n", path);
        return (Texture){0};
    }

    Texture tex = LoadTextureFromMemory(data, width, height);
    stbi_image_free(data);

    return tex;
}

Font LoadFontFromMemory(unsigned char* data, int size)
{
    stbtt_fontinfo font;
    if (stbtt_InitFont(&font, data, stbtt_GetFontOffsetForIndex(data, 0)) == 0) {
        fprintf(stderr, "Failed to load font");
        return (Font){0};
    }
    const char* codepoints = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    int total_width = 0;
    int total_height = 0;
    float scale = stbtt_ScaleForPixelHeight(&font, size);
    unsigned char** bitmaps = (unsigned char**)malloc(strlen(codepoints) * sizeof(unsigned char*));
    Glyph* glyphs = (Glyph*)malloc(strlen(codepoints) * sizeof(Glyph));

    for (int i = 0; i < strlen(codepoints); i++)
    {
        int codepoint = (int)codepoints[i];
        int width, height;
        bitmaps[i] = stbtt_GetCodepointBitmap(&font, 0, scale, codepoint, &width, &height, 0, 0);
        if (bitmaps[i] == NULL) {
            fputs("BIIIIIP", stderr);
        }
        if (height > total_height) {
            total_height = height;
        }
        total_width += width;

        glyphs[i].codepoint = codepoint;
        if (i == 0) {
            glyphs[i].xoffset = 0;
        } else {
            glyphs[i].xoffset = glyphs[i-1].xoffset + glyphs[i-1].width;
        }
        glyphs[i].yoffset = 0;
        glyphs[i].width = width;
        glyphs[i].height = height;
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    unsigned char* base_data = (unsigned char*)malloc(total_width * total_height);
    memset(base_data, 0, total_width * total_height);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, total_width, total_height, 0, GL_RED, GL_UNSIGNED_BYTE, base_data);
    free(base_data);

    for (int i = 0; i < strlen(codepoints); i++)
    {
        Glyph glyph = glyphs[i];
        unsigned char* bitmap = bitmaps[i];
        unsigned char* flipped = (unsigned char*)malloc(glyph.width * glyph.height);
        for (int y = 0; y < glyph.height; y++)
        {
            for (int x = 0; x < glyph.width; x++)
            {
                flipped[y * glyph.width + x] = bitmap[(glyph.height-1-y) * glyph.width + x];
            }
        }
        glTexSubImage2D(GL_TEXTURE_2D, 0, glyph.xoffset, glyph.yoffset, glyph.width, glyph.height, GL_RED, GL_UNSIGNED_BYTE, flipped);

        stbtt_FreeBitmap(bitmap, NULL);
        free(flipped);
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    free(bitmaps);

    Font ret;
    ret.font = font;
    ret.texture = (Texture){texture, total_width, total_height};
    ret.glyphs = glyphs;

    return ret;
}

Font LoadFontFromFile(const char* path, int size)
{
    unsigned char* data = ReadFile(path);
    if (data == NULL) {
        fprintf(stderr, "ERROR: Failed to load font: %s\n", path);
        return (Font){0};
    }

    return LoadFontFromMemory(data, size);
}
