/* TASK5_UTIL.C-------------------------------------------------------------

	PG3401 EXAM 2026 Spring

	Autor:  candidate 262   
	Description: Handle utility functions for task 5, time date, 
        input cmdln validation & later data.


------------------------------------------------------------------------------*/

/*** Includes *****************************************************************/
   
   /* standard C libs*/
   #include <stdio.h>
   #include <stdlib.h>
   #include <string.h>
   #include <ctype.h>
   #include <time.h>


   /* task spesific include files*/
   #include "ewpdef.h"
   #include "task5_util.h"

/*******************************************************************************
***** UTILITY METHODS
*******************************************************************************/

/*** Check if string contains only decimal digits *****************************/

int IsValidInteger(char *pszString) {
   int i = 0;

   /* Check that string pointer is useable*/
   if (pszString == NULL) {  
      return 0;
   }

   /* Check that the string is not empty */
   if (pszString[0] == '\0') {
      return 0;
   }

   /* Iterate through each character and reject non digits*/
   for (i = 0; pszString[i] != '\0'; i++) {
      if (!isdigit((unsigned char)pszString[i])) {
         return 0;
      }
   }

   return 1;
}


/*** Validate cmdln input for port and id *************************************/

int CheckCliInput(int iArgc, char *apszArgv[],
 int *piPort, char *pszServerId, int iIdSize) {
   int CompareResult = 0;
   int iPort = 0;
   int ValidNumber = 0;

   /* validate number of cmdln arguments*/
   if (iArgc != 5) {
      puts("ERROR: Use ./program5 - port <port> -id <serverid>");
      return -1;
   }

   /* Validate first cmdln flag*/
   CompareResult = strcmp(apszArgv[1], "-port");
   if (CompareResult != 0) {
      puts("ERROR: First argument must be -port."); 
      return -1;
   }

   /* Check that port text is numeric*/
   ValidNumber = IsValidInteger(apszArgv[2]); /* omg*/
   if (ValidNumber == 0) {
      puts("ERROR: Port must be a valid number."); /*use 1024< port numbers?*/
      return -1;
   }

   /* Convert validated port from text to integer*/
   iPort = atoi(apszArgv[2]);

   /* Check port range*/
   if (iPort < 1 || iPort > 65535) {
      puts("ERROR: Port must be between 1 and 65535");
      return -1;
   }

   /* Validate second cmdln flag*/ 
   CompareResult = strcmp(apszArgv[3], "-id");
   if (CompareResult != 0) {
      puts("ERROR: third cmdln arg must be -id.");
      return -1;
   }

   /* check buffer w Id length*/
   if ((int)strlen(apszArgv[4]) >= iIdSize) {
      puts("ERROR: Server Id is too long.");
      return -1;
   }

   /* saves validated input*/
   *piPort = iPort;

   /* copy input safely*/
   strncpy(pszServerId, apszArgv[4], iIdSize -1);
   pszServerId[iIdSize -1] = '\0';

   return 0;
}


/*** Fill common EWA size header **********************************************/

void FillSizeHeader(struct EWA_EXAM25_TASK5_PROTOCOL_SIZEHEADER *pHead,
 const char *pszSize) {

   /* With EWA protocol magic number */
   memcpy(pHead->acMagicNumber, EWA_EXAM25_TASK5_PROTOCOL_MAGIC, 3);

   /* With ascii size string; 0064*/
   memcpy(pHead->acDataSize, pszSize, 4);

   /* With set delimiter protocol*/
   pHead->acDelimeter[0] = '|';
}

/*** Converts EWA size header to integer **************************************/

int GetHeaderDataSize(struct EWA_EXAM25_TASK5_PROTOCOL_SIZEHEADER *pHead) {
   char szSize[5];
   int iSize = 0;
   
   /* init and clear buffer*/
   memset(szSize, 0, sizeof(szSize));
   /* copies 4 byte ascii*/
   memcpy(szSize, pHead->acDataSize, 4);
   /* adding 0 termination*/
   szSize[4] = '\0';

   /* converts ascii to integer*/
   iSize = atoi(szSize);

   return iSize;
}

/**** Create formated server accept string ************************************/

int CreateAcceptString(char *pszBuffer, char *pszServerId) {
   time_t tNow;
   struct tm *pTimeInfo =NULL;
   char szTime[32];

   /* Validate input pointers*/
   if (pszBuffer == NULL || pszServerId == NULL) {
      return -1;
   }

   /* Clear local time buffer prior to use*/
   memset(szTime, 0, sizeof(szTime));
   /* Get current system time & convert to local time*/
   tNow = time(NULL);
   pTimeInfo = localtime(&tNow);

   if (pTimeInfo != NULL) {
      strftime(szTime, sizeof(szTime), "%Y-%m-%d %H:%M:%S", pTimeInfo);
   } else {
      strcpy(szTime, "time error");
   }

   /* accept string*/
   strcpy(pszBuffer, SERVER_IP);
   strcat(pszBuffer, " ");
   strcat(pszBuffer, PROTOCOL_NAME);
   strcat(pszBuffer, " ");
   strcat(pszBuffer, pszServerId);
   strcat(pszBuffer, " ");
   strcat(pszBuffer, szTime);

   return 0;
}

/*--EOF-----------------------------------------------------------------------*/
