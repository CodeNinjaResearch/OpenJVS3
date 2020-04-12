#include "sync.h"
#include "jvs.h"
#include "definitions.h"

static SYNC_CIRCUIT sync_circuit_use = SYNC_CIRCUIT_FLOAT;

open_jvs_status_t SyncAlgorithmSet(SYNC_CIRCUIT circ_type)
{
	open_jvs_status_t retval = OPEN_JVS_ERR_OK;

	switch (circ_type)
	{
	case SYNC_CIRCUIT_FLOAT:
	case SYNC_CIRCUIT_SWITCH:
	{
		sync_circuit_use = circ_type;
	}
	break;

	default:
	{
		printf("**** Invalid Sync algorithm net: %u ****\n", circ_type);
		retval = OPEN_JVS_ERR_INVALID_SYNC_CIRCUIT;
	}
	break;
	}
	return retval;
}

int SyncPinInit(void)
{
	int retval = 0;

	/* Make Pin available */
	switch (sync_circuit_use)
	{
	case SYNC_CIRCUIT_SWITCH:
	case SYNC_CIRCUIT_FLOAT:
		/* GPIO SYNC PINS */
		retval = GPIOExport(sync_pin);
		if (retval != 0)
		{
			printf("Warning: Sync pin %d not available\n", sync_pin);
		}
		break;

	SYNC_NONE:
		break;
	}

	/* Config Pin*/
	if (retval == 0)
	{
		switch (sync_circuit_use)
		{
		case SYNC_CIRCUIT_SWITCH:
		{
			retval = GPIODirection(sync_pin, OUT);
			if (retval != 0)
			{
				printf("Warning: Sync pin %d could not be set to output\n", sync_pin);
			}
		}
		break;

		case SYNC_NONE:
		case SYNC_CIRCUIT_FLOAT:
			break;
		}
	}

	SyncPinLow(false);
	return retval;
}

int SyncPinLow(bool pull_low)
{
	int error = 0;

	if (pull_low)
	{
		switch (sync_circuit_use)
		{
		case SYNC_CIRCUIT_SWITCH:
		{
			error = GPIOWrite(sync_pin, 1);

			if (error != 0)
			{
				printf("Warning: Failed to ground  pin %d\n", sync_pin);
			}
		}
		break;

		case SYNC_CIRCUIT_FLOAT:
		{
			error = GPIODirection(sync_pin, OUT);

			if (error == 0)
			{
				error = GPIOWrite(sync_pin, 0);
			}

			if (error != 0)
			{
				printf("Warning: Failed to ground  pin %d\n", sync_pin);
			}
		}
		break;

		case SYNC_NONE:
			break;

		default:
		{
			printf("Invalid Sync algorithm net: %u \n", sync_circuit_use);
		}
		break;
		}

		debug("Floated sync pin");
	}
	else
	{
		switch (sync_circuit_use)
		{
		case SYNC_CIRCUIT_SWITCH:
		{
			error = GPIOWrite(sync_pin, 0);

			if (error != 0)
			{
				printf("Warning: Failed to pull high pin %d\n", sync_pin);
			}
		}
		break;

		case SYNC_CIRCUIT_FLOAT:
		{
			error = GPIODirection(sync_pin, IN);

			if (error != 0)
			{
				printf("Warning: Failed to float pin %d\n", sync_pin);
			}
		}
		break;

		case SYNC_NONE:
			break;

		default:
		{
			printf("Invalid Sync algorithm net: %u \n", sync_circuit_use);
		}
		break;
		}

		debug("Grounded sync pin");
	}
	return error;
}

int GPIOExport(int pin)
{
#define BUFFER_MAX 3
	char buffer[BUFFER_MAX];
	ssize_t bytes_written;
	int fd;

	fd = open("/sys/class/gpio/export", O_WRONLY);
	if (-1 == fd)
	{
		printf("Failed to open export for writing!\n");
		return (-1);
	}

	bytes_written = snprintf(buffer, BUFFER_MAX, "%d", pin);
	write(fd, buffer, bytes_written);
	close(fd);
	return (0);
}

int GPIOUnexport(int pin)
{
	char buffer[BUFFER_MAX];
	ssize_t bytes_written;
	int fd;

	fd = open("/sys/class/gpio/unexport", O_WRONLY);
	if (-1 == fd)
	{
		//fprintf(stderr, "Failed to open unexport for writing!\n");
		return (-1);
	}

	bytes_written = snprintf(buffer, BUFFER_MAX, "%d", pin);
	write(fd, buffer, bytes_written);
	close(fd);
	return (0);
}

int GPIODirection(int pin, int dir)
{
	static const char s_directions_str[] = "in\0out";

#define DIRECTION_MAX 35
	char path[DIRECTION_MAX];
	int fd;

	snprintf(path, DIRECTION_MAX, "/sys/class/gpio/gpio%d/direction", pin);
	fd = open(path, O_WRONLY);
	if (-1 == fd)
	{
		//fprintf(stderr, "Failed to open gpio direction for writing!\n");
		return (-1);
	}

	if (-1 == write(fd, &s_directions_str[IN == dir ? 0 : 3], IN == dir ? 2 : 3))
	{
		//fprintf(stderr, "Failed to set direction!\n");
		return (-1);
	}

	close(fd);
	return (0);
}

int GPIORead(int pin)
{
#define VALUE_MAX 30
	char path[VALUE_MAX];
	char value_str[3];
	int fd;

	snprintf(path, VALUE_MAX, "/sys/class/gpio/gpio%d/value", pin);
	fd = open(path, O_RDONLY);
	if (-1 == fd)
	{
		//fprintf(stderr, "Failed to open gpio value for reading!\n");
		return (-1);
	}

	if (-1 == read(fd, value_str, 3))
	{
		//fprintf(stderr, "Failed to read value!\n");
		return (-1);
	}

	close(fd);

	return (atoi(value_str));
}

int GPIOWrite(int pin, int value)
{
	static const char s_values_str[] = "01";

	char path[VALUE_MAX];
	int fd;

	snprintf(path, VALUE_MAX, "/sys/class/gpio/gpio%d/value", pin);
	fd = open(path, O_WRONLY);
	if (-1 == fd)
	{
		fprintf(stderr, "Failed to open gpio value for writing!\n");
		return (-1);
	}

	if (1 != write(fd, &s_values_str[LOW == value ? 0 : 1], 1))
	{
		fprintf(stderr, "Failed to write value!\n");
		return (-1);
	}

	close(fd);
	return (0);
}
