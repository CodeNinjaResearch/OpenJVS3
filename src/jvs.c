#include "jvs.h"

int serialIO = -1;
int deviceID = -1;
int debugEnabled = 1;
JVSCapabilities* capabilities;

int initJVS(char *devicePath, JVSCapabilities* capabilitiesSetup)
{
	capabilities = capabilitiesSetup;

	if ((serialIO = open(devicePath, O_RDWR | O_NOCTTY | O_SYNC)) < 0)
	{
		printf("Failed to open %s\n", devicePath);
		return 0;
	}
	
	setSerialAttributes(serialIO, B115200);

	setSyncPin(0); // Float Sync

	return 1;
}

int disconnectJVS()
{
	return close(serialIO);
}

int writeCapabilities(JVSPacket *outputPacket)
{
	outputPacket->data[outputPacket->length] = STATUS_SUCCESS;
	outputPacket->length += 1;

	if (capabilities->players > 0)
	{
		outputPacket->data[outputPacket->length + 1] = CAP_PLAYERS;
		outputPacket->data[outputPacket->length + 2] = capabilities->players;
		outputPacket->data[outputPacket->length + 3] = capabilities->switches;
		outputPacket->length += 3;
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
	JVSPacket inPacket;

	if (!readPacket(&inPacket))
	{
		printf("Error: Could not read packet\n");
		return 0;
	}

	JVSPacket outputPacket;

	int index = 2;

	while (index < inPacket.length)
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
			char *id = "OpenJVS Emulator;I/O BD JVS;837-13551;Ver1.00;98/10";
			memcpy(&outputPacket.data[outputPacket.length + 1], id, strlen(id) + 1);
			outputPacket.length += strlen(id) + 2;
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
			writeCapabilities(&outputPacket);
			break;
		default:
			printf("Warning: This command is not properly supported\n");
		}
		index += size;
	}

	usleep(10 * 1000);

	outputPacket.destination = BUS_MASTER;

	return writePacket(&outputPacket);
}

int readByte(unsigned char *byte)
{
	return read(serialIO, byte, 1);
}

int writeByte(unsigned char byte)
{
	char buffer[] = {0x00};
	buffer[0] = byte;
	return write(serialIO, buffer, sizeof(buffer));
}

int readPacket(JVSPacket *packet)
{
	unsigned char byte = 0;
	int n = readByte(&byte);

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
	writeByte(SYNC);
	writeByte(packet->destination);
	writeByte(packet->length + 1);
	unsigned char checksum = packet->destination + packet->length + 1;
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

/* Sets the configuration of the serial port */
int setSerialAttributes(int fd, int myBaud)
{
	struct termios options;
	int status;
	tcgetattr(fd, &options);

	cfmakeraw(&options);
	cfsetispeed(&options, myBaud);
	cfsetospeed(&options, myBaud);

	options.c_cflag |= (CLOCAL | CREAD);
	options.c_cflag &= ~PARENB;
	options.c_cflag &= ~CSTOPB;
	options.c_cflag &= ~CSIZE;
	options.c_cflag |= CS8;
	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	options.c_oflag &= ~OPOST;

	options.c_cc[VMIN] = 0;
	options.c_cc[VTIME] = 100; // Ten seconds (100 deciseconds)

	tcsetattr(fd, TCSANOW, &options);

	ioctl(fd, TIOCMGET, &status);

	status |= TIOCM_DTR;
	status |= TIOCM_RTS;

	ioctl(fd, TIOCMSET, &status);

	usleep(100 * 1000); // 10mS

	return 0;
}

int setSyncPin(int a)
{
	if (a == 0)
	{
		debug("Floated sync pin");
	}
	else
	{
		debug("Grounded sync pin");
	}
	return 0;
}
