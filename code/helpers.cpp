#define PrintLine(Name) PlatformPrint_(#Name, Name)

static void
PlatformPrint_(const char *Name, v4 Value)
{
    PlatformPrint("%20s: %5.2f %5.2f %5.2f, %5.2f", Name, Value.x, Value.y, Value.z, Value.w);
}

static void
PlatformPrint_(const char *Name, v3 Value)
{
    PlatformPrint("%20s: %5.2f %5.2f %5.2f", Name, Value.x, Value.y, Value.z);
}


static void
PlatformPrint_(const char *Name, v2 Value)
{
    PlatformPrint("%20s: %5.2f %5.2f", Name, Value.x, Value.y);
}


static void
PlatformPrint_(const char *Name, float Value)
{
    PlatformPrint("%20s: %5.2f", Name, Value);
}


static void
PlatformPrint_(const char *Name, m4 Value)
{
    PlatformPrint_(Name, Value.R1);
    PlatformPrint_(Name, Value.R2);
    PlatformPrint_(Name, Value.R3);
    PlatformPrint_(Name, Value.R4);
}






