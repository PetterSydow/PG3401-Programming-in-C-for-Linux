/* TASK5_HANDLER.C-------------------------------------------------------------

	PG3401 EXAM 2026 Spring

	Autor:  candidate 262   
	Description: Handle server EWA protocol communication related
	             tasks with the client. Serveraccept> helo > 
		     mail from > rcpt to > data and quit commands > recives
                     file name & validates this > receivs file content  > saves   			     received file to validated name. 

------------------------------------------------------------------------------*/

/*** Incldues *****************************************************************/
   
   /* standard & networking libs*/
   #include <string.h>
   #include <stdlib.h>
   #include <stdio.h>
   #include <sys/socket.h>

   /* task spesific libs*/
   #include "ewpdef.h"
   #include "task5_handler.h"
   #include "task5_util.h"

/********************************************************************************
***** SERVER STATUS ENUM ********************************************************
********************************************************************************/

enum SERVER_STATUS {
   EXPECT_HELO,
   EXPECT_MAILFROM,
   EXPECT_RCPTTO,
   EXPECT_DATA_OR_QUIT,
   RECEIVING_FILE,
   DONE
};

/*******************************************************************************
**** PROTOCOL HANDLER METHODS
*******************************************************************************/


/*** Sends innital 220 server accept msg ***/
int SendAccept(int iSocket, char *pszServerId) {
   int SendStatus = 0;
   int StringStatus = 0;
   struct EWA_EXAM25_TASK5_PROTOCOL_SERVERACCEPT stAccept;
   char szFormatted[ACCEPT_BUFFER_SIZE];

   /* Clear protocol struct prior to filling it up*/
   memset(&stAccept, 0, sizeof(stAccept));

   /* Clear tmp formated string buffer prior to use*/
   memset(szFormatted, 0, sizeof(szFormatted));

   /* Fills EWA protocol header*/
   FillSizeHeader(&stAccept.stHead, "0064");

   /* Fill server ready status code*/
   memcpy(stAccept.acStatusCode, "220", 3);

   stAccept.acHardSpace[0] = ' ';   /*  0x20;*/

   /* Making formatted string for accept msg text*/
   StringStatus = CreateAcceptString(szFormatted, pszServerId);
   if (StringStatus < 0) {
      puts("ERROR: Failed to create accept string.");
      return -1;
   }   

   /* copy forward formatted message to struct protocol */
   strcpy(stAccept.acFormattedString, szFormatted);

   /* Make sure of hard zero termination*/
   stAccept.acHardZero[0] = '\0';

   /* Send complete server accept struct*/
   SendStatus = send(iSocket, &stAccept, sizeof(stAccept), 0);

   /* Validate if whether we managed to send or not*/
   if (SendStatus != sizeof(stAccept)) {
      puts("ERROR: Failed to so send servAccept.");
      return -1;
   }

   /* Check what was sent*/
   printf("Sent accept: %.3s %s\n", 
	stAccept.acStatusCode, 
	stAccept.acFormattedString);

   return 0;
}

/*** Send Server reply ********************************************************/

int SendReply(int iSocket, char *pszStatusCode, char *pszMessage) {
   int SendStatus = 0;
   struct EWA_EXAM25_TASK5_PROTOCOL_SERVERREPLY stReply;

   /* Clear rply struct prior to filling*/
   memset(&stReply, 0, sizeof(stReply));

   /* fill EWA protocol header*/
   FillSizeHeader(&stReply.stHead, "0064");

   memcpy(stReply.acStatusCode, pszStatusCode, 3);

   stReply.acHardSpace[0] = ' ';
   
   /* Copy reply hard space after status code*/
   strncpy(stReply.acFormattedString, pszMessage, sizeof(stReply.acFormattedString) -1);

   /* ensure sero termination byte in protoc struct*/
   stReply.acHardZero[0] = '\0';

   /* Send complete server rply struct*/
   SendStatus = send(iSocket, &stReply, sizeof(stReply), 0);

   /* validate the full struct was sent*/
   if (SendStatus != sizeof(stReply)) {
      puts("ERROR: Failed to send server reply");
      return -1;
   }

   /* print what was sent to check... */
   printf("Sent reply: %.3s %s\n", 
	stReply.acStatusCode, 
	stReply.acFormattedString);

   return 0;
}

