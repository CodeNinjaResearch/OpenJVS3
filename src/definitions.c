#include "definitions.h"

MODE modeStringToEnum(const char *str)
{
  int j;
  for (j = 0; j < sizeof(modeConversion) / sizeof(modeConversion[0]); ++j)
  {
    if (strcmp(str, modeConversion[j].str))
    {
      return modeConversion[j].val;
    }
    else
    {
      printf("mapping.c:modeStringToEnum: no such enum for string (%s)\n", str);
      return 0;
    }
  }
}

const char *modeEnumToString(MODE mode)
{
    int j;
    for (j = 0; j < sizeof(modeConversion) / sizeof(modeConversion[0]); ++j)
        if (mode == modeConversion[j].val)
            return modeConversion[j].str;
    printf("mapping.c:modeEnumToString: no such string for enum %d \n", (int)mode);
    return 0;
}
