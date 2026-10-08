/* PROGRAM6.C-------------------------------------------------------------

	PG3401 EXAM 2026 Spring
	Version: Hello
	Autor:  candidate 262   
	Description: Main program file for Task 6. Handles the overall program 
             flow: reads and validates user input, connect to EWA, call the 
             protocol receive function, and clean up socket before returning.

------------------------------------------------------------------------------*/

/*** Includes *****************************************************************/
   
   /* standard libs*/
   #include <string.h>
   #include <stdio.h>
   #include <unistd.h>

   /* task spesific libs*/
   /*#include "task6_handler.h"*/
   #include "task6_socket.h"
   #include "task6_util.h"
   #include "task6_protocol.h"

/*******************************************************************************
**** Main
*******************************************************************************/

int main(int iArgc, char *apszArgv[]) {
   /*Initialize local*/
   int iPort = 0;
   int iSocket = -1;
   int Status = 0;
   char szServerAddress[SERVER_ADDRESS_SIZE];

   /* Clear server address buffer prior to using it*/
   memset(szServerAddress, 0, sizeof(szServerAddress));	

   puts("*** Task 6: Test Server ***"); 

   /* Check cmdln input and get port/server address*/
   Status = CheckCliInput(iArgc, apszArgv, szServerAddress, sizeof(szServerAddress), &iPort);
   if (Status < 0) {
      return 1;
   }
   
   printf("Connecting to EWA on on %s:%d\n", szServerAddress, iPort);

   /* Create server socket, bind client to a port and start listening*/
   iSocket = ConnectToServer(szServerAddress, iPort);
   if (iSocket < 0) {
      return 1;
   }

   puts("*** Connected to EWA ***");

   /* Receive BMP file from EWA using custom protocol */
   Status = ReceiveFileFromEwa(iSocket);
   if (Status < 0) {
      puts("ERROR: Failed while receiving file from EWA.");
      close(iSocket);
      return 1;
   }

   close(iSocket);
   iSocket = -1;

   puts("*** Task 6. Server closing test done sucessfully! ***");

   return 0;
}

/*--EOF-----------------------------------------------------------------------*/
