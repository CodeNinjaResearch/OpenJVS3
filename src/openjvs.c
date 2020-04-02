#include "openjvs.h"

int running = 1;

int main(int argc, char **argv)
{
    signal(SIGINT, handle_sigint);

    printf("OpenJVS3\n");

    for (int i = 0; i < argc; i++)
    {
        if (!strcmp("--add-device", argv[i]))
        {
            printf("Welcome to adding a device\n");
            exit(-1);
        }

        if (!strcmp("--version", argv[i]))
        {
            printf("Release: 3.1.1, Input Engine: 2.0, JVS Engine: 3.0.1\n");
            printf("https://github.com/bobbydilley/OpenJVS\n");
            exit(-1);
        }
    }

    /* Setup the IO we are trying to emulate */
    JVSCapabilities capabilities;
    capabilities.name = "OpenJVS3 Emulator;I/O BD JVS;837-13551;Ver1.00;98/10";
    capabilities.players = 2;
    capabilities.switches = 16;
    capabilities.analogueInBits = 8;
    capabilities.analogueInChannels = 8;
    capabilities.rotaryChannels = 8;
    capabilities.generalPurposeOutputs = 0;
    capabilities.coins = 2;

    /* Setup the inputs on the computer */
    if (!initInput())
    {
        printf("Error: Inputs could not be setup properly\n");
        return 1;
    }

    /* Setup the JVS Emulator with the RS485 path and capabilities */
    if (!initJVS("/dev/ttyUSB1", &capabilities))
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

void handle_sigint(int sig)
{
    if (sig == 2)
    {
        printf("Debug: closing\n");
        stopThreads();
        exit(0);
    }
}
