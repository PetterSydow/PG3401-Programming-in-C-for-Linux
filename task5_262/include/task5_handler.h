/* HANDLER.H--------------------------------------------------------------------

	PG3401 EXAM 2026 Spring

	Autor:  candidate 262   
	Description: EWA/tcp Protocol handle the EWA client communication, 
                     including SERVERACCEPT, HELO, MAIL FROM, RCPT TO, DATA, 
                     file transfer, QUIT and server replies.

------------------------------------------------------------------------------*/

#ifndef HANDLER_H
#define HANDLER_H

int HandleHelo(int iSocket);

int HandleClient(int iSocket, char *pszServerId);

int HandleMailFrom(int iSocket);

int HandleRcptTo(int iSocket);

int HandleDataCommand(int iSocket, char *pszFileName, int iFileNameSize);

int HandleFileTransfer(int iSocket, char *pszFileName);

int HandleDataOrQuit(int iSocket, int *piDone);

int SendAccept(int iSocket, char *pszServerId);

int SendReply(int iSocket, char *pszStatusCode, char *pszMessage);





#endif

/*--EOF-----------------------------------------------------------------------*/
