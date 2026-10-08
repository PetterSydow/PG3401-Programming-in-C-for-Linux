/* SOCKET.H--------------------------------------------------------------------

	PG3401 EXAM 2026 Spring

	Autor:  candidate 262   
	Description: Header for socket.c for task 5 socket funtions. Creates the
	             server socket, bind, listen on selected port & accepts the
                     connection of EWA.

------------------------------------------------------------------------------*/

#ifndef SOCKET_H
#define SOCKET_H

 /* maximum waiting connection for listen*/ 
#define MAX_CLIENTS 5
 
   /* create TCP socket, bind, listen*/
int CreateServerSocket(int iPort);
   /* accepts 1 client*/
int AcceptClient(int iServerSocket);

#endif

/*--EOF-----------------------------------------------------------------------*/

