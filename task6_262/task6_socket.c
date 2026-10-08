/* TASK6_SOCKET.C---------------------------------------------------------------


	Author: candidate 262  
	Description: Task 5 Handeled the socket, creates the "tcp" socket, binding to
 	loopback. Listening for incoming connections & accepting clients (1)EWA.
        Here used in rewerse to connect to EWA.

------------------------------------------------------------------------------*/

/*** Incldues *****************************************************************/
   
   /* Standard C libraries*/
   #include <string.h>
   #include <stdio.h>

   /* Linux/ socket related libraries*/
   #include <sys/socket.h>
   #include <sys/types.h>
   #include <netinet/in.h>  /* check if standard*/
   #include <arpa/inet.h>
   #include <unistd.h>

   /* Task spesific libraries*/
   #include "task6_util.h"
   #include "task6_socket.h"

  

/*******************************************************************************
**** SOCKET METHODS
*******************************************************************************/

/*** Create "TCP" socket, bind to loopback, start listening********************/
int ConnectToServer(char *pszServerAddress, int iPort) {
   int iSocket = -1;
   int SocketResult = 0;
   struct sockaddr_in stServerAddr;
   
   /* Innitializes server address structure prior to using it*/
   memset(&stServerAddr, 0, sizeof(stServerAddr));

   /* Create TCP socket*/
   iSocket = socket(AF_INET, SOCK_STREAM, 0);
   if(iSocket < 0){
      perror("ERROR: socket failed");
      return -1;
   }
   
   /* Fill server address family, port and loopback addr*/
   stServerAddr.sin_family = AF_INET;   /* Lecture 10*/
   stServerAddr.sin_port = htons((unsigned short)iPort);
   stServerAddr.sin_addr.s_addr = inet_addr(pszServerAddress); 
   /*127.0.0.1 htonl(0x7F000001*/

   /* Checks if bind was completed successfully*/
   if (stServerAddr.sin_addr.s_addr == INADDR_NONE) {
      perror("ERROR: Invalid server IP address.");
      close(iSocket); 
      return -1;
   }

   /* Connect to EWA*/
   SocketResult = connect(iSocket, (struct sockaddr *)&stServerAddr, sizeof(stServerAddr));

   /* Check if connection complted */
   if (SocketResult < 0) {
      perror("ERROR: connection failed");
      close(iSocket);
      return -1;
   }

   return iSocket;
}

/*--EOF-----------------------------------------------------------------------*/
