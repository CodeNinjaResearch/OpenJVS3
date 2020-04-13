#include <string.h>
#include <math.h>

#include "definitions.h"
#include "io.h"

JVSState state;

JVSCapabilities *getCapabilities();
JVSState *getState();

JVSStatus initIO(void)
{
  JVSCapabilities *capabilities = getCapabilities();

  if (capabilities == NULL)
    return OPEN_JVS_ERR_NULL;

  // Copy the correct IO capabilities over
  memset(capabilities, 0, sizeof(JVSCapabilities));
  uint8_t ioChoice = 1;
  switch (ioChoice)
  {
  case 0:
    memcpy(capabilities, &jvs_io_lindbergh, sizeof(JVSCapabilities));
    break;

  case 1:
    memcpy(capabilities, &jvs_io_naomi, sizeof(JVSCapabilities));
    break;
  }

  // Set the maximum analogue values
  if (capabilities->analogueInBits > 16)
    return OPEN_JVS_ERR_ANALOG_BITS;
  capabilities->analogueMax = pow(2, capabilities->analogueInBits) - 1;

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

  return OPEN_JVS_ERR_OK;
}

int setSwitch(int player, int switchNumber, int value)
{
  JVSCapabilities *capabilities;
  capabilities = getCapabilities();

  if (player > capabilities->players)
  {
    printf("Error: That player does not exist.\n");
    return 0;
  }

  if (switchNumber >= capabilities->switches)
  {
    printf("Error: That switch does not exist.\n");
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
  JVSCapabilities *capabilities = getCapabilities();

  if (channel < capabilities->analogueInChannels)
  {
    state.analogueChannel[channel] = value * capabilities->analogueMax;
    return 1;
  }
  return 0;
}

int setRotary(int channel, int value)
{
  JVSCapabilities *capabilities = getCapabilities();

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
