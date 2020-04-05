#include <string.h>
#include "definitions.h"
#include "io.h"

JVSState state;

open_jvs_status_t initIO(void)
{
  open_jvs_status_t retval = OPEN_JVS_ERR_OK;

  JVSCapabilities *capabilities;

  capabilities = getCapabilities();

  if (NULL == capabilities)
  {
    retval = OPEN_JVS_ERR_NULL;
  }

  if (OPEN_JVS_ERR_OK == retval)
  {
    memset(capabilities, 0, sizeof(JVSCapabilities));

    // todo: use config to select template for JVS-IO ?
    uint8_t idx = 1;
    switch (idx)
    {
    default:
    case 0:
    {
      memcpy(capabilities, &jvs_io_lindbergh, sizeof(JVSCapabilities));
    }
    break;

    case 1:
    {
      memcpy(capabilities, &jvs_io_naomi, sizeof(JVSCapabilities));
    }
    }
  }

  /* Set analogue according to numver of bits */
  if (OPEN_JVS_ERR_OK == retval)
  {
    if (capabilities->analogueInBits > 16)
    {
      retval = OPEN_JVS_ERR_ANALOG_BITS;
    }

    /* Set max value that is supported for the set number of bits set for the analog channel */

    uint16_t max = 0;
    printf("jvs_analog_number_bits:%d\n", capabilities->analogueInBits);

    for (int16_t i = 0; i < capabilities->analogueInBits; i++)
    {
      max |= (1 << i);
    }

    if (OPEN_JVS_ERR_OK == retval)
    {
      retval = jvs_set_analog_max(max);
    }

    printf("jvs_analog_max:%04X \n", capabilities->analogueMax);
  }

  div_t switchDiv = div(capabilities->switches, 8);
  int switchBytes = switchDiv.quot + (switchDiv.rem ? 1 : 0);
  for (int players = 0; players < (capabilities->players + 1); players++)
  {
    for (int switches = 0; switches < switchBytes; switches++)
    {
      state.inputSwitch[players][switches] = 0x00;
    }
  }

  for (int analogueChannels = 0; analogueChannels < capabilities->analogueInChannels; analogueChannels++)
  {
    state.analogueChannel[analogueChannels] = 0;
  }

  for (int rotaryChannels = 0; rotaryChannels < capabilities->rotaryChannels; rotaryChannels++)
  {
    state.rotaryChannel[rotaryChannels] = 0;
  }

  state.coinCount = 2;

  return retval;
}

open_jvs_status_t jvs_get_analog_max(uint16_t *analog_max)
{
  open_jvs_status_t retval = OPEN_JVS_ERR_OK;
  JVSCapabilities *capabilities;

  capabilities = getCapabilities();

  if (NULL == capabilities)
  {
    retval = OPEN_JVS_ERR_NULL;
  }

  if (NULL == analog_max)
  {
    retval = OPEN_JVS_ERR_NULL;
  }

  if (OPEN_JVS_ERR_OK == retval)
  {
    *analog_max = capabilities->analogueMax;
  }

  return retval;
}

open_jvs_status_t jvs_set_analog_max(uint16_t max)
{
  open_jvs_status_t retval = OPEN_JVS_ERR_OK;

  JVSCapabilities *capabilities;

  capabilities = getCapabilities();

  if (NULL == capabilities)
  {
    retval = OPEN_JVS_ERR_NULL;
  }

  if (OPEN_JVS_ERR_OK == retval)
  {
    capabilities->analogueMax = max;
  }

  return retval;
}

int setSwitch(int player, int switchNumber, int value)
{
  JVSCapabilities *capabilities;
  capabilities = getCapabilities();

  if (player > capabilities->players)
  {
    printf("Error - That player does not exist.\n");
    return 0;
  }

  if (switchNumber >= capabilities->switches)
  {
    printf("Error - That switch does not exist.\n");
    return 0;
  }

  div_t switchDiv = div(switchNumber, 8);
  int switchBytes = switchDiv.quot + (switchDiv.rem ? 1 : 0);

  if (value)
  {
    state.inputSwitch[player][switchDiv.quot] |= 1 << (7 - switchDiv.rem);
  }
  else
  {
    state.inputSwitch[player][switchDiv.quot] &= ~(1 << (7 - switchDiv.rem));
  }

  return 1;
}

int incrementCoin()
{
  state.coinCount++;
  return 1;
}
int setAnalogue(int channel, int value)
{
  JVSCapabilities *capabilities;
  capabilities = getCapabilities();

  if (channel < capabilities->analogueInChannels)
  {
    state.analogueChannel[channel] = value;
    return 1;
  }
  return 0;
}

int setRotary(int channel, int value)
{
  JVSCapabilities *capabilities;
  capabilities = getCapabilities();

  if (channel < capabilities->rotaryChannels)
  {
    state.rotaryChannel[channel] = value;
    return 1;
  }

  return 0;
}

JVSState *getState()
{
  return &state;
}

void controlPrintStatus()
{
  /* Print Buttons */
  for (int player = 0; player < getCapabilities()->players + 1; player++)
  {
    if (player == 0)
    {
      printf("System:\n\t");
    }
    else
    {
      printf("Player %d:\n\t", player);
    }
    for (int channel = 0; channel < (getCapabilities()->switches + 7) / 8; channel++)
    {
      for (int bit = 7; 0 <= bit; bit--)
      {
        printf("%d ", (getState()->inputSwitch[player][channel] >> bit) & 0x01);
      }
    }
    printf("\n");
  }

  /* Print Analogue Count */
  printf("Analogue:\n\t");
  for (int channel = 0; channel < getCapabilities()->analogueInChannels; channel++)
  {
    printf("%d ", getState()->analogueChannel[channel]);
  }
  printf("\n");

  /* Print Rotary Count */
  printf("Rotary:\n\t");
  for (int channel = 0; channel < getCapabilities()->rotaryChannels; channel++)
  {
    printf("%d ", getState()->rotaryChannel[channel]);
  }
  printf("\n");

  /* Print Coin Count */
  printf("Coin:\n\t%d\n", getState()->coinCount);
}
