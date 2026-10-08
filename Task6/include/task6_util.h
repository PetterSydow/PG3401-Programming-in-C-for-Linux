/* TASK6_UTIL.H----------------------------------------------------------------

	PG3401 EXAM 2026 Spring

	Author:   candidate 262   
	Description: Header for utility functions in task 6
	
------------------------------------------------------------------------------*/

#ifndef TASK6_UTIL_H
#define TASK6_UTIL_H


#define SERVER_ADDRESS_SIZE 64

int IsValidInteger(char *pszString);

int CheckCliInput(int iArgc,
 char *apszArgv[],
 char *pszServerAddress,
 int iServerAddressSize, 
int *piPort);


#endif

/*--EOF-----------------------------------------------------------------------*/
