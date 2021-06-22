#include "types.h"
type_print *PlatformPrint;

#define PrintLine(Name) PlatformPrint_(#Name, Name) 
static short Row = 0;
static void
PlatformPrint_(const char *Name, v4 Value)
{
    PlatformPrint(Row++, "%20s: %5.2f %5.2f %5.2f, %5.2f\t\t", Name, Value.x, Value.y, Value.z, Value.w);
}

static void
PlatformPrint_(const char *Name, v3 Value)
{
    PlatformPrint(Row++, "%20s: %5.2f %5.2f %5.2f\t\t", Name, Value.x, Value.y, Value.z);
}


static void
PlatformPrint_(const char *Name, v2 Value)
{
    PlatformPrint(Row++, "%20s: %5.2f %5.2f\t\t", Name, Value.x, Value.y);
}


static void
PlatformPrint_(const char *Name, float Value)
{
    PlatformPrint(Row++, "%20s: %5.2f\t\t", Name, Value);
}


static void
PlatformPrint_(const char *Name, m4 Value)
{
    PlatformPrint(Row++, "");
    PlatformPrint_(Name, Value.R1);
    PlatformPrint_("", Value.R2);
    PlatformPrint_("", Value.R3);
    PlatformPrint_("", Value.R4);
}

static void
PlatformPrint_(const char *Name, int Value)
{
    PlatformPrint(Row++, "%s %d\t\t", Name, Value);
}