/***** Handle HELO and send 250 reply ****************************************/
int HandleHelo(int iSocket) {
   int ReceiveStatus = 0;
   int ReplyStatus = 0;
   struct EWA_EXAM25_TASK5_PROTOCOL_CLIENTHELO stHelo;
   char szHeloText[HELO_BUFFER_SIZE]; /* yep they are frens now*/
   char szReply[REPLY_BUFFER_SIZE];
   char *pszDot = NULL;
   char *pszClientName = NULL;
   char *pszClientIp = NULL;

   /*Clear recieved Helo struct prior to using, sending pluss w local buffers*/
   memset(&stHelo, 0, sizeof(stHelo));
   memset(szHeloText, 0, sizeof(szHeloText));
   memset(szReply, 0, sizeof(szReply));
   
   /* get complete Helo msg from client*/
   ReceiveStatus = recv(iSocket, &stHelo, sizeof(stHelo), 0);

   /* validate this msg*/
   if (ReceiveStatus != sizeof(stHelo)) {
      puts("ERROR: Failed to get Helo packet.");
      return -1;
   }
   
   /* check the message */ 
   if (memcmp(stHelo.acCommand, "HELO", 4) != 0) {
      puts("ERROR: In Expected Helo message in packet.");
      return -1;
   }
   
   /* copy message into local buffer prior to parsning*/
   memcpy(szHeloText, stHelo.acFormattedString, sizeof(stHelo.acFormattedString));

   pszDot = strchr(szHeloText, '.');
   
   /* split client name | Ip, if sepperator is present*/
   if(pszDot != NULL) {
      *pszDot = '\0';
      pszClientName = szHeloText;
      pszClientIp = pszDot + 1;
   } else {
      pszClientName = "client";
      pszClientIp = SERVER_IP;
   }
   /* make helo message, hello helo hm*/
   strcpy(szReply, pszClientIp);
   strcat(szReply, " Hello ");
   strcat(szReply, pszClientName);

   /* printf for verification test*/
   printf("Recived Helo: %s.%s\n", pszClientName, pszClientIp);

   /* Send 250 reply after accepted HELO*/
   ReplyStatus = SendReply(iSocket, "250", szReply);
   if (ReplyStatus < 0) {
      return -1;
   }

   return 0;
}

/***** Handle getting EWA mail & Reply with 250 *******************************/

int HandleMailFrom(int iSocket) {
   int ReceiveStatus = 0;
   int ReplyStatus = 0;
   struct EWA_EXAM25_TASK5_PROTOCOL_MAILFROM stMailFrom;
   /* Initialize*/
   memset(&stMailFrom, 0, sizeof(stMailFrom));

   ReceiveStatus = recv(iSocket, &stMailFrom, sizeof(stMailFrom), 0);

   /* Complain, no mail, step 4*/
   if(ReceiveStatus != sizeof(stMailFrom)) {
      puts("ERROR: Failed to get my mail!");
      return -1;
   }

   /* Validate mail*/
   if (memcmp(stMailFrom.acCommand, "MAIL FROM:", 10) !=0) {
      puts("ERROR: Expected |MAIL FROM| command");
      return -1;
   }

   /* Verify & check EWA mail*/
   printf("Recieved MAIL FROM: %s\n", stMailFrom.acFormattedString);

   /* Send 250 reply back*/
   ReplyStatus = SendReply(iSocket, "250", "Message recieved");
   if (ReplyStatus < 0) {
      return -1;
   }

   return 0;
}

/***** Handle recieving RCPT to, Reply with 250 *******************************/
 
int HandleRcptTo(int iSocket) {
   int ReceiveStatus = 0;
   int ReplyStatus = 0;
   struct EWA_EXAM25_TASK5_PROTOCOL_RCPTTO stRcptTo;

   /* Init prior to filling with goodies*/
   memset(&stRcptTo, 0, sizeof(stRcptTo));

   ReceiveStatus = recv(iSocket, &stRcptTo, sizeof(stRcptTo), 0); 

   /* Validate that the full stRcptTo struct was obtained*/
   if (ReceiveStatus != sizeof(stRcptTo)) {
      puts("ERROR: Failed to get the |RCPT TO| packet.");
      return -1;
   }

   /* Check if the packet contains the "RCPT TO" command*/
   if(memcmp(stRcptTo.acCommand, "RCPT TO:", 8) != 0) {
      puts("ERROR: Did not get |RCPT TO| command.");
      return -1;
   }

   /* Print message recived from client*/
   printf("Recieved RCPT TO: %s\n", stRcptTo.acFormattedString);

   /* Send 250 back after obtained RCPTTO command*/
   ReplyStatus = SendReply(iSocket, "250", "message recieved ok");
   if (ReplyStatus < 0) {
      return -1;
   }

   return 0;
}

/***** Validate DATA filename *************************************************/

