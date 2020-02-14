#include "openjvs.h"

int running = 1;

int main(int argc, char **argv)
{
    printf("OpenJVS3 Development Version\n");

    /* Setup the IO we are trying to emulate */
    JVSCapabilities capabilities;
    capabilities.name = "OpenJVS3 Emulator;I/O BD JVS;837-13551;Ver1.00;98/10";
    capabilities.players = 2;
    capabilities.switches = 8;
    capabilities.analogueInBits = 8;
    capabilities.analogueInChannels = 8;
    capabilities.rotaryChannels = 8;
    capabilities.coins = 1;

    /* Setup the JVS Emulator with the RS485 path and capabilities */
    if (!initJVS("/dev/ttyS10", &capabilities))
    {
        printf("Error: Couldn't connect to serial\n");
        return 1;
    }


    /* Process packets forever */
    while (running)
    {
        if (!processPacket())
        {
            printf("Error: Failed to process packet properly.");
            return 1;
        }
    }

    /* Close the file pointer */
    if (!disconnectJVS())
    {
        printf("Error: Couldn't disconnect from serial\n");
        return 1;
    }

    return 0;
}
