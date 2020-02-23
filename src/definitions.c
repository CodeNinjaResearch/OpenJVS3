#include "definitions.h"

MODE modeStringToEnum(const char *str)
{
    int j;
    for (j = 0; j < sizeof(modeConversion) / sizeof(modeConversion[0]); ++j)
        if (!strcmp(str, modeConversion[j].str))
            return modeConversion[j].val;
    printf("mapping.c:modeStringToEnum: no such string\n");
    return 0;
}

const char *modeEnumToString(MODE mode)
{
    int j;
    for (j = 0; j < sizeof(modeConversion) / sizeof(modeConversion[0]); ++j)
        if (mode == modeConversion[j].val)
            return modeConversion[j].str;
    printf("mapping.c:modeEnumToString: no such Enum\n");
    return 0;
}
