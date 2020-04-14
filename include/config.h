#ifndef CONFIG_H_
#define CONFIG_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "definitions.h"

#define DEFAULT_LOCATION "/etc/openjvs"
#define DEFAULT_GLOBAL_CONFIG "/global_config"
#define DEFAULT_DEFAULT_CONFIG "/maps/default_config"
#define DEFAULT_DEVICE_MAP "docs/maps/device/"

#define MAX_STRING_LENGTH 1024

typedef struct JVSConfig
{
    char devicePath[MAX_STRING_LENGTH];
    int syncType;
    char defaultMapping[MAX_STRING_LENGTH];
    char defaultIO[MAX_STRING_LENGTH];
    int debugMode;
} JVSConfig;

JVSStatus processConfig(char *filePath, JVSConfig *config);
int processInMapFile(char *filePath, MappingIn *mappingIn);
int processOutMapFile(char *filePath, MappingOut *mappingOut);

void print_mapping_in(MappingIn *mappingIn);

#endif // CONFIG_H_
