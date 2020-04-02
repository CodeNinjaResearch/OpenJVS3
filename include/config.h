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

int processConfig(char *filePath);
int processInMapFile(char *filePath, MappingIn *mappingIn);
int processOutMapFile(char *filePath, MappingOut *mappingOut);

void print_mapping_in(MappingIn *mappingIn);

#endif // CONFIG_H_