int IsValidFileName(char *pszFileName) {
   int iLength = 0;
   /* checks filename pointer*/
   if (pszFileName == NULL) {
      return 0;
   }
   /* Get filename length*/
   iLength = strlen(pszFileName);
  
   /* Checks for empty case*/
   if (iLength <= 0) {
      return 0;
   }

   /* Check if filename fits accepted buffer length [50] in util header */
   if (iLength >= FILENAME_SIZE) {
      return 0;
   }

   /* Reject slash*/
   if (strstr(pszFileName, "/") !=NULL) {
      return 0;
   }

   /* Reject .. path traversial in filename */
   if (strstr(pszFileName, "..") !=NULL) {
      return 0;
   }

   /* Reject backslash*/
   if (strstr(pszFileName, "\\") !=NULL) {
      return 0;
   }

   return 1;
}

/***** Manage DATA command, check filename & reject with 501 ******************/

int HandleDataOrQuit(int iSocket, int *piDone) {
   int ReceiveStatus = 0;
   int ReplyStatus = 0;
   int TransferStatus = 0;
   struct EWA_EXAM25_TASK5_PROTOCOL_CLIENTDATACMD stDataCmd;
   char szFileName[FILENAME_SIZE];

   /* init, clear DATA struct and filename buffer*/
   memset(&stDataCmd, 0, sizeof(stDataCmd));

   memset(&szFileName, 0, sizeof(szFileName));

   /* get data CMD packet from EWA client */
   ReceiveStatus = recv(iSocket, &stDataCmd, sizeof(stDataCmd), 0); 

   puts("*** STATUS DATA CHECK ***");

   /* Validate that the full DATA packet was obtained*/
   if (ReceiveStatus != sizeof(stDataCmd)) {
      puts("ERROR: Failed to get the |DATA| packet.");
      return -1;
   }

   /* Check if EWA sent the DATA command*/
   if(memcmp(stDataCmd.acCommand, "DATA", 4) == 0) {
      
      /* Copy accepted filename for file-transfer*/
      strncpy(szFileName, stDataCmd.acFormattedString, sizeof(szFileName) -1);
      szFileName[sizeof(szFileName) -1] = '\0';
   
      /* Print provided file name*/
      printf("Recieved DATA filename: %s\n", szFileName);

      /* Validate filename reply with 501 if invalid*/
      if (IsValidFileName(szFileName) == 0) {
         ReplyStatus = SendReply(iSocket, "501", "Invalid filename. Provide valid filename 1-49");
         if (ReplyStatus < 0) {
            return -1;
         }
      
         return -1;
      }

      /* Send 354 and let EWA know the server is ready to recive data*/
      ReplyStatus = SendReply(iSocket, "354", "Ready for approved DATA file");
      if (ReplyStatus < 0) {
         return -1;
      }

      /* */
      TransferStatus = HandleFileTransfer(iSocket, szFileName);
      if (TransferStatus < 0) {
         return -1;
      }

      return 0;
   }

   /* Check if EWA sendt QUIT command*/    
   if (memcmp(stDataCmd.acCommand, "QUIT", 4) == 0) {

      /* Tell EWA server is closing*/
      ReplyStatus = SendReply(iSocket, "221", "127.0.0.1 closing connection");
      if (ReplyStatus < 0) {
         return -1;
      }

      /* Lets HandleClient() know the protocol should end*/  
      *piDone = 1;

      return 0;
   } 

   puts("ERROR: Expected DATA or QUIT command");
   return -1;
}

/***** RECIVE & SAVE DATA *****************************************************/

