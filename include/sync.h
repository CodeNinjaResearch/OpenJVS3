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

#define sync_pin 12

int SyncPinInit(void);
void SyncPinLow(bool pull_low);

#define IN 0
#define OUT 1

#define LOW 0
#define HIGH 1

int GPIOUnexport(int pin);
int GPIOExport(int pin);
int GPIODirection(int pin, int dir);
int GPIORead(int pin);
int GPIOWrite(int pin, int value);

void set_debug_pin(uint8_t b);
void init_debug_pin(void);

#endif // SYNC_H_
