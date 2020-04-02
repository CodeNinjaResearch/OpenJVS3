#include <circ_buffer.h>
#include "openjvs.h"
#include "stdio.h"
#include <unistd.h>

int running = 1;

circ_buffer_t read_buffer;
/* Setup the IO we are trying to emulate */
JVSCapabilities capabilities;

JVSCapabilities *getCapabilities()
{
  return &capabilities;
}

int main(int argc, char **argv)
{
  open_jvs_status_t retval = OPEN_JVS_ERR_OK;

  signal(SIGINT, handle_sigint);

  printf("OpenJVS\n");

  scanInputs();
  connectDevices();

  /* Setup the JVS Emulator with the RS485 path and capabilities */
  retval = initJVS("/dev/ttyUSB0", &capabilities);

  if (OPEN_JVS_ERR_OK != retval)
  {
    printf("initJVS() returned:%d \n",retval);
    return 1;
  }


  /* Process packets forever */
  while (running)
  {
    retval = jvs_do();

#ifdef OFFLINE_MODE
    // Give time for debug prints of task started later
    sleep(5);
    retval = OPEN_JVS_ERR_OFFLINE;
#endif

    switch(retval)
    {
      /* Status */
      case OPEN_JVS_ERR_TIMEOUT:
      case OPEN_JVS_ERR_SYNC_BYTE:
      case OPEN_JVS_NO_RESPONSE:
      case OPEN_JVS_ERR_WAIT_BYTES:
      {
        uint8_t dbg[300];
        snprintf(dbg, sizeof(dbg),"jvs main loop returned:%d \n", retval);
        debug(dbg);
      }
      break;

      /* Errors */
      default:
      {
        uint8_t dbg[300];
        snprintf(dbg, sizeof(dbg),"jvs main loop returned:%d \n", retval);

        running = false;
      }
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
