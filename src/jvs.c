#include "jvs.h"

int serialIO = -1;
char *devicePath = "/dev/ttyUSB0";
int deviceID = -1;

int connectJVS()
{
	if ((serialIO = open(devicePath, O_RDWR | O_NOCTTY | O_SYNC)) < 0)
	{
		printf("Failed to open %s\n", devicePath);
		return 0;
	}

	setSerialAttributes(serialIO, B115200);

	setSyncPin(0); // Float Sync

	return 1;
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
			size = 2;
			deviceID = -1;
			setSyncPin(0);
			break;
		case CMD_ASSIGN_ADDR:
			size = 2;
			deviceID = inPacket.data[index + 1];
			outputPacket.data[outputPacket.length] = STATUS_SUCCESS;
			outputPacket.length += 1;
			setSyncPin(1);
			break;
		default:
			printf("Warning: This command is not properly supported\n");
		}
		index += size;
	}

	usleep(10 * 1000);

	outputPacket.destination = BUS_MASTER;
	writePacket(&outputPacket);

	return 1;
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
		printf("Checksum Error - The checksum is not correct\n");
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
		// Float
	}
	else
	{
		// Ground
	}
	return 0;
}
