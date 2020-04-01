#include "jvs.h"

int deviceID = -1;
int debugEnabled = 0;

int initJVS(char *devicePath, JVSCapabilities *capabilitiesSetup)
{
	/* Init the connection to the Naomi */
	if (!initDevice(devicePath))
	{
		return 0;
	}

	/* Init the Virtual IO */
	if (!initIO(capabilitiesSetup))
	{
		return 0;
	}

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

	if (capabilities->coins > 0)
	{
		outputPacket->data[outputPacket->length] = CAP_COINS;
		outputPacket->data[outputPacket->length + 1] = capabilities->coins;
		outputPacket->data[outputPacket->length + 2] = 0x00;
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

	if (capabilities->generalPurposeOutputs > 0)
	{
		outputPacket->data[outputPacket->length] = CAP_GPO;
		outputPacket->data[outputPacket->length + 1] = capabilities->generalPurposeOutputs;
		outputPacket->data[outputPacket->length + 2] = 0x00;
		outputPacket->data[outputPacket->length + 3] = CAP_END;
		outputPacket->length += 4;
	}

	if (capabilities->analogueOutChannels > 0)
	{
		outputPacket->data[outputPacket->length] = CAP_ANALOG_OUT;
		outputPacket->data[outputPacket->length + 1] = capabilities->analogueOutChannels;
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

	int attempts = 3;
	if (attempts > 0 && !readPacket(&inPacket))
	{
		printf("Error: Could not read packet\n");
		attempts -= 1;
		if (attempts < 1)
		{
			return 0;
		}
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
			outputPacket.data[outputPacket.length] = state->inputSwitch[0][0];
			outputPacket.length += 1;
			printf("Players %d\n", inPacket.data[index + 1]);
			printf("Bytes Per Player %d\n", inPacket.data[index + 2]);
			for (int i = 0; i < inPacket.data[index + 1]; i++)
			{
				for (int j = 0; j < inPacket.data[index + 2]; j++)
				{
					outputPacket.data[outputPacket.length] = (state->inputSwitch[i + 1][j]);
					printf("Output: %d\n", outputPacket.data[outputPacket.length]);
					outputPacket.length += 1;
				}
			}
			break;
		case CMD_READ_COINS:
			debug("CMD_READ_COINS");
			size = 2;
			outputPacket.data[outputPacket.length] = STATUS_SUCCESS;
			outputPacket.data[outputPacket.length + 1] = 0x00;
			outputPacket.data[outputPacket.length + 2] = state->coinCount;
			outputPacket.data[outputPacket.length + 3] = 0x00;
			outputPacket.data[outputPacket.length + 4] = 0x00;
			outputPacket.length += 5;
			break;
		case CMD_READ_ANALOGS:
			debug("CMD_READ_ANALOGS");
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
			debug("CMD_READ_ROTARY");
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
		case CMD_WRITE_GPO:
			debug("CMD_WRITE_GPO");
			size = 2 + inPacket.data[index + 1];
			for (int i = 0; i < inPacket.data[index + 1]; i++)
			{
				unsigned char thing = inPacket.data[index + 1 + i];
				for (int j = 7; j >= 0; j--)
				{
					unsigned char bit = (thing >> j) & 1;
					printf("%u", bit);
				}
				printf(" ");
			}
			printf("\n");
			outputPacket.data[outputPacket.length] = STATUS_SUCCESS;
			outputPacket.length += 1;
			break;
		case CMD_DECREASE_COINS:
			debug("CMD_DECREASE_COINS");
			size = 4;
			outputPacket.data[outputPacket.length] = STATUS_SUCCESS;
			outputPacket.length += 1;
			int amount = inPacket.data[index + 2] << 8 || inPacket.data[index + 8];
			state->coinCount -= amount;
			break;
		default:
			printf("Warning: This command is not properly supported [0x%02hhX]\n", inPacket.data[index]);
		}
		index += size;
	}
	debug("");

	outputPacket.destination = BUS_MASTER;

	return writePacket(&outputPacket);
}

int readPacket(JVSPacket *packet)
{

	unsigned char byte = 0;
	int n = readBytes(&byte, 1);
	while (byte != SYNC || n < 1)
	{
		n = readBytes(&byte, 1);
	}

	readBytes(&packet->destination, 1);
	readBytes(&packet->length, 1);
	unsigned char checksumComputed = packet->destination + packet->length;

	unsigned char inputBuffer[MAX_PACKET_SIZE];
	int read = 0;
	while (read < packet->length)
	{
		read += readBytes(inputBuffer + read, packet->length - read);
	}

	int inputIndex = 0;
	for (int i = 0; i < packet->length - 1; i++)
	{
		packet->data[inputIndex] = inputBuffer[i];
		if (packet->data[inputIndex] == ESCAPE)
		{
			i++;
			packet->data[inputIndex] = inputBuffer[i] + 1;
		}
		checksumComputed = (checksumComputed + packet->data[inputIndex]) & 0xFF;
		inputIndex++;
	}
	unsigned char checksumReceived = inputBuffer[packet->length - 1];

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

	int outputIndex = 0;
	unsigned char outputBuffer[MAX_PACKET_SIZE];

	outputBuffer[outputIndex] = SYNC;
	outputBuffer[outputIndex + 1] = packet->destination;
	outputBuffer[outputIndex + 2] = packet->length + 2;
	outputBuffer[outputIndex + 3] = STATUS_SUCCESS;
	outputIndex += 4;

	unsigned char checksum = packet->destination + packet->length + 2 + STATUS_SUCCESS;
	for (int i = 0; i < packet->length; i++)
	{
		if (packet->data[i] == SYNC || packet->data[i] == ESCAPE)
		{
			outputBuffer[outputIndex] = ESCAPE;
			outputBuffer[outputIndex + 1] = (packet->data[i] - 1);
			outputIndex += 2;
		}
		else
		{
			outputBuffer[outputIndex] = (packet->data[i]);
			outputIndex++;
		}
		checksum = (checksum + packet->data[i]) & 0xFF;
	}
	outputBuffer[outputIndex] = checksum;
	outputIndex += 1;

	if (writeBytes(outputBuffer, outputIndex) < outputIndex)
	{
		printf("Failure: Could not write enough bytes\n");
		return 0;
	}
	return 1;
}