int HandleFileTransfer(int iSocket, char *pszFileName) {
   int ReceiveStatus = 0;
   int ReplyStatus = 0;
   int iDataSize = 0;
   int iTotalSize = 0;
   int iWriteSize = 0;
   struct EWA_EXAM25_TASK5_PROTOCOL_SIZEHEADER stHeader;
   char *pszFileContent = NULL;
   char *pszNewContent = NULL;
   char *pszMarker = NULL;
   FILE *fOutput = NULL;

   /* Alloc one byte to give realloc a starting pointer*/
   pszFileContent = malloc(1);
   if (pszFileContent == NULL) {
   puts("ERROR: Failed to allocate file buffer");
      return -1;
   }

   pszFileContent[0] = '\0'; /* zero terminates innital file buffer*/

   /* keeps receiving packets until end marker is found*/
   while (pszMarker == NULL) {

      /* Clears EWA dataheader prior to filling data*/
      memset(&stHeader, 0, sizeof(stHeader));

      /* gets 8 byte header for file content*/
      ReceiveStatus = recv(iSocket, &stHeader, sizeof(stHeader), 0);
      
      /* Checks that full data is recived*/
      if (ReceiveStatus != sizeof(stHeader)) {
         puts("ERROR: Failed to get file data header");
	 free(pszFileContent);
	 pszFileContent = NULL;
	 return -1;
      }

      iDataSize = GetHeaderDataSize(&stHeader);

      /* Max data size set up to 9998*/
      if (iDataSize <= 0 || iDataSize > MAX_DATA_SIZE) {
         puts("ERROR: Invalid file data size");
         free(pszFileContent);
         pszFileContent = NULL;
         return -1;
      }

      /* grows file content buffer with +1 for new data*/
      pszNewContent = realloc(pszFileContent, iTotalSize + iDataSize +1);
      if (pszNewContent == NULL) {
         puts("ERROR: Failed to grow the file buffer further");
         free(pszFileContent);
         pszFileContent = NULL;
         return -1;
      }
      
      /* Update file content after realloc*/
      pszFileContent = pszNewContent;
      /* Recive file content from EWA*/
      ReceiveStatus = recv(iSocket, pszFileContent + iTotalSize, iDataSize, 0);

      /* Validate that all announced file bytes were recived (64)*/
      if (ReceiveStatus !=iDataSize) {
         puts("ERROR: Failed to recive complete data packet");
         free(pszFileContent);
         pszFileContent = NULL;
         return -1;
      }
      /* Update total file size state*/
      iTotalSize += ReceiveStatus; 
      /* 0 terminates buffer so it can be searched safely*/
      pszFileContent[iTotalSize] = '\0'; 

      /* sends 250 for Data recieved*/
      ReplyStatus = SendReply(iSocket, "250", "Data recieved");
      if (ReplyStatus < 0) {
         free(pszFileContent);
         pszFileContent = NULL;
         return -1;
      }
     
      /* Searches through data for EOF marker (util header)*/ 
      pszMarker = strstr(pszFileContent, FILE_END_MARKER);
   }   
   
   iWriteSize = pszMarker - pszFileContent;
   /* create file with validated name supplied by EWA*/
   fOutput = fopen(pszFileName, "wb");
   if (fOutput == NULL) {
      puts("ERROR: Failed to create received file. Verify r/w permissions");
      free(pszFileContent);
      pszFileContent = NULL;
      return -1;
   }

   /* Write file content*/
   if (fwrite(pszFileContent, 1, iWriteSize, fOutput) != (size_t)iWriteSize) {
      puts("ERROR: Failed to write data to file. Verify r/w permissions");
      fclose(fOutput);
      free(pszFileContent);
      pszFileContent = NULL;
      return -1;
   }
 
   /* Close after successful write*/
   fclose(fOutput);

   /* print saved filename to verify*/
   printf("Saved received file: %s\n",pszFileName);

   /* free file buffer*/
   free(pszFileContent);
   pszFileContent = NULL;

   return 0;
}

/***** Handle one connected client ********************************************/
/* Passed over from program5 main handle client, iClientSocket > iSocket*/
int HandleClient(int iSocket, char *pszServerId) {
   int Status = 0;
   int Done = 0;
   enum SERVER_STATUS eStatus = EXPECT_HELO;

   /* Send innital server accept message */
   Status = SendAccept(iSocket, pszServerId);
   if (Status < 0) {
      return -1;
   }

   /* Get Helo and send greeting reply*/
   if (eStatus == EXPECT_HELO) {
      Status = HandleHelo(iSocket);
      if (Status < 0) {
         return -1;
      }

      eStatus = EXPECT_MAILFROM;
   }

   /*Get Mail from and send 250 reply*/
   if(eStatus == EXPECT_MAILFROM) {
      Status = HandleMailFrom(iSocket);
      if (Status < 0) {
         return -1;
      }      
   
      eStatus = EXPECT_RCPTTO;
   }

   /* Get RCPTO and send 250 back*/
   if (eStatus == EXPECT_RCPTTO) {
      Status = HandleRcptTo(iSocket);
      if (Status < 0) {
         return -1;
      }
   
      eStatus = EXPECT_DATA_OR_QUIT;
   }

   /* Get DATA commands until EWA sends QUIT*/
   while (Done == 0 && eStatus == EXPECT_DATA_OR_QUIT) {
      Status = HandleDataOrQuit(iSocket, &Done);
      if (Status < 0) {
         return -1;
      }

      /* Waits for another DATA message from EWA, until QUIT*/
      if (Done == 0) {
         eStatus = EXPECT_DATA_OR_QUIT;
      } else {
         eStatus = DONE;   
      }
   }
   
   puts("Succsessfuly got through the protocol."); 

   return 0;
}

/*--EOF-----------------------------------------------------------------------*/
