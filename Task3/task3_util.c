/*TASK3_UTIL.C-----------------------------------------------------------------/


   	EXAM PG3401 Task 3
   	
   	Author: Candidate 262
   	Description: Input and menu help functions. Handels pausing, 
        menu printing, reading user input & cutting newlines.



------------------------------------------------------------------------------*/

/**** Includes ****************************************************************/

/**** Standard libs*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**** Task spesific*/
#include "task3_util.h"

/*******************************************************************************
*********** METHODS ***********************************************************/
 
/**** Print MENU **************************************************************/
/* ║ sidebars I stole from EWA */
void PrintMenu(void) {
   puts("\n============== INTELIGRADER ===================");
   puts("║ First voice activated exam grader!            ║");
   puts("║ Voice module failed... (use keys)             ║");
   puts("║                                               ║");
   puts("║ 1. Add a candidate manually                   ║");
   puts("║ 2. Add candidates from .txt  file             ║");
   puts("║ 3. Set score and justification                ║");
   puts("║ 4. Print all candidates with score and grade  ║");
   puts("║ 5. Print full candidate ID list               ║");
   puts("║ 6. Print candidates by [A-F] grade            ║");
   puts("║ 7. Get candidates not yet fully graded        ║");
   puts("║ 8. Get detailed candidate rapport with cand # ║");
   puts("║ 9. Exit INTELIGRADER                          ║");
   puts("=================================================");
   printf("Chose an option: ");
}

/**** Get menu choice from user************************************************/
int GetMenuChoice(void) {
   char szBuffer[16];
   int iChoice = 0;

   /* init input buffer*/
   memset(szBuffer, 0, sizeof(szBuffer));

   /* read menu choice*/
   if (ReadLine(szBuffer, sizeof(szBuffer)) < 0) {
      return 0;
   }

   /* change input to integer & return*/    
   iChoice = atoi(szBuffer);

   return iChoice;
}

/**** Pause (like EWA, I think)************************************************/
void Pause(void) {
   char szBuffer[8];

   /*init buffer and wait for enter. Can i get it into a menu looking box*/
   memset(szBuffer, 0, sizeof(szBuffer));
   puts("\nPress [ENTER] to continue");
   fgets(szBuffer, sizeof(szBuffer), stdin);
}

/**** Remove Newline from fgets read ******************************************/
void CutNewLine(char *pszString) {
   int iLength = 0;

   /* check string pointer*/   
   if (pszString == NULL) {
      return;
   }

   /* Removes new line from fgets*/
   iLength = strlen(pszString);
   if (iLength > 0 && pszString[iLength -1] == '\n') {
      pszString[iLength -1] = '\0';
   }
}

/**** Read line from stdin ****************************************************/
int ReadLine(char *pszBuffer, int iBufferSize) {
   char *pszResult = NULL;

   /* check input buffer*/
   if (pszBuffer == NULL || iBufferSize <= 0) {
      return -1;
   }

   /* read user input*/
   pszResult = fgets(pszBuffer, iBufferSize, stdin);
   if (pszResult == NULL) {
      return -1;
   }
   
   CutNewLine(pszBuffer); /* Removes newsline form fgets*/
   
   return 0;
}

/*----EOF---------------------------------------------------------------------*/
