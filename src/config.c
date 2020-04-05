#include <stdio.h>
#include <string.h>

#include "config.h"

void trimToken(char *str, int maxlen)
{
    int length = strnlen(str, maxlen);

    /* Look for unwated paterrns and terminate the token*/
    for (int i = 0; i < length; i++)
    {
        char val = *(str + i);
        if (('\n' == val) || ('\r' == val))
        {
            *(str + i) = '\0';
        }
    }
}

int processConfig(char *filePath)
{
    printf("Processing config from: %s\n", filePath);
}

void print_mapping_in(MappingIn *mappingIn)
{
    if (NULL != mappingIn)
    {
        printf("Type:%u Mode:%u Key/Channel:%u Min:%d Max:%d \n",
               mappingIn->type,
               mappingIn->mode,
               mappingIn->channel,
               mappingIn->min,
               mappingIn->max);
    }
}

int processInMapFile(char *filePath, MappingIn *mappingIn)
{
    int count = 0;
    FILE *fp;
    char buffer[1024];
    if ((fp = fopen(filePath, "r")) != NULL)
    {
        fgets(buffer, 1024, fp);
        while (!feof(fp))
        {
            if ((buffer[0] != '#') && (buffer[0] != 0) && (buffer[0] != '\r') && (buffer[0] != '\n') && (strcmp(buffer, "") != 0))
            {
                char *token = strtok(buffer, " ");
                trimToken(token, sizeof(buffer) - ((unsigned int)((token - buffer))));

                IN_TYPE type = KEY;
                /* KEY <CHANNEL> <MODE> */
                if (strcmp(token, "KEY") == 0 || strcmp(token, "ABS") == 0 || strcmp(token, "REV_ABS") == 0)
                {
                    int reverse = 0;
                    if (strcmp(token, "KEY") == 0)
                        type = KEY;
                    if (strcmp(token, "ABS") == 0)
                        type = ABS;

                    if (strcmp(token, "REV_ABS") == 0)
                    {
                        type = ABS;
                        reverse = 1;
                    }
                    token = strtok(NULL, " ");
                    trimToken(token, sizeof(buffer) - ((unsigned int)((token - buffer))));

                    int channel = atoi(token);

                    token = strtok(NULL, " ");
                    trimToken(token, sizeof(buffer) - ((unsigned int)((token - buffer))));

                    MODE mode = modeStringToEnum(token);

                    // todo: min/max should be defined either by user, the JVS-IO capabilites or the input device..
                    int min = 0;
                    int max = 0;
                    if (type == ABS)
                    {
                        min = 0;
                        max = 255;
                    }

                    MappingIn tempMapping = {
                        .channel = channel,
                        .type = type,
                        .mode = mode,
                        .min = min,
                        .max = max,
                        .reverse = reverse};

                    mappingIn[count] = tempMapping;
                    count++;
                }
                else
                {
                    printf("config.c: processInMapFile: incorrect settings keyword (%s).\n", token);
                }
            }
            fgets(buffer, 1024, fp);
        }
    }
    return count;
}

int processOutMapFile(char *filePath, MappingOut *mappingIn)
{
    int count = 0;
    FILE *fp;
    char buffer[1024];
    if ((fp = fopen(filePath, "r")) != NULL)
    {
        fgets(buffer, 1024, fp);
        while (!feof(fp))
        {
            if ((buffer[0] != '#') && (buffer[0] != 0) && (buffer[0] != '\r') && (buffer[0] != '\n') && (strcmp(buffer, "") != 0))
            {
                char *token = strtok(buffer, " ");
                IN_TYPE type = KEY;
                /* KEY <CHANNEL> <MODE> */
                if (strcmp(token, "ROTARY") == 0 || strcmp(token, "ANALOGUE") == 0 || strcmp(token, "BUTTON") == 0 || strcmp(token, "SYSTEM") == 0)
                {
                    if (strcmp(token, "ANALOGUE") == 0)
                        type = ANALOGUE;
                    if (strcmp(token, "BUTTON") == 0)
                        type = BUTTON;
                    if (strcmp(token, "SYSTEM") == 0)
                        type = SYSTEM;
                    if (strcmp(token, "ROTARY") == 0)
                        type = SYSTEM;

                    token = strtok(NULL, " ");
                    trimToken(token, sizeof(buffer) - ((unsigned int)((token - buffer))));
                    int channel = atoi(token);

                    token = strtok(NULL, " ");
                    trimToken(token, sizeof(buffer) - ((unsigned int)((token - buffer))));

                    MappingOut tempMapping = {
                        .channel = channel,
                        .type = type,
                        .mode = modeStringToEnum(token)};

                    mappingIn[count] = tempMapping;
                    count++;
                }
                else
                {
                    printf("config.c: processOutMapFile: incorrect settings keyword:%x \n", buffer[0]);
                }
            }
            fgets(buffer, 1024, fp);
        }
    }
    return count;
}
