#ifndef JVS_H_
#define JVS_H_

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <time.h>
#include <stdarg.h>
#include <sys/ioctl.h>

#include "device.h"
#include "constants.h"
#include "io.h"
#include "config.h"

typedef struct
{
    unsigned char destination;
    unsigned char length;
    unsigned char data[MAX_PACKET_SIZE];
} JVSPacket;


int initJVS(char *devicePath, JVSCapabilities* capabilitiesSetup);
int disconnectJVS();
int writeCapabilities(JVSPacket *outputPacket, JVSCapabilities* capabilities);
int readPacket(JVSPacket *packet);
int writePacket(JVSPacket *packet);
int processPacket();
void debug(char *string);

#endif // JVS_H_
