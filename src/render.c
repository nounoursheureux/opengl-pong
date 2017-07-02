#define STB_TRUETYPE_IMPLEMENTATION
#include "render.h"
#include "utils.h"
#include "glad/glad.h"

typedef struct RenderState {
    unsigned int shader;
    MiniMatrix projview;
} RenderState;

static RenderState render_state = (RenderState){0};

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

void BeginShader(unsigned int shader)
{
    render_state.shader = shader;
    glUseProgram(shader);
}

void EndShader()
{
    render_state.shader = 0;
    glUseProgram(0);
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
    const char* codepoints = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ 0123456789!";
    int total_width = 0;
    int total_height = 0;
    float scale = stbtt_ScaleForPixelHeight(&font, size);
    unsigned char** bitmaps = (unsigned char**)malloc(strlen(codepoints) * sizeof(unsigned char*));
    Glyph* glyphs = (Glyph*)malloc(strlen(codepoints) * sizeof(Glyph));

    for (int i = 0; i < strlen(codepoints); i++)
    {
        int codepoint = (int)codepoints[i];
        int width, height, xoffset, yoffset, advance, lsb;
        if (!stbtt_FindGlyphIndex(&font, codepoint)) {
            fprintf(stderr, "Codepoint not found in font: U+%04x\n", codepoint);
            continue;
        }

        bitmaps[i] = stbtt_GetCodepointBitmap(&font, 0, scale, codepoint, &width, &height, &xoffset, &yoffset);
        stbtt_GetCodepointHMetrics(&font, codepoint, &advance, &lsb);
        if (height > total_height) {
            total_height = height;
        }
        total_width += width;

        glyphs[i].codepoint = codepoint;
        if (i == 0) {
            glyphs[i].texture_rect.x = 0;
        } else {
            glyphs[i].texture_rect.x = glyphs[i-1].texture_rect.x + glyphs[i-1].texture_rect.w;
        }
        glyphs[i].texture_rect.y = 0;
        glyphs[i].texture_rect.w = width;
        glyphs[i].texture_rect.h = height;
        glyphs[i].xoffset = xoffset;
        glyphs[i].yoffset = yoffset;
        glyphs[i].advance = advance * scale;
        glyphs[i].lsb = (float)lsb * scale;
    }

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
        unsigned char* flipped = (unsigned char*)malloc(glyph.texture_rect.w * glyph.texture_rect.h * 4);
        for (int y = 0; y < glyph.texture_rect.h; y++)
        {
            for (int x = 0; x < glyph.texture_rect.w; x++)
            {
                flipped[4 * (y*glyph.texture_rect.w + x)] = 255;
                flipped[4 * (y*glyph.texture_rect.w + x) + 1] = 255;
                flipped[4 * (y*glyph.texture_rect.w + x) + 2] = 255;
                flipped[4 * (y*glyph.texture_rect.w + x) + 3] = bitmap[(glyph.texture_rect.h-1-y)*glyph.texture_rect.w + x];
            }
        }
        glTexSubImage2D(GL_TEXTURE_2D, 0, glyph.texture_rect.x, glyph.texture_rect.y, glyph.texture_rect.w, glyph.texture_rect.h, GL_RGBA, GL_UNSIGNED_BYTE, flipped);

        stbtt_FreeBitmap(bitmap, NULL);
        free(flipped);
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    free(bitmaps);

    Font ret;
    ret.info = font;
    ret.texture = (Texture){texture, total_width, total_height};
    ret.glyphs = glyphs;
    ret.glyphs_num = strlen(codepoints);
    ret.scale = scale;

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

void DrawText(Font font, const char* text, float x, float y)
{
    int* indices = (int*)malloc(strlen(text) * sizeof(int));
    for (int i = 0; i < strlen(text); i++)
    {
        for (size_t j = 0; j < font.glyphs_num; j++)
        {
            if ((char)font.glyphs[j].codepoint == text[i]) {
                indices[i] = j;
                break;
            }

            if (j == font.glyphs_num - 1) {
                fprintf(stderr, "Glyph not pre-rendered: %c\n", text[i]);
            }
        }
    }

    glBindTexture(GL_TEXTURE_2D, font.texture.id);
    int mvp_loc = glGetUniformLocation(render_state.shader, "mvp");
    MiniVector2 position = {x, y};
    MiniVector2 offset = {0.f, 0.f};

    for (int i = 0; i < strlen(text); i++)
    {
        // get glyph
        Glyph glyph = font.glyphs[indices[i]];

        // calculate coords on screen
        offset.x += glyph.lsb;
        offset.x = roundf(offset.x);
        MiniVector2 glyph_position = MiniVector2Add(position, offset);

        // calculate tex coords
        float newvertices[4 * 5] = {
            0.f, 0.f, 0.f, (float)glyph.texture_rect.x / (float)font.texture.width, (float)glyph.texture_rect.y / (float)font.texture.height, // bottom left
            1.f, 0.f, 0.f, (float)(glyph.texture_rect.x + glyph.texture_rect.w) / (float)font.texture.width, (float)glyph.texture_rect.y / (float)font.texture.height, // bottom right
            1.f, 1.f, 0.f, (float)(glyph.texture_rect.x + glyph.texture_rect.w) / (float)font.texture.width, (float)(glyph.texture_rect.y + glyph.texture_rect.h) / (float)font.texture.height, // top right
            0.f, 1.f, 0.f, (float)glyph.texture_rect.x / (float)font.texture.width, (float)(glyph.texture_rect.y + glyph.texture_rect.h) / (float)font.texture.height, // top left
        };
        glBufferData(GL_ARRAY_BUFFER, sizeof(newvertices), newvertices, GL_DYNAMIC_DRAW);
        offset.x += glyph.advance;
        if (i < strlen(text)-1) {
            offset.x += (float)stbtt_GetCodepointKernAdvance(&font.info, glyph.codepoint, font.glyphs[indices[i+1]].codepoint) * font.scale;
        }

        // upload uniforms
        MiniMatrix model = MiniMatrixMultiply(MiniMatrixTranslate(glyph_position.x + (float)glyph.xoffset, glyph_position.y - (float)(glyph.yoffset + glyph.texture_rect.h), 0.f), MiniMatrixScale(glyph.texture_rect.w, glyph.texture_rect.h, 1.f));
        MiniMatrix mvp = MiniMatrixMultiply(render_state.projview, model);
        glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, mvp.data);
        
        // draw elements
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
    }

    float vertices[4 * 5] = {
        0.f, 0.f, 0.f, 0.f, 0.f, // bottom left
        1.f, 0.f, 0.f, 1.f, 0.f, // bottom right
        1.f, 1.f, 0.f, 1.f, 1.f, // top right
        0.f, 1.f, 0.f, 0.f, 1.f, // top left
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    // TODO: one VAO for text, one VAO for sprites
    // or instanced rendering
}

void SetProjViewMatrix(MiniMatrix mat)
{
    render_state.projview = mat;
}
