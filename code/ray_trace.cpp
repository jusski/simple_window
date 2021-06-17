#include "types.h"
#include "engine.h"
#include <float.h>

static object3d *
RayTrace(v3 RayOrigin, v3 RayDirection, object3d *Objects, int Count)
{
    object3d *Result = 0;
    float HitDistance = FLT_MAX;

    for(int ObjectIndex = 0; ObjectIndex < Count; ++ObjectIndex)
    {
        object3d *Object = Objects + ObjectIndex;
        v3 ObjectOrigin = (Object->WorldSpaceTransform * V4(Object->Origin, 1)).xyz;
        v3 RelativeRayOrigin = RayOrigin - ObjectOrigin;

        float a = Inner(RayDirection, RayDirection);
        float b = 2*Inner(RayDirection, RelativeRayOrigin);
        float c = Inner(RelativeRayOrigin, RelativeRayOrigin) - Object->SquaredRadius;

        float RootTerm = b*b - 4*a*c;
        float Denom = 2*a;
        if(RootTerm > 0)
        {
            float t1 = (-b + sqrtf(RootTerm)) / Denom;
            float t2 = (-b - sqrtf(RootTerm)) / Denom;

            float t = t1;
            if((t2 > 0) && (t2 < t1))
            {
                t = t2;
            }

            if((t > 0) && (t < HitDistance))
            {
                HitDistance = t;
                Result = Object;
            }
        }
        
    }
    return(Result);
}
