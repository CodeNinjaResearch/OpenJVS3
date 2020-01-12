#include "openjvs.h"

int main(int argc, char **argv)
{
    printf("OpenJVS3 Development Version\n");

    if (!connectJVS())
    {
        printf("Error: Couldn't connect to serial\n");
        return 1;
    }

    while(1) {
        if(!processPacket()) {
            printf("Error: Failed to process packet properly.");
            return 1;
        }
    }

    return 0;
}