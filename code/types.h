#pragma once

struct v2
{
    union
    {
        struct
        {
            float x, y;
        };
    };
};

union v3
{
    struct
    {
        float r,g,b;    
    };
    struct
    {
        float x,y,z;
    };
    struct
    {
        v2 xy;
        float _Ignore0;
    };
    struct
    {
        float _Ignore1;
        v2 yz;
    };
    float E[3];
};

union v4
{
    struct
    {
        float r,g,b,a;    
    };
    struct
    {
        float x,y,z,w;
    };
    struct
    {
        v2 xy;
        float _Ignore0;
        float _Ignore1;
    };
    struct
    {
        v3 xyz;
        float _Ignore2;
    };
};

union m3
{
    v3 R[3];
    float E[3][3];
};

union m4
{
    struct
    {
        v4 R1, R2, R3, R4;
    };
    v4 R[4];
    float E[4][4];
};

static inline v3
operator+(v3 a, v3 b)
{
    v3 Result;
    Result.x = a.x + b.x;
    Result.y = a.y + b.y;
    Result.z = a.z + b.z;

    return(Result);
}

static inline v3
operator-(v3 a, v3 b)
{
    v3 Result;
    Result.x = a.x - b.x;
    Result.y = a.y - b.y;
    Result.z = a.z - b.z;

    return(Result);
}


static v3
operator*(v3 a, float c)
{
    v3 Result;
    Result.x = c * a.x;
    Result.y = c * a.y;
    Result.z = c * a.z;

    return(Result);
}

static v3
operator*(float c, v3 a)
{
    v3 Result;
    Result.x = c * a.x;
    Result.y = c * a.y;
    Result.z = c * a.z;

    return(Result);
}

static v3&
operator*=(v3 &a, float c)
{
    a.x = c * a.x;
    a.y = c * a.y;
    a.z = c * a.z;

    return(a);
}

static float
Inner(v3 A, v3 B)
{
    float Result = A.x*B.x + A.y*B.y + A.z*B.z;
    return(Result);
}

static float
Inner(v4 A, v4 B)
{
    float Result = A.x*B.x + A.y*B.y + A.z*B.z + A.w*B.w;
    return(Result);
}

static inline float
operator*(v3 A, v3 B)
{
    float Result = Inner(A, B);
    return(Result);
}

static inline float
operator*(v4 A, v4 B)
{
    float Result = Inner(A, B);
    return(Result);
}

static v3
V3(float x, float y, float z)
{
    v3 Result = {x, y, z};
    return(Result);
}

static v4
V4(v3 A, float w)
{
    v4 Result = {A.x, A.y, A.z, w};
    return(Result);
}

static v4
V4(float x, float y, float z, float w)
{
    v4 Result = {x, y, z, w};
    return(Result);
}
 
static inline v3
operator*(m3 A, v3 B)
{
    v3 Result;
    Result.x = A.R[0] * B;
    Result.y = A.R[1] * B;
    Result.z = A.R[2] * B;

    return(Result);
}

static inline v4
operator*(m4 A, v4 B)
{
    v4 Result;
    Result.x = A.R[0] * B;
    Result.y = A.R[1] * B;
    Result.z = A.R[2] * B;
    Result.w = A.R[3] * B;

    return(Result);
}

static inline m3
operator*(m3 A, m3 B)
{
    m3 Result = {};
    for(int Y = 0; Y < 3; ++Y)
    {
        for(int X = 0; X < 3; ++X)
        {
            for(int I = 0; I < 3; ++I)
            {
                Result.E[Y][X] += A.E[Y][I] * B.E[I][X];
            }
        }
    }
    return(Result);
}


static inline m4
operator*(m4 A, m4 B)
{
    m4 Result = {};
    for(int Y = 0; Y < 4; ++Y)
    {
        for(int X = 0; X < 4; ++X)
        {
            for(int I = 0; I < 4; ++I)
            {
                Result.E[Y][X] += A.E[Y][I] * B.E[I][X];
            }
        }
    }
    return(Result);
}

