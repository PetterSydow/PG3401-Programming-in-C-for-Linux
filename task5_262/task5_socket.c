/* TASK5_SOCKET.C---------------------------------------------------------------


      Author: candidate 262  
      Description: Socket functions for task 5, creates the "tcp" server socket,   
                   binding to 127.0.0.1 and provided port number. Listening for 
                   incoming connections & accepting EWA.  


------------------------------------------------------------------------------*/

/*** Incldues *****************************************************************/
   
   /* Standard C libraries*/
   #include <string.h>
   #include <stdio.h>

   /* Linux/ socket related libraries*/
   #include <sys/socket.h>
   #include <sys/types.h>
   #include <netinet/in.h>
   #include <arpa/inet.h>
   #include <unistd.h>

   /* Task spesific libraries*/
   #include "task5_util.h"
   #include "task5_socket.h"

  

/*******************************************************************************
**** SOCKET METHODS
*******************************************************************************/

/*** Create "TCP" socket, bind to loopback, start listening********************/
int CreateServerSocket(int iPort) {
   int iServerSocket = 0;
   int SocketResult = 0;
   int iReuse = 1;   /* used by SO_REUSEADDR*/
   struct sockaddr_in stServerAddr;
   
   /* Innitializes server address structure prior to using it*/
   memset(&stServerAddr, 0, sizeof(stServerAddr));

   /* Lecture 10*/
   iServerSocket = socket(AF_INET, SOCK_STREAM, 0);
   if(iServerSocket < 0){
      perror("ERROR: socket failed with");
      return -1;
   }

   /* allows fast reuse of port after restarting the program*/
   SocketResult = setsockopt(iServerSocket, SOL_SOCKET, SO_REUSEADDR, &iReuse, sizeof(iReuse));
   
   /* Secures that setsockopt; set socket option, completed successfully*/
   if (SocketResult < 0) {
      perror("ERROR: setsocketoptions failed.");
      close(iServerSocket);
      return -1;
   }

   /* Fill server address family, port and loopback addr*/
   stServerAddr.sin_family = AF_INET;   /* Lecture 10*/
   stServerAddr.sin_port = htons((unsigned short)iPort);
   stServerAddr.sin_addr.s_addr = inet_addr(SERVER_IP); 
   /*127.0.0.1 htonl(0x7F000001*/

   /* Binds socket to configured IP address:port*/
   SocketResult = bind(iServerSocket, (struct sockaddr *)&stServerAddr, sizeof(stServerAddr));

   /* Checks if bind was completed successfully*/
   if (SocketResult < 0) {
      perror("ERROR: bind failed");
      close(iServerSocket); 
      return -1;
   }

   /* Starts listening for incoming connections*/
   SocketResult = listen(iServerSocket, MAX_CLIENTS);

   /* Check if listen was completed successfully */
   if (SocketResult < 0) {
      perror("ERROR: something failed to listen, again");
      close(iServerSocket);
      return -1;
   }

   return iServerSocket;
}

/*** Accept one Client connection *********************************************/
int AcceptClient(int iServerSocket) {
   int iClientSocket = -1;
   socklen_t iClientLen = 0;
   struct sockaddr_in stClientAddr;

   /* Clear client addr structure befoure accepting connection*/
   memset(&stClientAddr, 0, sizeof(stClientAddr));
   
   /* set size of client addr*/
   iClientLen = sizeof(stClientAddr);
   
   /* Accepting incoming connection*/
   iClientSocket = accept(iServerSocket, (struct sockaddr *)&stClientAddr, &iClientLen);

   /* Validate that client was added succsessfully*/
   if (iClientSocket < 0) {
   perror("ERROR: accept failed");
   return -1;
   }

   /* Print connected client IP address*/
   printf("Client connected from %s\n", inet_ntoa(stClientAddr.sin_addr));

   return iClientSocket;
}

/*--EOF-----------------------------------------------------------------------*/
