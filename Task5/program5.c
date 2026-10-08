/* PROGRAM5.C-------------------------------------------------------------------

	PG3401 EXAM 2026 Spring
	Version: Hello
	Autor:  candidate 262   
	Description: Main func for server. Starts the server, waits for EWA, 
                     when connected passes EWA over to the protocol handler 
                     and closes sockets befoure exit.


------------------------------------------------------------------------------*/

/*** Includes *****************************************************************/
   
   /* standard libs*/
   #include <string.h>
   #include <stdio.h>
   #include <unistd.h>

   /* task spesific libs*/
   #include "task5_handler.h"
   #include "task5_socket.h"
   #include "task5_util.h"

/*******************************************************************************
**** Main
*******************************************************************************/

int main(int iArgc, char *apszArgv[]) {
   /*Initialize local*/
   int iPort = 0;
   int iServerSocket = -1;
   int iClientSocket = -1;
   int Status = 0;
   char szServerId[ID_SIZE];

   /* Clear server Id buffer prior to using it*/
   memset(szServerId, 0, sizeof(szServerId));	

   puts("*** Test Server ***"); 

   /* Check cmdln input and get port/server ID*/
   Status = CheckCliInput(iArgc, apszArgv, &iPort, szServerId, sizeof(szServerId));
   if (Status < 0) {
      return 1;
   }
   
   printf("Starting server on 127.0.0.1:%d with ID \"%s\"\n", iPort, szServerId);

   /* Create server socket, bind client to a port and start listening*/
   iServerSocket = CreateServerSocket(iPort);
   if (iServerSocket < 0) {
      return 1;
   }

   puts("*** Server listening. Waiting for EWA connection ***"); 

   /* Accept a connection*/
   iClientSocket = AcceptClient(iServerSocket);
   if (iClientSocket < 0) {
      close(iServerSocket);
      return 1;
   }

   puts("*** Client connected ***");

   /* Handle client, passes accepted client over through to protocol handler*/
   Status = HandleClient(iClientSocket, szServerId);
   if (Status < 0) {
      puts("ERROR: Client handeling failed, EWA dropped.");   
      close(iClientSocket);
      close(iServerSocket);
      return 1;
   }

   /* Close connection, terminate clients then server socket*/
   close(iClientSocket);
   iClientSocket = -1;
   /* Stop listening over socket*/
   close(iServerSocket);
   iServerSocket = -1;

   puts("*** Server closing test done sucessfully! ***");

   return 0;
}

/*--EOF-----------------------------------------------------------------------*/
