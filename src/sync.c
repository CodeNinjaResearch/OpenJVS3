#include "sync.h"
#include "jvs.h"
#include "definitions.h"

int SyncPinInit(void)
{
	int retval = 0;

#ifdef USE_SYNC_PIN
	/* GPIO SYNC PINS */
	if ((retval = GPIOExport(sync_pin)) == -1)
	{
		printf("Warning: Sync pin %d not available\n", sync_pin);
	}
#endif

	SyncPinLow(false);

	return retval;
}

void SyncPinLow(bool pull_low)
{
	bool error = false;

	if (pull_low)
	{
#ifdef USE_SYNC_PIN
#if (SNYC_PIN_IMPLEMENTAION == SYNC_PIN_HW_SWITCH)
		if (GPIODirection(sync_pin, OUT) == -1 || GPIOWrite(sync_pin, 1) == -1)
		{
			printf("Warning: Failed to ground  pin %d\n", sync_pin);
		}
#elif (SNYC_PIN_IMPLEMENTAION == SYNC_PIN_HW_FLOAT)
		if (GPIODirection(sync_pin, OUT) == -1 || GPIOWrite(sync_pin, 0) == -1)
		{
			printf("Warning: Failed to ground  pin %d\n", sync_pin);
		}
#endif
#endif
		debug("Floated sync pin");
	}
	else
	{

#ifdef USE_SYNC_PIN
#if (SNYC_PIN_IMPLEMENTAION == SYNC_PIN_HW_SWITCH)
		if (GPIODirection(sync_pin, OUT) == -1 || GPIOWrite(sync_pin, 0) == -1)
		{
			printf("Warning: Failed to pull high pin %d\n", sync_pin);
		}
#elif (SNYC_PIN_IMPLEMENTAION == SYNC_PIN_HW_FLOAT)
		if (GPIODirection(sync_pin, IN) == -1)
		{
			printf("Warning: Failed to float pin %d\n", sync_pin);
		}
#endif
#endif
		debug("Grounded sync pin");
	}
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
		//fprintf(stderr, "Failed to open gpio value for writing!\n");
		return (-1);
	}

	if (1 != write(fd, &s_values_str[LOW == value ? 0 : 1], 1))
	{
		//fprintf(stderr, "Failed to write value!\n");
		return (-1);
	}

	close(fd);
	return (0);
}
