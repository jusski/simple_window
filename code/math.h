#pragma once

#define Min(A, B) A < B ? A : B
#define Max(A, B) A > B ? A : B

static float
Clamp01(float A)
{
    float Result = Max(Min(A, 1.0f), 0.0f);
    return(Result);
}

static float
Clamp01Range(float A, float MinA, float MaxA)
{
    float Result = (A - MinA) / (MaxA - MinA);
    return(Result);
}

static float
ClampRange(float A, float MinRange, float MaxRange, float MinA, float MaxA)
{
    float Result = Clamp01Range(A, MinA, MaxA) * (MaxRange - MinRange) + MinRange;
    return(Result);
}
