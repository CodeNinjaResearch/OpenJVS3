/*
 * buffer.h
 *
 *  Created on: 19.01.2020
 *      Author: RedOne87
 */

#include <stdint.h>
#include <stdbool.h>

#ifndef BUFFER_H_
#define BUFFER_H_

#define CIRC_BUFFER_SIZE 512
#define LINEAR_BUFFER_SIZE 512

typedef struct
{
	uint8_t data[CIRC_BUFFER_SIZE];
	uint32_t readPosition;
	uint32_t writePosition;
} Buffer;

typedef enum
{
	BUFFER_SUCCESS = 0,
	BUFFER_ERR_EMPTY,
	BUFFER_ERROR_FULL,
	BUFFER_ERROR_NULL,
	BUFFER_ERR_OFFSET,

} BufferStatus;

void initBuffer(Buffer *buffer);
BufferStatus pushToBuffer(Buffer *buffer, uint8_t data);
BufferStatus popFromBuffer(Buffer *buffer, uint8_t *out_data);
BufferStatus peekFromBuffer(Buffer *buffer, uint32_t offset, uint8_t *out_data);
BufferStatus bufferIsFilled(Buffer *buffer, uint32_t *out_number);
BufferStatus bytesAvailableFromBuffer(Buffer *buffer, uint32_t *out_number);
BufferStatus discardFromBuffer(Buffer *buffer, uint32_t number_bytes);

void printBuffer(Buffer *read_buffer);

#endif // BUFFER_H_
