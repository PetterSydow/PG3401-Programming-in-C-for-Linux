/*TASK3_UTIL.H------------------------------------------------------------------

   	EXAM PG3401 
   	
   	Author: Candidate 262
   	Description: Utillity header for task 3, menu help & input handeling.


------------------------------------------------------------------------------*/

#ifndef TASK3_UTIL_H
#define TASK3_UTIL_H


/*******UTILITY METHODS *******************************************************/

/* Remove Newline from fgets read*/
void CutNewLine(char *pszString);

/* Read menu input form user*/
int GetMenuChoice(void);

/* Read one line from stdin into buffer*/
int ReadLine(char *pszBuffer, int iBufferSize);

/*Print main menu*/
void PrintMenu(void);

/* Pause program until [ENTER], EWA inspired */
void Pause(void);


#endif

/*----EOF---------------------------------------------------------------------*/
