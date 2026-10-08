/* TASK6_UTIL.C-------------------------------------------------------------

	PG3401 EXAM 2026 Spring

	Autor:  candidate 262   
	Description: Handle utility functions for task 5, time date, 
        input cmdln validation & later data. In task 6: here validates cmdln 
        input (from the user), checks the port number, and extracts the EWA 
        server address and port used by the client.


------------------------------------------------------------------------------*/

/*** Includes *****************************************************************/
   
   /* standard C libs*/
   #include <stdio.h>
   #include <stdlib.h>
   #include <string.h>
   #include <ctype.h>

   /* task spesific include files*/
   #include "task6_util.h"

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

   /* Check that the string is not empty*/
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

int CheckCliInput(int iArgc,
 	char *apszArgv[],
 	char *pszServerAddress,
 	int iServerAddressSize,
 	int *piPort) {
   int CompareResult = 0;
   int ValidNumber = 0;
   int iPort = 0;

   /* validate number of cmdln arguments*/
   if (iArgc != 5) {
      puts("ERROR: Use ./program6 -server <ipaddress> -port <port>");
      return -1;
   }

   /* Validate first cmdln flag*/
   CompareResult = strcmp(apszArgv[1], "-server");
   if (CompareResult != 0) {
      puts("ERROR: First argument must be -server"); 
      return -1;
   }
   
   /* Check server addr length befoure copying*/
   if ((int)strlen(apszArgv[2]) >= iServerAddressSize) {
      puts("ERROR: Server Address size is too long.");
      return -1;
   }

   /* Validate second cmdln flag*/ 
   CompareResult = strcmp(apszArgv[3], "-port");
   if (CompareResult != 0) {
      puts("ERROR: second cmdln arg must be -port.");
      return -1;
   }
   /* Check that port text is numeric*/
   ValidNumber = IsValidInteger(apszArgv[4]);
   if (ValidNumber == 0) {
      puts("ERROR: Port must be a valid number.");
      return -1;
   }

   /* Convert validated port from text to integer*/
   iPort = atoi(apszArgv[4]);

   /* Checks port range*/
   if (iPort < 1 || iPort > 65535) {
      puts("ERROR: Port must be between 1 and 65535");
      return -1;
   }

      /* copy input safely*/
   strncpy(pszServerAddress, apszArgv[2], iServerAddressSize -1);
   pszServerAddress[iServerAddressSize -1] = '\0';

   /* saves validated input*/
   *piPort = iPort;


   return 0;
}

/*--EOF-----------------------------------------------------------------------*/
