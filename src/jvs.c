#include <circ_buffer.h>
#include "jvs.h"
#include "constants.h"
#include "definitions.h"




/* Select timeout in ms*/
#define TIMEOUT_SELECT 500

/* Use for timeout between received bytes */
time_t time_last_reception;
time_t time_current;

int deviceID = -1;
int debugEnabled = 1;

//uint8_t response[LINEAR_BUFFER_SIZE];
//uint8_t request[LINEAR_BUFFER_SIZE];

// todo: init read_buffer!!
circ_buffer_t read_buffer;

JVSPacket packetIn;
JVSPacket packetOut;

/*
 * todo:
 * -> Reception/Sending of data not byte-wise (use my receive and escape functions)
 * -> add indices to processPacket arrays?
 * -> Request for strings want a \0 terminated string in the response?
 * -> JVS IO struct: add deviceID, CMD_COMMAND_VERSION, CMD_JVS_VERSION, CMD_COMMS_VERSION, CMD_READ_SWITCHES with variable length
 * -> CMD_READ_COINS: use 16 bit
 * -> CMD_READ_ANALOGS, CMD_READ_ROTARY: Use number of bits specified by JVS IO capabilities
 * -> use mutex between IO threads and JVS thread and work with deep copy of JVSState not have inconsisten data?
 * */


// DEBUG STUFF
void print_msg (JVSPacket *msg)
{
  if(debugEnabled)
  {
    printf ("Data:\n");
    for (uint32_t i = 0; i < msg->length; i++)
    {
      printf ("%02X", msg->data[i]);
    }
    printf ("\n");
  }
}


open_jvs_status_t initJVS(char *devicePath, JVSCapabilities *capabilitiesSetup)
{
  open_jvs_status_t retval = OPEN_JVS_ERR_OK;
  /* Init the connection to the Naomi */
  initDevice(devicePath);

  /* Init the Virtual IO */
  retval = initIO(capabilitiesSetup);

  return retval;
}

int disconnectJVS()
{
	return closeDevice();
}

