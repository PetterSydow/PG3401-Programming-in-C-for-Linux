/* UTIL.H-----------------------------------------------------------------------

	PG3401 EXAM 2026 Spring

	Autor:   candidate 262   
	Description: Header for utility functions and shared constants.
                Contains server and protocol defines, buffer sizes, cmdln
                validation, EWA header constraints, accept string
                creation, and data parsing.
	
------------------------------------------------------------------------------*/

#ifndef UTIL_H
#define UTIL_H

#include "ewpdef.h"

#define SERVER_IP "127.0.0.1"
#define PROTOCOL_NAME "SMTP"
/* Limits user input for server ID*/
#define ID_SIZE 9

/* Defines buffer sizes*/
#define ACCEPT_BUFFER_SIZE 128
#define REPLY_BUFFER_SIZE 128
#define HELO_BUFFER_SIZE 64
/* Transfer data size limits and sets CRLF CRLF . CRLF EOF marker*/
#define FILENAME_SIZE 50
#define MAX_DATA_SIZE 9998
#define FILE_END_MARKER "\r\n\r\n.\r\n"


int IsValidInteger(char *pszString);

int CheckCliInput(int iArgc, char *apszArgv[], int *piPort, char *pszId, int iIdSize);

void FillSizeHeader(struct EWA_EXAM25_TASK5_PROTOCOL_SIZEHEADER *pHead, const char *pszSize);

int CreateAcceptString(char *pszBuffer, char *pszServerId);

int GetHeaderDataSize(struct EWA_EXAM25_TASK5_PROTOCOL_SIZEHEADER *pHead);

#endif


/*--EOF-----------------------------------------------------------------------*/
