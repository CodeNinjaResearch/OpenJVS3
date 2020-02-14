#include "jvs.h"

int deviceID = -1;
int debugEnabled = 0;

int initJVS(char *devicePath, JVSCapabilities *capabilitiesSetup)
{
	/* Init the connection to the Naomi */
	initDevice(devicePath);

	/* Init the Virtual IO */
	initIO(capabilitiesSetup);

	return 1;
}

int disconnectJVS()
{
	return closeDevice();
}

int writeCapabilities(JVSPacket *outputPacket, JVSCapabilities *capabilities)
{
	outputPacket->data[outputPacket->length] = STATUS_SUCCESS;
	outputPacket->length += 1;

	if (capabilities->players > 0)
	{
		outputPacket->data[outputPacket->length] = CAP_PLAYERS;
		outputPacket->data[outputPacket->length + 1] = capabilities->players;
		outputPacket->data[outputPacket->length + 2] = capabilities->switches;
		outputPacket->data[outputPacket->length + 3] = CAP_END;
		outputPacket->length += 4;
	}

	if (capabilities->analogueInChannels > 0)
	{
		outputPacket->data[outputPacket->length] = CAP_ANALOG_IN;
		outputPacket->data[outputPacket->length + 1] = capabilities->analogueInChannels;
		outputPacket->data[outputPacket->length + 2] = capabilities->analogueInBits;
		outputPacket->data[outputPacket->length + 3] = CAP_END;
		outputPacket->length += 4;
	}

	if (capabilities->rotaryChannels > 0)
	{
		outputPacket->data[outputPacket->length] = CAP_ROTARY;
		outputPacket->data[outputPacket->length + 1] = capabilities->rotaryChannels;
		outputPacket->data[outputPacket->length + 2] = 0x00;
		outputPacket->data[outputPacket->length + 3] = CAP_END;
		outputPacket->length += 4;
	}

	if (capabilities->coins > 0)
	{
		outputPacket->data[outputPacket->length] = CAP_COINS;
		outputPacket->data[outputPacket->length + 1] = capabilities->coins;
		outputPacket->data[outputPacket->length + 2] = 0x00;
		outputPacket->data[outputPacket->length + 3] = CAP_END;
		outputPacket->length += 4;
	}

	outputPacket->data[outputPacket->length] = CAP_END;
	outputPacket->length += 1;

	return 1;
}

void debug(char *string)
{
	if (debugEnabled)
	{
		printf("Debug: %s\n", string);
	}
}

