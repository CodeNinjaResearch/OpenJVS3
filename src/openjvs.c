#include "openjvs.h"

int running = 1;

int main(int argc, char **argv)
{
    printf("OpenJVS3 Development Version\n");

    if (!connectJVS("/dev/ttyUSB0"))
    {
        printf("Error: Couldn't connect to serial\n");
        return 1;
    }

    while(running) {
        if(!processPacket()) {
            printf("Error: Failed to process packet properly.");
            return 1;
        }
    }

    if(!disconnectJVS()) {
        printf("Error: Couldn't disconnect from serial\n");
        return 1;
    }

    return 0;
}