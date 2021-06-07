#pragma once

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

static m3
ZRotate(float Angle)
{
    float C = cosf(Angle);
    float S = sinf(Angle);
    m3 Result =
    {
        C, -S, 0,
        S,  C, 0,
        0,  0, 1
    };

    return(Result);
}

static m3
XRotate(float Angle)
{
    float C = cosf(Angle);
    float S = sinf(Angle);
    m3 Result =
    {
        1, 0,  0,
        0, C, -S,
        0, S,  C
    };

    return(Result);
}


static m3
YRotate(float Angle)
{
    float C = cosf(Angle);
    float S = sinf(Angle);
    m3 Result =
    {
        C, 0, -S,
        0, 1,  0,
        S, 0,  C
    };

    return(Result);
}
