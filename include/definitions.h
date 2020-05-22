#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include "buffer.h"

/* Select timeout in ms*/
#define TIMEOUT_SELECT 500

//#define OFFLINE_MODE

#define DEBUG_PIN_ENALBED
#define DEBUG_PIN_ENALBED

#define min(x, y) ((x) <= (y)) ? (x) : (y)

#define STRING_LEN 256
#define NUMBER_INPUTS 32

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
    OPEN_JVS_ERR_ANALOG_BITS,
    OPEN_JVS_ERR_PACKET_BUFFER_OVERFLOW,
    OPEN_JVS_ERR_INVALID_SYNC_CIRCUIT,

    /* Errors/Status that are fine */
    OPEN_JVS_ERR_TIMEOUT,
    OPEN_JVS_ERR_SYNC_BYTE,
    OPEN_JVS_NO_RESPONSE,
    OPEN_JVS_ERR_WAIT_BYTES,

} JVSStatus;

typedef enum
{
    WHEEL,
    ACCELERATOR,
    BREAK,
    ANALOGUE_X,
    ANALOGUE_Y,
    ANALOGUE_Z,
    START,
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
    COIN_1,
    COIN_2,
} Mode;

typedef enum
{
    /* X-Box Pad*/
    LS_X,
    LS_Y,
    LS_BUTTON,
    RS_X,
    RS_Y,
    RS_BUTTON,
    UP,
    DOWN,
    LEFT,
    RIGHT,
    BUTTON_A,
    BUTTON_B,
    BUTTON_C,
    BUTTON_D,
    BUTTON_START,
    BUTTON_BACK,
    BUTTON_HOME,
    BUTTON_LT,
    BUTTON_RT,
    BUTTON_LB,
    BUTTON_RB,

    /* Wheel */

    /* WiiMote */

    /* Lightgun*/

} InputType;

static const struct
{
    Mode val;
    const char *str;
} modeConversion[] = {
    {WHEEL, "WHEEL"},
    {ACCELERATOR, "ACCELERATOR"},
    {BREAK, "BREAK"},
    {ANALOGUE_X, "ANALOGUE_X"},
    {ANALOGUE_Y, "ANALOGUE_Y"},
    {ANALOGUE_Z, "ANALOGUE_Z"},
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
    {START, "START"},
    {COIN_1, "COIN_1"},
    {COIN_2, "COIN_2"}};

typedef enum
{
    KEY,
    ABS
} InType;

typedef enum
{
    NONE,
    SYSTEM,
    BUTTON,
    ANALOGUE,
    ROTARY,
    GUN,
    COIN,
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
    InType type;
    Mode mode;
    int min;
    int max;
    int reverse;
} MappingIn2;

typedef struct
{
    int channel;
    OutType type;
    Mode mode;
    int player;
    int min;
    int max;
    int reverse;
} MappingOut;

typedef enum
{
    JVS_GAME_CAT_JOYSTICK = 0,
    JVS_GAME_CAT_SHOOTING,
    JVS_GAME_CAT_DRIVING,
    JVS_GAME_CAT_ANALOG,
    JVS_GAME_CAT_NUMBER,

} JvsGameCategory;

typedef enum
{
    PLAYER1,
    PLAYER2,
} JvsPlayer;

typedef enum
{
    JVS_BTN_START,
    JVS_BTN_SERVICE,
    JVS_BTN_UP,
    JVS_BTN_DOWN,
    JVS_BTN_LEFT,
    JVS_BTN_RIGHT,
    JVS_BTN_PUSH1,
    JVS_BTN_PUSH2,
    JVS_BTN_PUSH3,
    JVS_BTN_PUSH4,
    JVS_BTN_PUSH5,
    JVS_BTN_PUSH6,
    JVS_BTN_PUSH7,
    JVS_BTN_PUSH8,
    JVS_BTN_TEST,
    JVS_BTN_TILT1,
    JVS_BTN_TILT2,
    JVS_BTN_TILT3,
    JVS_BTN_RESERVED_1,
    JVS_BTN_RESERVED_2,
    JVS_BTN_RESERVED_3
} JvsInputType;

typedef enum
{
    /* Gnernal */
    FUNC_COIN,
    FUNC_START,

    /* Joystick games */
    FUNC_UP,
    FUNC_DOWN,
    FUNC_LEFT,
    FUNC_RIGHT,
    FUNC_PUNSH,
    FUNC_KICK,
    FUNC_GUARD,

    /* Lightgun Shooter */
    FUNC_X_AXIS,
    FUNC_y_AXIS,
    FUNC_TRIGGER,
    FUNC_RELOAD,
    FUNC_GRANDE,
    FUNC_OUT_OF_SCREEN,

    /* Driving */
    FUNC_WHEEL,
    FUNC_GAS,
    FUNC_BREAK,
    FUNC_CHANGE_VIEW,
    FUNC_BOOSTER,
    FUNC_GEAR_UP,
    FUNC_GEAR_DOWN,

} JvsGameFunction;

typedef struct
{
    char Description[STRING_LEN];
    JvsPlayer Player;
    JvsInputType Input;
    JvsGameFunction Function;

} JvsInput;

typedef struct
{
    char name[STRING_LEN];
    char description[STRING_LEN];
    JvsGameCategory category;

    /* Array of type JvsInput*/
    Array inputs;
    //JvsInput inputs[NUMBER_INPUTS];

} JvsGame;

typedef struct
{
    JvsPlayer player;
    /* Array of type MappingIn*/
    Array events;

} InputDevice;

typedef struct
{
    InputType input;
    JvsGameFunction function;

} InputType2Function;

typedef struct
{
    InputType input;
    JvsInputType jvs_input;

} InputType2JvsInput;

typedef struct
{
    /* Arrays of type InputType2Function for each game_type*/
    Array game_type[JVS_GAME_CAT_NUMBER];

} MappingFunction;

typedef struct
{
    /* Name of game */
    char name[STRING_LEN];
    /* Array of InputType2JvsInput */
    Array inputs;

} MappingSpecific;

Mode modeStringToEnum(const char *str);
const char *modeEnumToString(Mode mode);
int setRealtimePriority(bool realtime);

#endif // DEFINITIONS_H_
