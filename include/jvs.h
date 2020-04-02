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

/* Global definitions */


typedef struct
{
//    uint8_t destination;
    uint32_t length;

    uint8_t data[MAX_PACKET_SIZE];
} JVSPacket;

open_jvs_status_t initJVS(char *devicePath, JVSCapabilities *capabilitiesSetup);
int disconnectJVS();
int writeCapabilities(JVSPacket *outputPacket, JVSCapabilities *capabilities);
int readPacket(JVSPacket *packet);
int writePacket(JVSPacket *packet);
open_jvs_status_t processPacket2(JVSPacket * inPacket, JVSPacket * outPacket);
void debug(char *string);

open_jvs_status_t jvs_do(void);

open_jvs_status_t find_start_of_message(circ_buffer_t * read_buffer);
open_jvs_status_t decode_escape_circ (circ_buffer_t * read_buffer , JVSPacket * out_packet_decoded,  uint32_t *out_raw_length);
open_jvs_status_t check_message(JVSPacket * packet);
uint8_t calc_checksum(uint8_t *message, uint8_t len);
open_jvs_status_t encode_escape (JVSPacket * packet);

#endif // JVS_H_
