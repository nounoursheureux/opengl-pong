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
    char* source = ReadFile(path);
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
