#ifndef IO_H_
#define IO_H_

#include <stdio.h>
#include <stdlib.h>

#include "constants.h"

typedef struct
{
    char inputSwitch[MAX_STATE_SIZE][MAX_STATE_SIZE];
    int coinCount;
    char analogueChannel[MAX_STATE_SIZE];
    char rotaryChannel[MAX_STATE_SIZE];
} JVSState;

typedef struct
{
    unsigned char players;
    unsigned char switches;
    unsigned char coins;
    unsigned char analogueInChannels;
    unsigned char analogueInBits;
    unsigned char rotaryChannels;
    unsigned char keypad;
    unsigned char gunChannels;
    unsigned char gunXBits;
    unsigned char gunYBits;
    unsigned char generalPurposeInputs;
    unsigned char card;
    unsigned char hopper;
    unsigned char generalPurposeOutputs;
    unsigned char analogueOutChannels;
    unsigned char displayOutRows;
    unsigned char displayOutColumns;
    unsigned char displayOutEncodings;
    unsigned char backup;
    char *name;
} JVSCapabilities;

int initIO(JVSCapabilities *capabilitiesSetup);
int setSwitch(int player, int switchNumber, int value);
int incrementCoin();
int setAnalogue(int channel, int value);
int setRotary(int channel, int value);
void controlPrintStatus();

JVSCapabilities *getCapabilities();
JVSState *getState();

#endif // DEVICE_H_