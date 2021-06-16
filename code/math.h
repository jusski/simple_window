#pragma once
#include "types.h"

static float
Square(float A)
{
    return(A*A);
}

static float
Clamp(float A, float MinA, float MaxA)
{
    float Result = Max(Min(A, MaxA), MinA);
    return(Result);
}

static float
Clamp01(float A)
{
    float Result = Clamp(A, 0.0f, 1.0f);
    return(Result);
}

static float
RandomUnilateral()
{
    float Result = (float)rand() / RAND_MAX;
    return(Result);
}

static float
RandomBilateral()
{
    float Result = 2*RandomUnilateral() - 1.0f;
    return(Result);
}

static float
RandomRange(float Min, float Max)
{
    float Result = RandomUnilateral() * (Max - Min) + Min;
    return(Result);
}

float Radians(float Angle)
{
    float Result = Angle * Pi / 180.0f;
    return(Result);
}

static m4
ZRotate(float Angle)
{
    float C = cosf(Angle);
    float S = sinf(Angle);
    m4 Result =
    {
        C,-S, 0, 0,
        S, C, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };

    return(Result);
}

static m4
XRotate(float Angle)
{
    float C = cosf(Angle);
    float S = sinf(Angle);
    m4 Result =
    {
        1, 0, 0, 0,
        0, C,-S, 0,
        0, S, C, 0,
        0, 0, 0, 1
    };

    return(Result);
}


static m4
YRotate(float Angle)
{
    float C = cosf(Angle);
    float S = sinf(Angle);
    m4 Result =
    {
        C, 0, S, 0,
        0, 1, 0, 0,
       -S, 0, C, 0,
        0, 0, 0, 1
    };

    return(Result);
}

static m3
Scale3(float A)
{
    m3 Result =
    {
        A, 0, 0,
        0, A, 0,
        0, 0, A
    };

    return(Result);
}

static m4
Scale(float A)
{
    m4 Result =
    {
        A, 0, 0, 0,
        0, A, 0, 0,
        0, 0, A, 0,
        0, 0, 0, 1
    };

    return(Result);
}

static m4
XTranslate(float A)
{
    m4 Result =
    {
        1, 0, 0, A,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };

    return(Result);
}


static m4
YTranslate(float A)
{
    m4 Result =
    {
        1, 0, 0, 0,
        0, 1, 0, A,
        0, 0, 1, 0,
        0, 0, 0, 1
    };

    return(Result);
}


static m4
ZTranslate(float A)
{
    m4 Result =
    {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, A,
        0, 0, 0, 1
    };

    return(Result);
}

static m4
Translate(v3 A)
{
    m4 Result =
    {
        1, 0, 0, A.x,
        0, 1, 0, A.y,
        0, 0, 1, A.z,
        0, 0, 0, 1
    };

    return(Result);
}

static m4
Rows(v4 R1, v4 R2, v4 R3, v4 R4)
{
    m4 Result =
    {
        R1,
        R2,
        R3,
        R4
    };
    return(Result);
}

static m4
Columns(v4 C1, v4 C2, v4 C3, v4 C4)
{
    m4 Result =
    {
        C1.x, C2.x, C3.x, C4.x,
        C1.y, C2.y, C3.y, C4.y,
        C1.z, C2.z, C3.z, C4.z,
        C1.w, C2.w, C3.w, C4.w,
    };
    return(Result);
}

static float
Sqrt(float Value)
{
    float Result = Value >= 0.0f ? sqrtf(Value) : 0.0f;
    return(Result);
}

static float
Length(v3 Vector)
{
    float Result = Sqrt(Inner(Vector, Vector));
    return(Result);
}

static v3
NOZ(v3 Vector)
{
    v3 Result = {};

    float L = Length(Vector);
    if(L != 0.0f)
    {
        Result = 1.0f / L * Vector;
    }

    return(Result);
}


static  v3
Cross(v3 A, v3 B)
{
    v3 Result = {};

    Result.x = A.y * B.z - A.z * B.y;
    Result.y = A.z * B.x - A.x * B.z;
    Result.z = A.x * B.y - A.y * B.x;
    
    return(Result);
}

