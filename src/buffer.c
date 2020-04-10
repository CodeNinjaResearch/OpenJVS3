/*
 * buffer.c
 *
 *  Created on: 19.01.2020
 *      Author: RedOne87
 */

#include <stdio.h>
#include <string.h>

#include "buffer.h"

void initBuffer(Buffer *buffer)
{
	buffer->readPosition = 0;
	buffer->writePosition = 0;
	memset(buffer->data, 0, sizeof(buffer->data));
}

BufferStatus pushToBuffer(Buffer *buffer, uint8_t data)
{
	if (buffer == NULL)
		return BUFFER_ERROR_NULL;

	// Check if the buffer is full
	if (buffer->writePosition == ((buffer->readPosition - 1 + CIRC_BUFFER_SIZE) % CIRC_BUFFER_SIZE))
		return BUFFER_ERROR_FULL;

	// Write data to the buffer
	buffer->data[buffer->writePosition] = data;
	buffer->writePosition = (buffer->writePosition + 1) % CIRC_BUFFER_SIZE;

	return BUFFER_SUCCESS;
}

BufferStatus discardFromBuffer(Buffer *buffer, uint32_t number_bytes)
{
	BufferStatus retval = BUFFER_SUCCESS;
	uint32_t number_bytes_available = 0;

	if (NULL == buffer)
	{
		retval = BUFFER_ERROR_NULL;
	}

	if (BUFFER_SUCCESS == retval)
	{
		retval = bufferIsFilled(buffer, &number_bytes_available);
	}

	if (BUFFER_SUCCESS == retval)
	{
		if ((buffer->readPosition == buffer->writePosition) || (number_bytes > (number_bytes_available)))
		{
			retval = BUFFER_ERR_OFFSET;
		}
	}

	if (BUFFER_SUCCESS == retval)
	{
		buffer->readPosition = (buffer->readPosition + number_bytes) % CIRC_BUFFER_SIZE;
	}
	return retval;
}

BufferStatus popFromBuffer(Buffer *buffer, uint8_t *out_data)
{
	BufferStatus retval = BUFFER_SUCCESS;

	if (NULL == buffer)
	{
		retval = BUFFER_ERROR_NULL;
	}

	if (BUFFER_SUCCESS == retval)
	{
		if (buffer->readPosition == buffer->writePosition)
		{
			retval = BUFFER_ERR_EMPTY;
		}
	}

	if (BUFFER_SUCCESS == retval)
	{
		*out_data = buffer->data[buffer->readPosition];
		buffer->readPosition = (buffer->readPosition + 1) % CIRC_BUFFER_SIZE;
	}
	return retval;
}

BufferStatus peekFromBuffer(Buffer *buffer, uint32_t offset, uint8_t *out_data)
{
	BufferStatus retval = BUFFER_SUCCESS;
	uint32_t number_bytes_available = 0;

	if (NULL == buffer)
	{
		retval = BUFFER_ERROR_NULL;
	}

	if (BUFFER_SUCCESS == retval)
	{
		retval = bufferIsFilled(buffer, &number_bytes_available);
	}

	if (BUFFER_SUCCESS == retval)
	{
		if ((buffer->readPosition == buffer->writePosition) || (offset >= (number_bytes_available)))
		{
			retval = BUFFER_ERR_OFFSET;
		}
	}

	if (BUFFER_SUCCESS == retval)
	{
		*out_data = buffer->data[(buffer->readPosition + offset) % CIRC_BUFFER_SIZE];
	}
	return retval;
}

BufferStatus bufferIsFilled(Buffer *buffer, uint32_t *out_number)
{
	BufferStatus retval = BUFFER_SUCCESS;

	if ((NULL == buffer) || (NULL == out_number))
	{
		retval = BUFFER_ERROR_NULL;
	}

	if (BUFFER_SUCCESS == retval)
	{
		*out_number = ((buffer->writePosition + CIRC_BUFFER_SIZE - buffer->readPosition) % CIRC_BUFFER_SIZE);
	}

	return retval;
}

BufferStatus bytesAvailableFromBuffer(Buffer *buffer, uint32_t *out_number)
{
	BufferStatus retval = BUFFER_SUCCESS;

	if ((NULL == buffer) || (NULL == out_number))
	{
		retval = BUFFER_ERROR_NULL;
	}

	if (BUFFER_SUCCESS == retval)
	{
		*out_number = ((buffer->readPosition + CIRC_BUFFER_SIZE - buffer->writePosition - 1) % CIRC_BUFFER_SIZE);
	}

	return retval;
}

void printBuffer(Buffer *read_buffer)
{
	BufferStatus retval = BUFFER_SUCCESS;
	uint32_t bytes_available = 0;

	retval = bufferIsFilled(read_buffer, &bytes_available);

	if (retval != BUFFER_SUCCESS)
	{
		printf("bytes_available:%d \n", bytes_available);
	}

	if (BUFFER_SUCCESS == retval)
	{
		uint8_t data;
		for (uint32_t i = 0; i < bytes_available; i++)
		{
			retval = peekFromBuffer(read_buffer, i, &data);

			if (BUFFER_SUCCESS != retval)
			{
				break;
			}

			printf("%02X", data);
		}
		printf("\n");
	}

	if (BUFFER_SUCCESS != retval)
	{
		printf("CircBuffer error: %d \n", retval);
	}
}