int writeCapabilities(JVSPacket *outputPacket, JVSCapabilities *capabilities)
{
	outputPacket->data[outputPacket->length] = REPORT_SUCCESS;
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

	// todo: Are these necessary?
	   /* Code 0x07: General purpose SW inputs */ 0x07, 0x00, 0x08, 0x00,
	              /* Code 0x13: Analog Output channels */ 0x13, 0x08, 0x00, 0x00,
	              /* Code 0x06: Enter screen position ???*/ 0x06, 0x08, 0x08, 0x02,
	              /* Code 0x12: General purpose driver */ 0x12,  0x08, 0x00, 0x00,

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

open_jvs_status_t processPacket(JVSPacket * inPacket, JVSPacket * outPacket)
{
  open_jvs_status_t retval = OPEN_JVS_NO_RESPONSE;
  // todo: use mutex between IO threads and JVS thread and work with deep copy of JVSState not have inconsisten data?
  JVSState *state = getState();
  JVSCapabilities *capabilities = getCapabilities();

  if((NULL == inPacket) || (NULL == outPacket) || (NULL == state) || (NULL == capabilities) )
  {
    printf("arg state:%p capabilities:%p \n", state,capabilities);
    retval = OPEN_JVS_ERR_NULL;
  }

  if(OPEN_JVS_NO_RESPONSE == retval)
  {
    uint8_t node_dest = inPacket->data[CMD_IDX_NODE_NUMBER];

    outPacket->length = 0;

    /* Check if we are the destination node */
    // DEBUG: debugEnabled
    if ((node_dest == NODE_BROADCAST) || (node_dest == deviceID) || debugEnabled)
    {
      outPacket->data[CMD_IDX_NODE_NUMBER] = NODE_BUS_MASTER;

      /* inPacketIndex always points at an CMD byte offset */
      uint32_t inPacketIndex = CMD_IDX_CMD_STATUS;

      printf("inPacketlen:%u \n", inPacket->length);
      //uint32_t inPacketPayloadLenght = GET_MSG_REQ_PAYLOAD_LEN(inPacket->data);
      uint32_t inPacketPayloadLenght = GET_MSG_REQ_LEN(inPacket->data);

      //uint32_t outPacketIndex = CMD_IDX_PAYLOAD;
      //uint32_t outPacketPayloadLength = CMD_LEN_HEADER + CMD_LEN_CMD;

      outPacket->length = CMD_LEN_HEADER + CMD_LEN_CMD;


      printf("inPacketIndex:%x :%x \n", inPacketIndex, (inPacketPayloadLenght - 1));

      while (inPacketIndex < (inPacketPayloadLenght - CMD_LEN_CHECKSUM))
      {
        uint32_t sizeCurrentCmd;

        // DEBUG ONLY
        printf("cmd:%x \n", inPacket->data[inPacketIndex]);
        switch (inPacket->data[inPacketIndex])
        {
          case CMD_RESET:
          {
            debug("CMD_RESET");
            deviceID = -1;
            setSyncPin(0);

            sizeCurrentCmd = CMD_LEN_CMD + 1;
          }
          break;

          case CMD_ASSIGN_ADDR:
          {
            debug("CMD_ASSIGN_ADDR");
            deviceID = (uint8_t) inPacket->data[inPacketIndex + CMD_LEN_CMD];

            outPacket->data[outPacket->length] = REPORT_SUCCESS;
            outPacket->length += 1;

            setSyncPin(1);

            sizeCurrentCmd = CMD_LEN_CMD + 1;
          }
          break;

          case CMD_REQUEST_ID:
          {
            debug("CMD_REQUEST_ID");

            outPacket->data[outPacket->length] = REPORT_SUCCESS;
            outPacket->length += 1;

            memcpy(&outPacket->data[outPacket->length], capabilities->name, strlen(capabilities->name));
            outPacket->length += strlen(capabilities->name);

            /* Add null termination */
            outPacket->data[outPacket->length] = 0;
            outPacket->length += 1;

            sizeCurrentCmd = CMD_LEN_CMD + 0;

          }
          break;

          case CMD_COMMAND_VERSION:
          {
            debug("CMD_COMMAND_VERSION");

            outPacket->data[outPacket->length] = REPORT_SUCCESS;
            outPacket->length += 1;

            outPacket->data[outPacket->length] = capabilities->jvs_command_version;
            outPacket->length += 1;

            sizeCurrentCmd = CMD_LEN_CMD + 0;
          }
          break;

          case CMD_JVS_VERSION:
          {
            debug("CMD_JVS_VERSION");
            outPacket->data[outPacket->length] = REPORT_SUCCESS;
            outPacket->length += 1;
            outPacket->data[outPacket->length] = capabilities->jvs_version;
            outPacket->length += 1;

            sizeCurrentCmd = CMD_LEN_CMD + 0;
          }
          break;

          case CMD_COMMS_VERSION:
          {
            debug("CMD_COMMS_VERSION");

            outPacket->data[outPacket->length] = REPORT_SUCCESS;
            outPacket->length += 1;
            outPacket->data[outPacket->length] = capabilities->jvs_com_version;
            outPacket->length += 1;

            sizeCurrentCmd = CMD_LEN_CMD + 0;
          }
          break;

          case CMD_CAPABILITIES:
          {
            debug("CMD_CAPABILITIES");
            writeCapabilities(outPacket, capabilities);

            sizeCurrentCmd = CMD_LEN_CMD + 0;
          }
          break;

          case CMD_READ_SWITCHES:
          {
            debug("CMD_READSWITCHES");

            uint8_t numberPlayers = inPacket->data[inPacketIndex + CMD_LEN_CMD + 0];
            uint8_t numberBytesPerPlayer = inPacket->data[inPacketIndex + CMD_LEN_CMD + 1];

            outPacket->data[outPacket->length] = REPORT_SUCCESS;
            outPacket->length += 1;

            /* System switches */
            outPacket->data[outPacket->length] = (state->inputSwitch[0][0]);
            outPacket->length += 1;

            /* Player switches according to reuest*/
            for (int i = 1; i < (numberPlayers + 1); i++)
            {
              for (int j = 0; j < numberBytesPerPlayer; j++)
              {
                outPacket->data[outPacket->length ] = (state->inputSwitch[i][j]);
                outPacket->length += 1;
              }
            }

            sizeCurrentCmd = CMD_LEN_CMD + 2;
          }
          break;

          case CMD_READ_COINS:
          {
            debug("CMD_READ_COINS\n");

            outPacket->data[outPacket->length] = REPORT_SUCCESS;
            outPacket->length += 1;

            outPacket->data[outPacket->length + 0] = 0x00;
            outPacket->data[outPacket->length + 1] = state->coinCount;
            outPacket->data[outPacket->length + 2] = 0x00;
            outPacket->data[outPacket->length + 3] = 0x00;
            outPacket->length += 4;

            sizeCurrentCmd = CMD_LEN_CMD + 1;

          }
          break;

          case CMD_READ_ANALOGS:
          {
            debug("CMD_READ_ANALOGS\n");

            uint8_t numberAnalogChannels = inPacket->data[inPacketIndex + CMD_LEN_CMD + 0];

            outPacket->data[outPacket->length] = REPORT_SUCCESS;
            outPacket->length += 1;

            for (uint32_t i = 0; i < numberAnalogChannels; i++)
            {
              outPacket->data[outPacket->length]  = (state->analogueChannel[i] >> 8 ) & (capabilities->analogueMask >> 8);
              outPacket->length += 1;

              outPacket->data[outPacket->length]  = (state->analogueChannel[i] >> 0 ) & (capabilities->analogueMask >> 0);
              outPacket->length += 1;
            }

            sizeCurrentCmd = CMD_LEN_CMD + 1;
          }
          break;

          case CMD_READ_ROTARY:
          {
            debug("CMD_READ_ROTARY\n");

            uint8_t numberAnalogChannels = inPacket->data[inPacketIndex + CMD_LEN_CMD + 0];

            outPacket->data[outPacket->length] = REPORT_SUCCESS;
            outPacket->length += 1;

            for (uint32_t i = 0; i < numberAnalogChannels; i++)
            {
              outPacket->data[outPacket->length]  = (state->analogueChannel[i] >> 8 );
              outPacket->length += 1;

              outPacket->data[outPacket->length]  = (state->analogueChannel[i] >> 0 );
              outPacket->length += 1;
            }

            sizeCurrentCmd = CMD_LEN_CMD + 1;
          }
          break;

          case CMD_DECREASE_COINS:
          {
            //uint8_t slot_assignment = inPacket->data[inPacketIndex + CMD_LEN_CMD + 0];
            uint16_t coin_decrement = ((uint16_t) (inPacket->data[inPacketIndex + CMD_LEN_CMD + 1]) | ((uint16_t) (inPacket->data[inPacketIndex + CMD_LEN_CMD + 2]) << 8));

            /* Prevent underflow of coins */
            if(coin_decrement > state->coinCount)
            {
              coin_decrement = state->coinCount;
            }
            state->coinCount -= coin_decrement;

            outPacket->data[outPacket->length] = REPORT_SUCCESS;
            outPacket->length += 1;

            sizeCurrentCmd = CMD_LEN_CMD + 3;
          }
          break;

          // todo:Commands missing compared with older version - but they might not be of importance
          // CMD_WRITEGPIO1, CMD_WRITEGPIOBYTE, CMD_WRITEGPIOBIT
          // CMD_SETMAINBOARDID
          // CMD_READSCREENPOS

          default:
          {
            retval = OPEN_JVS_ERR_INVALID_CMD;
            printf("Warning: This command is not properly supported [0x%02hhX]\n", inPacket->data[inPacketIndex]);
          }
          break;

        }

        inPacketIndex += sizeCurrentCmd;
      }

      /* Is there a response with payload to send? */
      if(outPacket->length > (CMD_LEN_HEADER + CMD_LEN_CMD))
      {
        /* Append byte for checksum */
        outPacket->length += 1;

        outPacket->data[CMD_IDX_SNY] = SYNC;
        outPacket->data[CMD_IDX_NODE_NUMBER] = NODE_BUS_MASTER;
        outPacket->data[CMD_IDX_NUMBER_BYTES_PAYLOAD] =  outPacket->length - (CMD_LEN_NUMBER_BYTES + CMD_LEN_NODE + CMD_LEN_SYNC);

        // todo: Also support other statuses?
        outPacket->data[CMD_IDX_CMD_STATUS] = STATUS_SUCCESS;

        retval = OPEN_JVS_ERR_OK;
      }
    }
  }

  return retval;
}




//int processPacket()
//{
//	JVSState *state = getState();
//	JVSCapabilities *capabilities = getCapabilities();
//
//	JVSPacket inPacket;
//
//	if (!readPacket(&inPacket))
//	{
//		printf("Error: Could not read packet\n");
//		return 0;
//	}
//
//	JVSPacket outputPacket;
//	outputPacket.length = 0;
//	outputPacket.destination = BUS_MASTER;
//
//	int index = 0;
//
//	while (index < inPacket.length - 1)
//	{
//		int size = 1;
//		switch (inPacket.data[index])
//		{
//		case CMD_RESET:
//			debug("CMD_RESET");
//			size = 2;
//			deviceID = -1;
//			setSyncPin(0);
//			break;
//		case CMD_ASSIGN_ADDR:
//			debug("CMD_ASSIGN_ADDR");
//			size = 2;
//			deviceID = inPacket.data[index + 1];
//			outputPacket.data[outputPacket.length] = STATUS_SUCCESS;
//			outputPacket.length += 1;
//			setSyncPin(1);
//			break;
//		case CMD_REQUEST_ID:
//			debug("CMD_REQUEST_ID");
//			outputPacket.data[outputPacket.length] = STATUS_SUCCESS;
//			memcpy(&outputPacket.data[outputPacket.length + 1], capabilities->name, strlen(capabilities->name) + 1);
//			outputPacket.length += strlen(capabilities->name) + 2;
//			break;
//		case CMD_COMMAND_VERSION:
//			debug("CMD_COMMAND_VERSION");
//			outputPacket.data[outputPacket.length] = STATUS_SUCCESS;
//			outputPacket.data[outputPacket.length + 1] = 0x11;
//			outputPacket.length += 2;
//			break;
//		case CMD_JVS_VERSION:
//			debug("CMD_JVS_VERSION");
//			outputPacket.data[outputPacket.length] = STATUS_SUCCESS;
//			outputPacket.data[outputPacket.length + 1] = 0x20;
//			outputPacket.length += 2;
//			break;
//		case CMD_COMMS_VERSION:
//			debug("CMD_COMMS_VERSION");
//			outputPacket.data[outputPacket.length] = STATUS_SUCCESS;
//			outputPacket.data[outputPacket.length + 1] = 0x10;
//			outputPacket.length += 2;
//			break;
//		case CMD_CAPABILITIES:
//			debug("CMD_CAPABILITIES");
//			writeCapabilities(&outputPacket, capabilities);
//			break;
//		case CMD_READ_SWITCHES:
//			debug("CMD_READSWITCHES");
//			size = 3;
//			outputPacket.data[outputPacket.length] = STATUS_SUCCESS;
//			outputPacket.length += 1;
//			for (int i = 0; i <= inPacket.data[index + 1]; i++)
//			{
//				for (int j = 0; j < inPacket.data[index + 2]; j++)
//				{
//					outputPacket.data[outputPacket.length] = (state->inputSwitch[i][j]);
//					outputPacket.length += 1;
//				}
//			}
//			break;
//		case CMD_READ_COINS:
//			debug("CMD_READ_COINS\n");
//			size = 2;
//			outputPacket.data[outputPacket.length] = STATUS_SUCCESS;
//			outputPacket.data[outputPacket.length + 1] = 0x00;
//			outputPacket.data[outputPacket.length + 2] = state->coinCount;
//			outputPacket.data[outputPacket.length + 3] = 0x00;
//			outputPacket.data[outputPacket.length + 4] = 0x00;
//			outputPacket.length += 5;
//			break;
//		case CMD_READ_ANALOGS:
//			debug("CMD_READ_ANALOGS\n");
//			size = 2;
//			outputPacket.data[outputPacket.length] = STATUS_SUCCESS;
//			outputPacket.length += 1;
//			for (int i = 0; i < inPacket.data[index + 1]; i++)
//			{
//				outputPacket.data[outputPacket.length] = state->analogueChannel[i];
//				outputPacket.data[outputPacket.length + 1] = 0x00;
//				outputPacket.length += 2;
//			}
//			break;
//		case CMD_READ_ROTARY:
//			debug("CMD_READ_ROTARY\n");
//			size = 2;
//			outputPacket.data[outputPacket.length] = STATUS_SUCCESS;
//			outputPacket.length += 1;
//			for (int i = 0; i < inPacket.data[index + 1]; i++)
//			{
//				outputPacket.data[outputPacket.length] = state->rotaryChannel[i];
//				outputPacket.data[outputPacket.length + 1] = 0x00;
//				outputPacket.length += 2;
//			}
//			break;
//		default:
//			printf("Warning: This command is not properly supported [0x%02hhX]\n", inPacket.data[index]);
//		}
//		index += size;
//	}
//
//	outputPacket.destination = BUS_MASTER;
//
//	return writePacket(&outputPacket);
//}

#ifdef OFFLINE_MODE

void test_buffer ()
{
  //uint8_t cmd[] = {0xE0, 0xFF, 0x03, 0xF0, 0xD9, 0xCB}; /* Reset ok */
  //uint8_t cmd[] = {0x23, 0x45, 0xFF ,0xE0, 0xFF, 0x03, 0xF0, 0xD9, 0xCB}; /* some bytes + Reset ok */
  //uint8_t cmd[] = {0x23, 0x45, 0xFF ,0xE0, 0xFF, 0x03, 0xF0, 0xD8, 0xCB}; /* some bytes + Reset with crc */
  //uint8_t cmd[] = {0x23, 0x45, 0xFF ,0xE0, 0xFF, 0x03, 0xF0, 0xD8}; /* some bytes + Reset with missing crc */

  //uint8_t cmd[] = {0xE0, 0xFF, 0x03, 0xF1, 0x01, 0xF4, 0xFF}; /* CMD_SETADDRESS */

  //uint8_t cmd[] = {0xE0, 0xFF, 0x03, 0xF1, CMD_ESCAPE, 0x01 -1, 0xF4, 0xFF}; /* CMD_SETADDRESS with escape injected*/
  //uint8_t cmd[] = {0xE0, 0x01, 0x02, 0x10, 0x13};  /* CMD_READID */

  uint8_t cmd[] = {0xE0, 0x01, 0x05, 0x11, 0x12, 0x13, 0x14, 0x50}; /* Multi Request */

  //uint8_t cmd[] = {0xE0, 0x01, 0x0D, 0x20, 0x02, 0x02, 0x22, 0x08, 0x21, 0x02, 0x32, 0x03, 0x00, 0x00, 0x00, 0xB4}; /* Multi Request*/

  //uint8_t cmd[] =  { 0xE0, 0xFF, 0x3, 0xef, 0x10, 0x01 }; /* Test CMD */

  for (uint32_t i = 0; i < sizeof(cmd); i++)
  {
    if (CIRC_BUFFER_ERR_OK != circ_buffer_push(&read_buffer, cmd[i]))
    {
      printf ("circ_buffer_push returned error!");
      exit (-1);
    }
  }
}

#endif

open_jvs_status_t jvs_do(void)
{

  // todo: find better structure
  static bool timeout_enable = false;

  open_jvs_status_t retval = OPEN_JVS_ERR_OK;
  //       uint32_t request_len_decoded;
  uint32_t request_len_raw;
  //       uint32_t response_len = 0;

#ifndef OFFLINE_MODE
  retval = read_serial(/*serial */ &read_buffer);
#else
  test_buffer();
#endif

  packetOut.length = 0;

  /* Reset inter-byte timeout */
  if (OPEN_JVS_ERR_OK == retval)
  {
    time(&time_last_reception);
    timeout_enable = true;
  }

  /* Find start sequence in circ buffer*/
  if ((OPEN_JVS_ERR_OK == retval) || (OPEN_JVS_ERR_WAIT_BYTES == retval))
  {
    retval = find_start_of_message(&read_buffer);

    if(debugEnabled)
    {
      debug("Received Message: \n");
      print_circ_buffer(&read_buffer);
    }
  }

  /* Remove escape sequence and store in request*/
  if (OPEN_JVS_ERR_OK == retval)
  {
    retval = decode_escape_circ(&read_buffer, &packetIn, &request_len_raw);

    if(debugEnabled)
    {
      printf("After decode_escape_circ: \n");
      print_msg(&packetIn);
    }
  }

  /* Check length and checksum */
  if (OPEN_JVS_ERR_OK == retval)
  {
    retval = check_message(&packetIn);

    if(debugEnabled)
    {
      printf("check_message: %d\n", retval);
    }

    /* Remove processed request from circ read-buffer */
    if (OPEN_JVS_ERR_OK == retval)
    {
      if (CIRC_BUFFER_ERR_OK != circ_buffer_discard(&read_buffer, request_len_raw))
      {
        retval = OPEN_JVS_ERR_REC_BUFFER;
        // DEBUG ONLY
        printf("circ_buffer_discard: %d\n", retval);
      }
    }
  }

  /* Process data and generate response if message is complete and valid */
  if (OPEN_JVS_ERR_OK == retval)
  {
    retval = processPacket(&packetIn, &packetOut);

    if(debugEnabled)
    {
      printf("processPacket:%u response_len:%u ", retval, packetOut.length);
      print_msg(&packetOut);
    }
  }

  if (OPEN_JVS_ERR_OK == retval)
  {
    /* Calculate checksum */
    packetOut.data[packetOut.length - 1] = calc_checksum(&packetOut.data[CMD_IDX_NODE_NUMBER], packetOut.length - 1 - CMD_LEN_CHECKSUM);
  }

  /* Encode escape sequence */
  if (OPEN_JVS_ERR_OK == retval)
  {
    retval = encode_escape(&packetOut);
  }

  /* Send response */
  if (OPEN_JVS_ERR_OK == retval)
  {
    if (packetOut.length > 0)
    {
#ifndef OFFLINE_MODE
      retval = write_serial (/*serial,*/ packetOut.data, packetOut.length);
#endif
    }
  }

  //       /* Remove processed request from circ read-buffer */
  //       if (OPEN_JVS_ERR_OK == retval)
  //       {
  //         if (CIRC_BUFFER_ERR_OK != circ_buffer_discard (&read_buffer, request_len_raw))
  //         {
  //           retval = OPEN_JVS_ERR_REC_BUFFER;
  //         }
  //       }

  /* Check for inter-byte timeout */
  time (&time_current);

  if ((((time_current - time_last_reception) > TIMEOUT_INTER_BYTE) && timeout_enable))
  {
    deviceID = 0xFFFF;
    /* Flush receive buffer and start over */
    circ_buffer_init (&read_buffer);

    setSyncPin(0);
    if (debugEnabled)
    {
      printf ("Timeout Reset buffer and address\n");
    }
  }

  return retval;
}


//int readPacket(JVSPacket *packet)
//{
//	unsigned char byte = 0;
//	int n = readByte(&byte);
//	while (byte != SYNC || n < 1)
//	{
//		n = readByte(&byte);
//	}
//
//	readByte(&packet->destination);
//	readByte(&packet->length);
//
//	unsigned char checksumComputed = packet->destination + packet->length;
//
//	for (int i = 0; i < packet->length - 1; i++)
//	{
//		readByte(&packet->data[i]);
//		if (packet->data[i] == ESCAPE)
//		{
//			readByte(&packet->data[i]);
//			packet->data[i] += 1;
//		}
//		checksumComputed = (checksumComputed + packet->data[i]) & 0xFF;
//	}
//	unsigned char checksumReceived = 0;
//	readByte(&checksumReceived);
//
//	if (checksumReceived != checksumComputed)
//	{
//		printf("Error: The checksum is not correct\n");
//		return 0;
//	}
//
//	return 1;
//}

//int writePacket(JVSPacket *packet)
//{
//	/* Don't return anything if there isn't anything to write! */
//	if (packet->length < 1)
//	{
//		return 1;
//	}
//
//	writeByte(SYNC);
//	writeByte(packet->destination);
//	writeByte(packet->length + 2);
//
//	writeByte(STATUS_SUCCESS);
//	unsigned char checksum = packet->destination + packet->length + 2 + STATUS_SUCCESS;
//	for (int i = 0; i < packet->length; i++)
//	{
//		if (packet->data[i] == SYNC || packet->data[i] == ESCAPE)
//		{
//			writeByte(ESCAPE);
//			writeByte(packet->data[i] - 1);
//		}
//		else
//		{
//			writeByte(packet->data[i]);
//		}
//		checksum = (checksum + packet->data[i]) & 0xFF;
//	}
//	writeByte(checksum);
//	return 1;
//}


open_jvs_status_t find_start_of_message(circ_buffer_t * read_buffer)
{
  uint32_t bytes_available;

  open_jvs_status_t retval = OPEN_JVS_ERR_OK;

  if (OPEN_JVS_ERR_OK == retval)
  {
    if (CIRC_BUFFER_ERR_OK != circ_buffer_filled (read_buffer, &bytes_available))
    {
      retval = OPEN_JVS_ERR_REC_BUFFER;
    }
  }

  if (OPEN_JVS_ERR_OK == retval)
  {
    uint8_t data;
    uint32_t i;
    bool found_start = false;

    /* Find start of message */
    for (i = 0; i < bytes_available; i++)
    {
      if (CIRC_BUFFER_ERR_OK != circ_buffer_peek (read_buffer, i, &data))
      {
        retval = OPEN_JVS_ERR_REC_BUFFER;
      }

      if (OPEN_JVS_ERR_OK == retval)
      {
        if (SYNC == data)
        {
          found_start = true;
          break;
        }
      }

      if (OPEN_JVS_ERR_OK != retval)
      {
        break;
      }
    }

    if (OPEN_JVS_ERR_OK == retval)
    {

      if (!found_start)
      {
        /* Get rid of all bytes including the current one when there is not CMD_SYNC */
        i += 1;
      }

      /* Discard bytes before Sync */
      if (0 != i)
      {
        if (CIRC_BUFFER_ERR_OK != circ_buffer_discard (read_buffer, i))
        {
          retval = OPEN_JVS_ERR_REC_BUFFER;
        }
      }

      if (OPEN_JVS_ERR_OK == retval)
      {
        if (!found_start)
        {
          retval = OPEN_JVS_ERR_SYNC_BYTE;
        }
      }
    }
  }

  return retval;
}

open_jvs_status_t decode_escape_circ(circ_buffer_t *read_buffer, JVSPacket *out_packet, uint32_t *out_raw_length)
{
  open_jvs_status_t retval = OPEN_JVS_ERR_OK;
  uint32_t i, j = 0;
  bool next_escaped = false;
  uint32_t len_buffer_circ;
  uint32_t len_buffer_decoded;
  uint32_t len_buffer_raw;
  uint8_t byte;

  if (OPEN_JVS_ERR_OK == retval)
  {
    if ((NULL == out_packet) || (NULL == out_raw_length))
    {
      retval = OPEN_JVS_ERR_NULL;
    }
  }

  if (OPEN_JVS_ERR_OK == retval)
  {
    if (CIRC_BUFFER_ERR_OK != circ_buffer_filled (read_buffer, &len_buffer_circ))
    {
      retval = OPEN_JVS_ERR_REC_BUFFER;
    }
  }

  if (OPEN_JVS_ERR_OK == retval)
  {
    /* Limit number of bytes copied to size of linear buffer */
    if (len_buffer_circ > sizeof(out_packet->data))
    {
      len_buffer_decoded = len_buffer_circ = sizeof(out_packet->data);
    }
    else
    {
      len_buffer_decoded = len_buffer_circ;
    }

    for (i = 0; i < len_buffer_circ; i++)
    {
      if (CIRC_BUFFER_ERR_OK != circ_buffer_peek (read_buffer, i, &byte))
      {
        retval = OPEN_JVS_ERR_REC_BUFFER;
        break;
      }

      if (byte == ESCAPE)
      {
        /* Remove escape sequence */
        next_escaped = true;
        len_buffer_decoded--;
      }
      else if (next_escaped)
      {
        out_packet->data[j] = byte + 1;
        next_escaped = false;
        j++;
      }
      else
      {
        /* Normal bytes */
        out_packet->data[j] = byte;
        j++;
      }
    }
  }

  if (OPEN_JVS_ERR_OK == retval)
  {
    *out_raw_length = len_buffer_circ;
    out_packet->length = len_buffer_decoded;
  }

  return retval;
}

open_jvs_status_t check_checksum(JVSPacket *packet)
{
  open_jvs_status_t retval = OPEN_JVS_ERR_OK;

  if (NULL == packet)
  {
    retval = OPEN_JVS_ERR_NULL;
  }

  if (OPEN_JVS_ERR_OK == retval)
  {
    uint8_t message_len = GET_MSG_REQ_LEN(packet->data);

    if (calc_checksum ((packet->data + CMD_LEN_SYNC), message_len - CMD_LEN_SYNC - CMD_LEN_CHECKSUM) != packet->data[packet->length - 1])
    {
      retval = OPEN_JVS_ERR_CHECKSUM;
    }
  }

  return retval;
}

open_jvs_status_t check_message (JVSPacket * packet)
{
  open_jvs_status_t retval = OPEN_JVS_ERR_OK;

  if(NULL == packet)
  {
    retval = OPEN_JVS_ERR_NULL;
  }

  /* Check if we have enough bytes to process packet*/
  if (OPEN_JVS_ERR_OK == retval)
  {
    if ((packet->length < CMD_LEN_HEADER) || (packet->length  < (GET_MSG_REQ_LEN(packet->data))))
    {
      retval = OPEN_JVS_ERR_WAIT_BYTES;
    }
  }

  /* Message received completely */
  if (OPEN_JVS_ERR_OK == retval)
  {
    /* Check checksum */
    retval = check_checksum(packet);
  }

  return retval;
}

uint8_t calc_checksum(uint8_t *message, uint8_t len)
{
  uint8_t checksum_calc = 0;

  for (uint8_t i = 0; i < len; i++)
  {
    checksum_calc += message[i];
  }

  return checksum_calc;
}

open_jvs_status_t encode_escape(JVSPacket *packet)
{
  open_jvs_status_t retval = OPEN_JVS_ERR_OK;

  uint8_t temp[sizeof(packet->data)];
  uint32_t len_new = 0;
  uint32_t i, j;

  if(NULL == packet)
  {
    retval = OPEN_JVS_ERR_NULL;
  }

  if (OPEN_JVS_ERR_OK == retval)
  {
    uint32_t len_new = packet->length;

    for (i = j = CMD_LEN_SYNC; i < packet->length; i++, j++)
    {
      if ((i >= sizeof(packet->data)) || (j >= sizeof(packet->data)))
      {
        retval = OPEN_JVS_ERR_PACKET_BUFFER_OVERFLOW;
        break;
      }

      if (packet->data[i] == SYNC || packet->data[i] == ESCAPE)
      {
        /* Insert escape sequence */
        temp[j] = ESCAPE;
        j++;

        temp[j] = packet->data[i] - 1;

        len_new++;
      }
      else
      {
        temp[j] = packet->data[i];
      }
    }

    /* Copy escaped packet */
    if (len_new != packet->length)
    {
      memcpy (packet, temp, len_new);
    }
  }

  return len_new;
}
