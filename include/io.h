#ifndef IO_H_
#define IO_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "definitions.h"
#include "constants.h"

typedef struct
{
    uint8_t inputSwitch[MAX_STATE_SIZE][MAX_STATE_SIZE];
    uint16_t coinCount;
    uint16_t analogueChannel[MAX_STATE_SIZE];
    uint16_t rotaryChannel[MAX_STATE_SIZE];
} JVSState;

typedef struct
{
    uint8_t players;
    uint8_t switches;
    uint8_t coins;
    uint8_t analogueInChannels;
    uint8_t analogueInBits;
    uint8_t rotaryChannels;
    uint8_t keypad;
    uint8_t gunChannels;
    uint8_t gunXBits;
    uint8_t gunYBits;
    uint8_t generalPurposeInputs;
    uint8_t card;
    uint8_t hopper;
    uint8_t generalPurposeOutputs;
    uint8_t analogueOutChannels;
    uint8_t displayOutRows;
    uint8_t displayOutColumns;
    uint8_t displayOutEncodings;
    uint8_t backup;
    char *name;

    uint8_t jvsCommandVersion;
    uint8_t jvs_version;
    uint8_t jvs_com_version;

    uint16_t analogueMax;

} JVSCapabilities;

static const JVSCapabilities jvs_io_lindbergh =
    {
        .name = "SEGA CORPORATION;I/O BD JVS;837-14572;Ver1.00;2005/10",
        .players = 2,
        .switches = 0xE,
        .jvsCommandVersion = 0x13,
        .jvs_version = 0x20,
        .jvs_com_version = 0x10,
        .analogueInChannels = 8,
        .analogueInBits = 10,
        .rotaryChannels = 8, // is this right?
        .coins = 2,
        .generalPurposeOutputs = 20,
};

static const JVSCapabilities jvs_io_naomi =
    {
        .name = "OpenJVS Emulator;I/O BD JVS;837-13551;Ver1.00;98/10",
        .players = 2,
        .switches = 16, // @Bobby: Why 16?
        .jvsCommandVersion = 0x10,
        .jvs_version = 0x11,
        .jvs_com_version = 0x10,
        .analogueInChannels = 8,
        .analogueInBits = 8,
        .rotaryChannels = 8, // is this right?
        .coins = 2,
        .generalPurposeOutputs = 6,
};

JVSStatus initIO(/* JVSCapabilities *capabilitiesSetup*/);
int setSwitch(int player, int switchNumber, int value);
int incrementCoin();
int setAnalogue(int channel, double value);
int setRotary(int channel, double value);
void controlPrintStatus();
JVSState *getState();

#endif // DEVICE_H_
