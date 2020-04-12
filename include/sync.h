#ifndef SYNC_H_
#define SYNC_H_

#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include "definitions.h"

#define sync_pin 12

#define IN 0
#define OUT 1

#define LOW 0
#define HIGH 1

typedef enum
{
    SYNC_CIRCUIT_FLOAT,
    SYNC_CIRCUIT_SWITCH,
    SYNC_NONE,

    /* data */
} SYNC_CIRCUIT;

int SyncPinInit(void);
int SyncPinLow(bool pull_low);
open_jvs_status_t SyncAlgorithmSet(SYNC_CIRCUIT circ_type);

int GPIOUnexport(int pin);
int GPIOExport(int pin);
int GPIODirection(int pin, int dir);
int GPIORead(int pin);
int GPIOWrite(int pin, int value);

void set_debug_pin(uint8_t b);
void init_debug_pin(void);

#endif // SYNC_H_
