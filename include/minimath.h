#ifndef MINI_MATH_H
#define MINI_MATH_H
#include <math.h>

typedef struct MiniVector2 {
    float x, y;
} MiniVector2;

typedef struct MiniMatrix {
    float data[16];
} MiniMatrix;

float deg2rad(float deg)
{
    return deg * M_PI / 180.f;
}

MiniVector2 MiniVector2Add(MiniVector2 a, MiniVector2 b)
{
    return (MiniVector2){a.x + b.x, a.y + b.y};
}

float MiniVector2Length(MiniVector2 vec)
{
    return sqrtf(vec.x*vec.x + vec.y*vec.y);
}

MiniMatrix MiniMatrixIdentity()
{
    MiniMatrix mat;
    mat.data[0] = 1.f;
    mat.data[1] = 0.f;
    mat.data[2] = 0.f;
    mat.data[3] = 0.f;
    mat.data[4] = 0.f;
    mat.data[5] = 1.f;
    mat.data[6] = 0.f;
    mat.data[7] = 0.f;
    mat.data[8] = 0.f;
    mat.data[9] = 0.f;
    mat.data[10] = 1.f;
    mat.data[11] = 0.f;
    mat.data[12] = 0.f;
    mat.data[13] = 0.f;
    mat.data[14] = 0.f;
    mat.data[15] = 1.f;

    return mat;
}

MiniMatrix MiniMatrixOrtho(float left, float right, float bottom, float top, float near, float far)
{
    MiniMatrix mat;
    mat.data[0] = 2.f / (right - left);
    mat.data[1] = 0.f;
    mat.data[2] = 0.f;
    mat.data[3] = 0.f;
    mat.data[4] = 0.f;
    mat.data[5] = 2.f / (top - bottom);
    mat.data[6] = 0.f;
    mat.data[7] = 0.f;
    mat.data[8] = 0.f;
    mat.data[9] = 0.f;
    mat.data[10] = -2 / (far - near);
    mat.data[11] = 0.f;
    mat.data[12] = -1 * (right + left) / (right - left);
    mat.data[13] = -1 * (top + bottom) / (top - bottom);
    mat.data[14] = -1 * (far + near) / (far - near);
    mat.data[15] = 1.f;

    return mat;
}

MiniMatrix MiniMatrixMultiply(MiniMatrix left, MiniMatrix right)
{
    MiniMatrix mat;

    for (int row = 0; row < 4; row++)
    {
    }

    // First row
    mat.data[0] = left.data[0] * right.data[0] + left.data[4] * right.data[1] + left.data[8] * right.data[2] + left.data[12] * right.data[3];
    mat.data[1] = left.data[1] * right.data[0] + left.data[5] * right.data[1] + left.data[9] * right.data[2] + left.data[13] * right.data[3];
    mat.data[2] = left.data[2] * right.data[0] + left.data[6] * right.data[1] + left.data[10] * right.data[2] + left.data[14] * right.data[3];
    mat.data[3] = left.data[3] * right.data[0] + left.data[7] * right.data[1] + left.data[11] * right.data[2] + left.data[15] * right.data[3];

    // Second row
    mat.data[4] = left.data[0] * right.data[4] + left.data[4] * right.data[5] + left.data[8] * right.data[6] + left.data[12] * right.data[7];
    mat.data[5] = left.data[1] * right.data[4] + left.data[5] * right.data[5] + left.data[9] * right.data[6] + left.data[13] * right.data[7];
    mat.data[6] = left.data[2] * right.data[4] + left.data[6] * right.data[5] + left.data[10] * right.data[6] + left.data[14] * right.data[7];
    mat.data[7] = left.data[3] * right.data[4] + left.data[7] * right.data[5] + left.data[11] * right.data[6] + left.data[15] * right.data[7];

    // Third row
    mat.data[8] = left.data[0] * right.data[8] + left.data[4] * right.data[9] + left.data[8] * right.data[10] + left.data[12] * right.data[11];
    mat.data[9] = left.data[1] * right.data[8] + left.data[5] * right.data[9] + left.data[9] * right.data[10] + left.data[13] * right.data[11];
    mat.data[10] = left.data[2] * right.data[8] + left.data[6] * right.data[9] + left.data[10] * right.data[10] + left.data[14] * right.data[11];
    mat.data[11] = left.data[3] * right.data[8] + left.data[7] * right.data[9] + left.data[11] * right.data[10] + left.data[15] * right.data[11];

    // Fourth row
    mat.data[12] = left.data[0] * right.data[12] + left.data[4] * right.data[13] + left.data[8] * right.data[14] + left.data[12] * right.data[15];
    mat.data[13] = left.data[1] * right.data[12] + left.data[5] * right.data[13] + left.data[9] * right.data[14] + left.data[13] * right.data[15];
    mat.data[14] = left.data[2] * right.data[12] + left.data[6] * right.data[13] + left.data[10] * right.data[14] + left.data[14] * right.data[15];
    mat.data[15] = left.data[3] * right.data[12] + left.data[7] * right.data[13] + left.data[11] * right.data[14] + left.data[15] * right.data[15];

    return mat;
}

MiniMatrix MiniMatrixScale(float x, float y, float z)
{
    MiniMatrix mat = MiniMatrixIdentity();
    mat.data[0] = x;
    mat.data[5] = y;
    mat.data[10] = z;

    return mat;
}
#endif