int processPacket()
{
	JVSState *state = getState();
	JVSCapabilities *capabilities = getCapabilities();

	JVSPacket inPacket;

	if (!readPacket(&inPacket))
	{
		printf("Error: Could not read packet\n");
		return 0;
	}

	JVSPacket outputPacket;
	outputPacket.length = 0;
	outputPacket.destination = BUS_MASTER;

	int index = 0;

	while (index < inPacket.length - 1)
	{
		int size = 1;
		switch (inPacket.data[index])
		{
		case CMD_RESET:
			debug("CMD_RESET");
			size = 2;
			deviceID = -1;
			setSyncPin(0);
			break;
		case CMD_ASSIGN_ADDR:
			debug("CMD_ASSIGN_ADDR");
			size = 2;
			deviceID = inPacket.data[index + 1];
			outputPacket.data[outputPacket.length] = STATUS_SUCCESS;
			outputPacket.length += 1;
			setSyncPin(1);
			break;
		case CMD_REQUEST_ID:
			debug("CMD_REQUEST_ID");
			outputPacket.data[outputPacket.length] = STATUS_SUCCESS;
			memcpy(&outputPacket.data[outputPacket.length + 1], capabilities->name, strlen(capabilities->name) + 1);
			outputPacket.length += strlen(capabilities->name) + 2;
			break;
		case CMD_COMMAND_VERSION:
			debug("CMD_COMMAND_VERSION");
			outputPacket.data[outputPacket.length] = STATUS_SUCCESS;
			outputPacket.data[outputPacket.length + 1] = 0x11;
			outputPacket.length += 2;
			break;
		case CMD_JVS_VERSION:
			debug("CMD_JVS_VERSION");
			outputPacket.data[outputPacket.length] = STATUS_SUCCESS;
			outputPacket.data[outputPacket.length + 1] = 0x20;
			outputPacket.length += 2;
			break;
		case CMD_COMMS_VERSION:
			debug("CMD_COMMS_VERSION");
			outputPacket.data[outputPacket.length] = STATUS_SUCCESS;
			outputPacket.data[outputPacket.length + 1] = 0x10;
			outputPacket.length += 2;
			break;
		case CMD_CAPABILITIES:
			debug("CMD_CAPABILITIES");
			writeCapabilities(&outputPacket, capabilities);
			break;
		case CMD_READ_SWITCHES:
			debug("CMD_READSWITCHES");
			size = 3;
			outputPacket.data[outputPacket.length] = STATUS_SUCCESS;
			outputPacket.length += 1;
			for (int i = 0; i <= inPacket.data[index + 1]; i++)
			{
				for (int j = 0; j < inPacket.data[index + 2]; j++)
				{
					outputPacket.data[outputPacket.length] = (state->inputSwitch[i][j]);
					outputPacket.length += 1;
				}
			}
			break;
		case CMD_READ_COINS:
			debug("CMD_READ_COINS\n");
			size = 2;
			outputPacket.data[outputPacket.length] = STATUS_SUCCESS;
			outputPacket.data[outputPacket.length + 1] = 0x00;
			outputPacket.data[outputPacket.length + 2] = state->coinCount;
			outputPacket.data[outputPacket.length + 3] = 0x00;
			outputPacket.data[outputPacket.length + 4] = 0x00;
			outputPacket.length += 5;
			break;
		case CMD_READ_ANALOGS:
			debug("CMD_READ_ANALOGS\n");
			size = 2;
			outputPacket.data[outputPacket.length] = STATUS_SUCCESS;
			outputPacket.length += 1;
			for (int i = 0; i < inPacket.data[index + 1]; i++)
			{
				outputPacket.data[outputPacket.length] = state->analogueChannel[i];
				outputPacket.data[outputPacket.length + 1] = 0x00;
				outputPacket.length += 2;
			}
			break;
		case CMD_READ_ROTARY:
			debug("CMD_READ_ROTARY\n");
			size = 2;
			outputPacket.data[outputPacket.length] = STATUS_SUCCESS;
			outputPacket.length += 1;
			for (int i = 0; i < inPacket.data[index + 1]; i++)
			{
				outputPacket.data[outputPacket.length] = state->rotaryChannel[i];
				outputPacket.data[outputPacket.length + 1] = 0x00;
				outputPacket.length += 2;
			}
			break;
		default:
			printf("Warning: This command is not properly supported [0x%02hhX]\n", inPacket.data[index]);
		}
		index += size;
	}

	outputPacket.destination = BUS_MASTER;

	return writePacket(&outputPacket);
}

int readPacket(JVSPacket *packet)
{
	unsigned char byte = 0;
	int n = readByte(&byte);
	while (byte != SYNC || n < 1)
	{
		n = readByte(&byte);
	}

	readByte(&packet->destination);
	readByte(&packet->length);

	unsigned char checksumComputed = packet->destination + packet->length;

	for (int i = 0; i < packet->length - 1; i++)
	{
		readByte(&packet->data[i]);
		if (packet->data[i] == ESCAPE)
		{
			readByte(&packet->data[i]);
			packet->data[i] += 1;
		}
		checksumComputed = (checksumComputed + packet->data[i]) & 0xFF;
	}
	unsigned char checksumReceived = 0;
	readByte(&checksumReceived);

	if (checksumReceived != checksumComputed)
	{
		printf("Error: The checksum is not correct\n");
		return 0;
	}

	return 1;
}

int writePacket(JVSPacket *packet)
{
	/* Don't return anything if there isn't anything to write! */
	if (packet->length < 1)
	{
		return 1;
	}

	writeByte(SYNC);
	writeByte(packet->destination);
	writeByte(packet->length + 2);

	writeByte(STATUS_SUCCESS);
	unsigned char checksum = packet->destination + packet->length + 2 + STATUS_SUCCESS;
	for (int i = 0; i < packet->length; i++)
	{
		if (packet->data[i] == SYNC || packet->data[i] == ESCAPE)
		{
			writeByte(ESCAPE);
			writeByte(packet->data[i] - 1);
		}
		else
		{
			writeByte(packet->data[i]);
		}
		checksum = (checksum + packet->data[i]) & 0xFF;
	}
	writeByte(checksum);
	return 1;
}
