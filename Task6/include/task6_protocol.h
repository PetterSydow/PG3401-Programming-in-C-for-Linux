/* TASK6_PROTOCOL.H----------------------------------------------------------------

	PG3401 EXAM 2026 Spring

	Author:   candidate 262   
	Description: Protocol header for task 6, most defines also in here.

------------------------------------------------------------------------------*/

#ifndef TASK6_PROTOCOL_H
#define TASK6_PROTOCOL_H

#include "ewpdef.h"

#define PROTOCOL_HEADER_SIZE 20
/* Went with maximum value of an unsigned short*/
#define MAX_PACKET_DATA_SIZE 65535
/* Packet flags used in ucFlags*/
#define FLAG_FIN 0x01
#define FLAG_ACK 0x10
#define FLAG_NACK 0x20

/* Calculate checksum using little endian 16-bit words*/
unsigned short CalculateChecksumLittle(unsigned char *pData, int iLength);

/* Receive full file from EWA and save as task6_received.bmp*/
int ReceiveFileFromEwa(int iSocket);

int ReceiveFirstPacketTest(int iSocket);

int SendAckTest(int iSocket, struct EWA_EXAM25_TASK4_PROTOCOL_TCP *pReceivedHeader);

unsigned short Swap16(unsigned short usValue);

unsigned short CalculateChecksum(unsigned char *pData, int iLength);

#endif

/*--EOF-----------------------------------------------------------------------*/
