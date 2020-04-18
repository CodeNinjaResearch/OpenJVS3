#ifndef SENSE_H_
#define SENSE_H_

#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include "definitions.h"

#define SENSE_PIN 12

#define IN 0
#define OUT 1

#define LOW 0
#define HIGH 1

typedef enum
{
    SENSE_FLOAT,
    SENSE_SWITCH,
    SENSE_NONE,
} JVSSenseCircuit;

int SyncPinInit(void);
int SyncPinLow(bool pull_low);
JVSStatus SyncAlgorithmSet(JVSSenseCircuit circuitType);

int GPIOUnexport(int pin);
int GPIOExport(int pin);
int GPIODirection(int pin, int dir);
int GPIORead(int pin);
int GPIOWrite(int pin, int value);

void set_debug_pin(uint8_t b);
void init_debug_pin(void);

#endif // SENSE_H_
