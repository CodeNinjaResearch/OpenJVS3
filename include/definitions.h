#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_

#include <stdio.h>
#include <string.h>


#define OFFLINE_MODE

#define min(x,y) ((x) <= (y)) ? (x) : (y)

typedef enum
{
        OPEN_JVS_ERR_OK = 0,
        /* Errors that indicates something is not working */
        OPEN_JVS_ERR_REC_BUFFER,
        OPEN_JVS_ERR_SERIAL_READ,
        OPEN_JVS_ERR_SERIAL_WRITE,
        OPEN_JVS_ERR_STATE,
        OPEN_JVS_ERR_CHECKSUM,
        OPEN_JVS_ERR_NULL,
        OPEN_JVS_ERR_INVALID_CMD,
        OPEN_JVS_ERR_OFFLINE,
 //       OPEN_JVS_ERR_REPORT,
        OPEN_JVS_ERR_JVS_PROFILE_NULL,
        OPEN_JVS_ERR_ANALOG_MASK,
        OPEN_JVS_ERR_PACKET_BUFFER_OVERFLOW,

        /* Errors/Status that are fine */
        OPEN_JVS_ERR_TIMEOUT,
        OPEN_JVS_ERR_SYNC_BYTE,
        OPEN_JVS_NO_RESPONSE,
        OPEN_JVS_ERR_WAIT_BYTES,

} open_jvs_status_t;


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
} modeConversion[] =
{
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
