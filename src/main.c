#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#define RAYMATH_IMPLEMENTATION
#include "raymath.h"

void error_callback(int err, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

char* ReadFile(const char* path)
{
    FILE* fp = fopen(path, "rb");
    if (!fp) return NULL;

    fseek(fp, 0, SEEK_END);
    int size = ftell(fp) + 1;
    rewind(fp);

    char* buf = (char*)malloc(size);
    fread(buf, 1, size, fp);
    buf[size-1] = 0;

    fclose(fp);
    return buf;
}

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

float deg2rad(int deg)
{
    return (float)deg * M_PI / 180.f;
}

typedef struct {
    Vector2 position;
    float radius;

    Vector2 velocity;
} Ball;

Ball InitBall()
{
    Ball ball;
    ball.position = (Vector2){400.f, 300.f};
    ball.radius = 5.f;
    int angle_deg = rand() % 360; // angle in degrees
    float angle = deg2rad(angle_deg);
    ball.velocity = (Vector2){cos(angle) * 10.f, sin(angle) * 10.f};

    return ball;
}

typedef struct {
    Vector2 position;
    Vector2 size;
    Vector2 velocity;
    unsigned int score;
} Paddle;

Paddle InitPaddle(float x, float y)
{
    return (Paddle){(Vector2){x, y}, (Vector2){10.f, 100.f}, (Vector2){0.f, 0.f}, 0};
}

typedef struct {
    Ball ball;
    Paddle paddles[2];
} GameState;

GameState InitGameState()
{
    GameState state;
    state.ball = InitBall();
    state.paddles[0] = InitPaddle(0.f, 300.f);
    state.paddles[1] = InitPaddle(790.f, 300.f);

    return state;
}

Paddle CheckPaddleCollision(Paddle paddle)
{
    Paddle newpaddle = paddle;

    if (paddle.position.y < 0) {
        newpaddle.position.y = 0;
    } else if (paddle.position.y + paddle.size.y > 600.f) {
        newpaddle.position.y = 600.f - paddle.size.y;
    }

    return newpaddle;
}

Ball CheckBallWallCollision(Ball ball)
{
    Ball newball = ball;

    /* if (ball.position.x - ball.radius < 0.f || ball.position.x + ball.radius > 800.f) {
        newball.velocity.x = -ball.velocity.x;
    } */

    if (ball.position.y - ball.radius < 0.f || ball.position.y + ball.radius > 600.f) {
        newball.velocity.y = -ball.velocity.y;
    }

    return newball;
}

Ball CheckBallPaddleCollision(Ball ball, Paddle paddle)
{
    Ball newball = ball;

    if (ball.position.x > paddle.position.x && ball.position.x < paddle.position.x + paddle.size.x && ball.position.y > paddle.position.y && ball.position.y < paddle.position.y + paddle.size.y) {
        newball.velocity.x = -ball.velocity.x;
    }

    return newball;
}

void NewSet(GameState* state)
{
    state->ball = InitBall();
    state->paddles[0].position = (Vector2){0.f, 300.f};
    state->paddles[1].position = (Vector2){790.f, 300.f};
}

void UpdateGame(GameState* state, GLFWwindow* window)
{
    state->paddles[0].velocity.y = 0.f;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        state->paddles[0].velocity.y += 10.f;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        state->paddles[0].velocity.y -= 10.f;
    }

    state->paddles[1].velocity.y = 0.f;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        state->paddles[1].velocity.y += 10.f;
    }
    if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        state->paddles[1].velocity.y -= 10.f;
    }

    state->ball.position = Vector2Add(state->ball.position, state->ball.velocity);
    state->paddles[0].position = Vector2Add(state->paddles[0].position, state->paddles[0].velocity);
    state->paddles[1].position = Vector2Add(state->paddles[1].position, state->paddles[1].velocity);

    // Collision between paddle and walls
    state->paddles[0] = CheckPaddleCollision(state->paddles[0]);
    state->paddles[1] = CheckPaddleCollision(state->paddles[1]);

    // Collision between ball and walls
    state->ball = CheckBallWallCollision(state->ball);
    state->ball = CheckBallPaddleCollision(state->ball, state->paddles[0]);
    state->ball = CheckBallPaddleCollision(state->ball, state->paddles[1]);

    if (state->ball.position.x < 0) {
        state->paddles[1].score++;
        NewSet(state);
    } else if (state->ball.position.x > 800.f) {
        state->paddles[0].score++;
        NewSet(state);
    }
}

int main()
{
    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) return 1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    int window_width = 800;
    int window_height = 600;
    GLFWwindow* window = glfwCreateWindow(window_width, window_height, "OpenGL pong", NULL, NULL);
    if (window == NULL) return 1;

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    glfwSwapInterval(1); // vsync

    float vertices[4 * 3] = {
        -1.f, -1.f, 0.f, // bottom left
        1.f, -1.f, 0.f, // bottom right
        1.f, 1.f, 0.f, // top right
        -1.f, 1.f, 0.f, // top left
    };

    unsigned int indexes[6] = {
        0, 1, 3,
        3, 1, 2,
    };

    unsigned int vao, vbo, ebo;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexes), indexes, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    unsigned int vertex_shader = LoadShaderFromFile(GL_VERTEX_SHADER, "res/shaders/base.vs");
    unsigned int rectangle_shader = LoadShaderFromFile(GL_FRAGMENT_SHADER, "res/shaders/rectangle.fs");
    unsigned int circle_shader = LoadShaderFromFile(GL_FRAGMENT_SHADER, "res/shaders/circle.fs");
    unsigned int rectangle_program = CreateShaderProgram(vertex_shader, rectangle_shader);
    unsigned int circle_program = CreateShaderProgram(vertex_shader, circle_shader);

    glClearColor(0.1f, 0.1f, 0.1f, 1.f);

    srand(time(NULL));
    GameState state = InitGameState();

    const double frame_time = 1.0 / 60.0;
    double last_frame = glfwGetTime();
    double elapsed = 0.0;

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        double current_time = glfwGetTime();
        elapsed += current_time - last_frame;
        last_frame = current_time;

        while (elapsed >= frame_time)
        {
            UpdateGame(&state, window);
            elapsed -= frame_time;
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(vao);

        // draw ball
        glUseProgram(circle_program);
        int center_loc = glGetUniformLocation(circle_program, "center");
        int radius_loc = glGetUniformLocation(circle_program, "radius");
        glUniform2fv(center_loc, 1, (float*)&state.ball.position);
        glUniform1f(radius_loc, state.ball.radius);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);

        // draw paddles
        glUseProgram(rectangle_program);
        int position_loc = glGetUniformLocation(rectangle_program, "position");
        int size_loc = glGetUniformLocation(rectangle_program, "size");

        // paddle 1
        glUniform2fv(position_loc, 1, (float*)&state.paddles[0].position);
        glUniform2fv(size_loc, 1, (float*)&state.paddles[0].size);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);

        // paddle 2
        glUniform2fv(position_loc, 1, (float*)&state.paddles[1].position);
        glUniform2fv(size_loc, 1, (float*)&state.paddles[1].size);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);

        glBindVertexArray(0);
        glfwSwapBuffers(window);
    }

    return 0;
}
