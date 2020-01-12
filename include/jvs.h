#ifndef JVS_H_
#define JVS_H_

#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <time.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>

#include "constants.h"

typedef struct
{
    unsigned char destination;
    unsigned char length;
    unsigned char data[MAX_PACKET_SIZE];
} JVSPacket;

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
} JVSCapabilities;

int connectJVS();
int readByte(unsigned char *byte);
int writeByte(unsigned char byte);
int readPacket(JVSPacket *packet);
int writePacket(JVSPacket *packet);
int processPacket();
int setSyncPin(int a);
int setSerialAttributes(int fd, int speed);

#endif // JVS_H_
