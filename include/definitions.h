#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_

#include <stdio.h>
#include <string.h>

typedef enum
{
    WHEEL,
    ACCELERATOR,
    BREAK,
    SERVICE,
    TEST,
    GEAR_UP,
    GEAR_DOWN,
    BUTTON_0,
    BUTTON_1,
    BUTTON_2,
    BUTTON_3,
    BUTTON_4,
    BUTTON_5,
    BUTTON_6,
    BUTTON_7,
    BUTTON_8,
    COIN
} MODE;

const static struct
{
    MODE val;
    const char *str;
} modeConversion[] = {
    {WHEEL, "WHEEL"},
    {ACCELERATOR, "ACCELERATOR"},
    {BREAK, "BREAK"},
    {SERVICE, "SERVICE"},
    {TEST, "TEST"},
    {GEAR_UP, "GEAR_UP"},
    {GEAR_DOWN, "GEAR_DOWN"},
    {BUTTON_0, "BUTTON_0"},
    {BUTTON_1, "BUTTON_1"},
    {BUTTON_2, "BUTTON_2"},
    {BUTTON_3, "BUTTON_3"},
    {BUTTON_4, "BUTTON_4"},
    {BUTTON_5, "BUTTON_5"},
    {BUTTON_6, "BUTTON_6"},
    {BUTTON_7, "BUTTON_7"},
    {BUTTON_8, "BUTTON_8"},
    {COIN, "COIN"}

};

typedef enum
{
    KEY,
    ABS,
} IN_TYPE;

typedef enum
{
    NONE,
    SYSTEM,
    BUTTON,
    ANALOGUE,
    ROTARY,
    GUN
} OUT_TYPE;

typedef struct
{
    int channel;
    IN_TYPE type;
    MODE mode;
    int min;
    int max;
} MappingIn;

typedef struct
{
    int channel;
    OUT_TYPE type;
    MODE mode;
    int min;
    int max;
} MappingOut;

MODE modeStringToEnum(const char *str);
const char *modeEnumToString(MODE mode);

#endif // DEFINITIONS_H_
