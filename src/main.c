#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "utils.h"
#define STB_IMAGE_IMPLEMENTATION
#include "render.h"
#include "minimath.h"

void error_callback(int err, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

const char* gl_error_to_string(int err)
{
    switch (err) {
        case GL_INVALID_ENUM: return "GL_INVALID_ENUM";
        case GL_INVALID_VALUE: return "GL_INVALID_VALUE";
        case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";
        // case GL_STACK_OVERFLOW: return "GL_STACK_OVERFLOW";
        // case GL_STACK_UNDERFLOW: return "GL_STACK_UNDERFLOW";
        case GL_OUT_OF_MEMORY: return "GL_OUT_OF_MEMORY";
        // case GL_CONTEXT_LOST: return "GL_CONTEXT_LOST";
        default: return "Unknown error code";
    }
}

typedef struct {
    MiniVector2 position;
    float radius;

    MiniVector2 velocity;
} Ball;

Ball InitBall()
{
    Ball ball;
    ball.position = (MiniVector2){400.f, 300.f};
    ball.radius = 5.f;
    int angle_deg = rand() % 360; // angle in degrees
    float angle = deg2rad((float)angle_deg);
    ball.velocity = (MiniVector2){cos(angle) * 10.f, sin(angle) * 10.f};

    return ball;
}

typedef struct {
    MiniVector2 position;
    MiniVector2 size;
    MiniVector2 velocity;
    unsigned int score;
} Paddle;

Paddle InitPaddle(float x, float y)
{
    return (Paddle){(MiniVector2){x, y}, (MiniVector2){20.f, 100.f}, (MiniVector2){0.f, 0.f}, 0};
}

typedef struct {
    Ball ball;
    Paddle paddles[2];
} GameState;

GameState InitGameState()
{
    GameState state;
    state.ball = InitBall();
    state.paddles[0] = InitPaddle(-10.f, 300.f);
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
    state->paddles[0].position = (MiniVector2){-10.f, 300.f};
    state->paddles[1].position = (MiniVector2){790.f, 300.f};
}

void UpdateGame(GameState* state, GLFWwindow* window)
{
    state->paddles[0].velocity.y /= 2.f;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        state->paddles[0].velocity.y = 10.f;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        state->paddles[0].velocity.y = -10.f;
    }

    state->paddles[1].velocity.y /= 2.f;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        state->paddles[1].velocity.y = 10.f;
    }
    if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        state->paddles[1].velocity.y = -10.f;
    }

    state->ball.position = MiniVector2Add(state->ball.position, state->ball.velocity);
    state->paddles[0].position = MiniVector2Add(state->paddles[0].position, state->paddles[0].velocity);
    state->paddles[1].position = MiniVector2Add(state->paddles[1].position, state->paddles[1].velocity);

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

    glViewport(0, 0, window_width, window_height);

    glfwSwapInterval(1); // vsync

    float vertices[4 * 5] = {
        0.f, 0.f, 0.f, 0.f, 0.f, // bottom left
        1.f, 0.f, 0.f, 1.f, 0.f, // bottom right
        1.f, 1.f, 0.f, 1.f, 1.f, // top right
        0.f, 1.f, 0.f, 0.f, 1.f, // top left
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

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    unsigned int vertex_shader = LoadShaderFromFile(GL_VERTEX_SHADER, "res/shaders/base.vs");
    unsigned int rectangle_shader = LoadShaderFromFile(GL_FRAGMENT_SHADER, "res/shaders/rectangle.fs");
    unsigned int circle_shader = LoadShaderFromFile(GL_FRAGMENT_SHADER, "res/shaders/circle.fs");
    unsigned int textured_shader = LoadShaderFromFile(GL_FRAGMENT_SHADER, "res/shaders/textured.fs");
    unsigned int rectangle_program = CreateShaderProgram(vertex_shader, rectangle_shader);
    unsigned int circle_program = CreateShaderProgram(vertex_shader, circle_shader);
    unsigned int textured_program = CreateShaderProgram(vertex_shader, textured_shader);

    stbi_set_flip_vertically_on_load(1);

    Font fira = LoadFontFromFile("res/fonts/FiraSans-Regular.ttf", 64);

    glClearColor(0.1f, 0.1f, 0.1f, 1.f);

    srand(time(NULL));
    GameState state = InitGameState();

    const double frame_time = 1.0 / 60.0;
    double last_frame = glfwGetTime();
    double elapsed = 0.0;

    MiniMatrix proj = MiniMatrixOrtho(0.f, 800.f, 0.f, 600.f, -1.f, 1.f);
    MiniMatrix view = MiniMatrixIdentity();
    /* MiniMatrix model = MiniMatrixScale(1280.f, 549.f, 1.f);
    MiniMatrix mvp = MiniMatrixMultiply(MiniMatrixMultiply(proj, view), model); */

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
        MiniMatrix model = MiniMatrixMultiply(MiniMatrixTranslate(state.ball.position.x - state.ball.radius, state.ball.position.y - state.ball.radius, 0.f), MiniMatrixScale(state.ball.radius * 2.f, state.ball.radius * 2.f, 1.f));
        MiniMatrix mvp = MiniMatrixMultiply(MiniMatrixMultiply(proj, view), model);

        glUseProgram(circle_program);
        int mvp_loc = glGetUniformLocation(circle_program, "mvp");
        int center_loc = glGetUniformLocation(circle_program, "center");
        int radius_loc = glGetUniformLocation(circle_program, "radius");
        glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, mvp.data);
        glUniform2fv(center_loc, 1, (float*)&state.ball.position);
        glUniform1f(radius_loc, state.ball.radius);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);

        // draw paddles
        glUseProgram(rectangle_program);
        mvp_loc = glGetUniformLocation(rectangle_program, "mvp");
        int position_loc = glGetUniformLocation(rectangle_program, "position");
        int size_loc = glGetUniformLocation(rectangle_program, "size");
        int time_loc = glGetUniformLocation(rectangle_program, "time");
        int speed_loc = glGetUniformLocation(rectangle_program, "speed");

        // paddle 1
        model = MiniMatrixMultiply(MiniMatrixTranslate(state.paddles[0].position.x, state.paddles[0].position.y, 0.f), MiniMatrixScale(state.paddles[0].size.x + 20.f, state.paddles[0].size.y, 1.f));
        mvp = MiniMatrixMultiply(MiniMatrixMultiply(proj, view), model);
        glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, mvp.data);
        glUniform2fv(position_loc, 1, (float*)&state.paddles[0].position);
        glUniform2fv(size_loc, 1, (float*)&state.paddles[0].size);
        glUniform1f(time_loc, current_time);
        glUniform1f(speed_loc, MiniVector2Length(state.paddles[0].velocity));
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);

        // paddle 2
        model = MiniMatrixMultiply(MiniMatrixTranslate(state.paddles[1].position.x - 20.f, state.paddles[1].position.y, 0.f), MiniMatrixScale(state.paddles[1].size.x + 20.f, state.paddles[1].size.y, 1.f));
        mvp = MiniMatrixMultiply(MiniMatrixMultiply(proj, view), model);
        glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, mvp.data);
        glUniform2fv(position_loc, 1, (float*)&state.paddles[1].position);
        glUniform2fv(size_loc, 1, (float*)&state.paddles[1].size);
        glUniform1f(time_loc, current_time);
        glUniform1f(speed_loc, MiniVector2Length(state.paddles[1].velocity));
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);

        // score
        glUseProgram(textured_program);
        glBindTexture(GL_TEXTURE_2D, fira.texture.id);
        mvp_loc = glGetUniformLocation(textured_program, "mvp");
        model = MiniMatrixScale(fira.texture.width, fira.texture.height, 1.f);
        mvp = MiniMatrixMultiply(MiniMatrixMultiply(proj, view), model);
        glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, mvp.data);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);

        glBindVertexArray(0);
        glfwSwapBuffers(window);

        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            fprintf(stderr, "%s\n", gl_error_to_string(err));
        }
    }

    return 0;
}
