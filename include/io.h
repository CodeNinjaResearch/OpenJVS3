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

    // todo: fill init
    uint8_t jvs_command_version;
    uint8_t jvs_version;
    uint8_t jvs_com_version;

    uint16_t analogueMask;
    uint16_t analogueMax;

} JVSCapabilities;


//    capabilities.name = "OpenJVS3 Emulator;I/O BD JVS;837-13551;Ver1.00;98/10";
//    capabilities.players = 2;
//    capabilities.switches = 8;
//    capabilities.analogueInBits = 8;
//    capabilities.analogueInChannels = 8;
//    capabilities.rotaryChannels = 8;
//    capabilities.coins = 1;



static const JVSCapabilities jvs_io_lindbergh =
{
    .name = "SEGA CORPORATION;I/O BD JVS;837-14572;Ver1.00;2005/10",
    .players = 2,
    .switches = 0xE,
    .jvs_command_version = 0x13,
    .jvs_version = 0x20,
    .jvs_com_version = 0x10,
    .analogueInChannels = 8,
    .analogueInBits = 16,
    .rotaryChannels = 8, // is this right?
    .coins = 2,
};

static const JVSCapabilities jvs_io_naomi=
{
    .name = "OpenJVS Emulator;I/O BD JVS;837-13551;Ver1.00;98/10",
    .players = 2,
    .switches = 0xD,
    .jvs_command_version = 0x10,
    .jvs_version = 0x11,
    .jvs_com_version = 0x10,
    .analogueInChannels = 8,
    .analogueInBits = 8,
    .rotaryChannels = 8, // is this right?
    .coins = 2,
};

//// todo: @Bobby: I did not check these values but just copied them from the old source - are these verified?
//static const jvs_io_t jvs_io_naomi=
//{
//    .jvs_id_str = "OpenJVS Emulator;I/O BD JVS;837-13551;Ver1.00;98/10",
//    .jvs_cmd_revision = 0x11,
//    .jvs_standard = 0x30,
//    .jvs_analog_channels = 8,
//    .jvs_analog_number_bits = 8,
//    .jvs_rotary_channels = 8, // is this right?
//    .jvs_coin_slots = 2,
//};






open_jvs_status_t initIO(/* JVSCapabilities *capabilitiesSetup*/);
int setSwitch(int player, int switchNumber, int value);
int incrementCoin();
int setAnalogue(int channel, int value);
int setRotary(int channel, int value);
void controlPrintStatus();


open_jvs_status_t jvs_get_analog_mask(uint16_t * analog_mask);
open_jvs_status_t jvs_set_analog_mask(uint16_t mask);
open_jvs_status_t jvs_get_analog_max(uint16_t * analog_max);
open_jvs_status_t jvs_set_analog_max(uint16_t max);

JVSCapabilities *getCapabilities();
JVSState *getState();

#endif // DEVICE_H_
