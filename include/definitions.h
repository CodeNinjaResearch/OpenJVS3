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
    BUTTON_UP,
    BUTTON_DOWN,
    BUTTON_LEFT,
    BUTTON_RIGHT,
    BUTTON_0,
    BUTTON_1,
    BUTTON_2,
    BUTTON_3,
    BUTTON_4,
    BUTTON_5,
    BUTTON_6,
    BUTTON_7,
    BUTTON_8,
    BUTTON_9,
    BUTTON_10,
    BUTTON_11,
    BUTTON_12,
    BUTTON_13,
    BUTTON_14,
    BUTTON_15,
    BUTTON_16,
    SYSTEM_1,
    SYSTEM_2,
    SYSTEM_3,
    SYSTEM_4,
    SYSTEM_5,
    SYSTEM_6,
    SYSTEM_7,
    SYSTEM_8,
    COIN
} Mode;

const static struct
{
    Mode val;
    const char *str;
} modeConversion[] = {
    {WHEEL, "WHEEL"},
    {ACCELERATOR, "ACCELERATOR"},
    {BREAK, "BREAK"},
    {SERVICE, "SERVICE"},
    {TEST, "TEST"},
    {GEAR_UP, "GEAR_UP"},
    {GEAR_DOWN, "GEAR_DOWN"},
    {BUTTON_UP, "BUTTON_UP"},
    {BUTTON_DOWN, "BUTTON_DOWN"},
    {BUTTON_LEFT, "BUTTON_LEFT"},
    {BUTTON_RIGHT, "BUTTON_RIGHT"},
    {BUTTON_0, "BUTTON_0"},
    {BUTTON_1, "BUTTON_1"},
    {BUTTON_2, "BUTTON_2"},
    {BUTTON_3, "BUTTON_3"},
    {BUTTON_4, "BUTTON_4"},
    {BUTTON_5, "BUTTON_5"},
    {BUTTON_6, "BUTTON_6"},
    {BUTTON_7, "BUTTON_7"},
    {BUTTON_8, "BUTTON_8"},
    {BUTTON_9, "BUTTON_9"},
    {BUTTON_10, "BUTTON_10"},
    {BUTTON_11, "BUTTON_11"},
    {BUTTON_12, "BUTTON_12"},
    {BUTTON_13, "BUTTON_13"},
    {BUTTON_14, "BUTTON_14"},
    {BUTTON_15, "BUTTON_15"},
    {BUTTON_16, "BUTTON_16"},
    {SYSTEM_1, "SYSTEM_1"},
    {SYSTEM_2, "SYSTEM_2"},
    {SYSTEM_3, "SYSTEM_3"},
    {SYSTEM_4, "SYSTEM_4"},
    {SYSTEM_5, "SYSTEM_5"},
    {SYSTEM_6, "SYSTEM_6"},
    {SYSTEM_7, "SYSTEM_7"},
    {SYSTEM_8, "SYSTEM_8"},
    {COIN, "COIN"}};

typedef enum
{
    KEY,
    ABS,
} InType;

typedef enum
{
    NONE,
    SYSTEM,
    BUTTON,
    ANALOGUE,
    ROTARY,
    GUN
} OutType;

typedef struct
{
    int channel;
    InType type;
    Mode mode;
    int min;
    int max;
    int reverse;
} MappingIn;

typedef struct
{
    int channel;
    OutType type;
    Mode mode;
    int min;
    int max;
    int reverse;
} MappingOut;

Mode modeStringToEnum(const char *str);
const char *modeEnumToString(Mode mode);

#endif // DEFINITIONS_H_
