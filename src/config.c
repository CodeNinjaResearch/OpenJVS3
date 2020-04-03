#include <stdio.h>
#include <string.h>

#include "config.h"

int processConfig(char *filePath)
{
    printf("Processing config from: %s\n", filePath);
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
            if (buffer[0] != '#' && buffer[0] != 0 && strcmp(buffer, "") != 0)
            {
                char *token = strtok(buffer, " ");
                InType type = KEY;
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
                    if (token[strlen(token) - 1] == '\n')
                        token[strlen(token) - 1] = '\0';
                    int channel = atoi(token);

                    token = strtok(NULL, " ");
                    if (token[strlen(token) - 1] == '\n')
                        token[strlen(token) - 1] = '\0';
                    Mode mode = modeStringToEnum(token);

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
            if (buffer[0] != '#' && buffer[0] != 0 && strcmp(buffer, "") != 0)
            {
                char *token = strtok(buffer, " ");
                InType type = KEY;
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
                    if (token[strlen(token) - 1] == '\n')
                        token[strlen(token) - 1] = '\0';
                    int channel = atoi(token);

                    token = strtok(NULL, " ");
                    if (token[strlen(token) - 1] == '\n')
                        token[strlen(token) - 1] = '\0';

                    MappingOut tempMapping = {
                        .channel = channel,
                        .type = type,
                        .mode = modeStringToEnum(token)};

                    mappingIn[count] = tempMapping;
                    count++;
                }
                else
                {
                    printf("config.c: processOutMapFile: incorrect settings keyword.\n");
                }
            }
            fgets(buffer, 1024, fp);
        }
    }
    return count;
}
