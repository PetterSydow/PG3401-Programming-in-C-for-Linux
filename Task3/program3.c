/*PROGRAM3.c--------------------------------------------------------------------

   	EXAM PG3401 Task 3
   	Name: "INTELIGrader" (VOICE ACTIVATED ELEVATOR!)
   	Author: Candidate 262
   	Description: Main function for task 3. Displays menu, receives user 
        input, calls list/grading functions based on these, frees up list 
        memory befoure exit.
        
        Future Idea: Possibly writes list memory to a file prior to exit.
        Reads this file in again on startup. Adds persistence.    


------------------------------------------------------------------------------*/

/**** Includes ****************************************************************/

/**** Standard libs*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**** Task spesific*/
#include "task3_examlist.h" 
#include "task3_util.h"


/**** STRUCTS *****************************************************************/

/*******************************************************************************
**** MAIN
*******************************************************************************/

int main(void){
   int iChoice = 0;   /* menu options*/
   int iGrading = 1;  /* menu active while =1*/
   int Status = 0;
   int iTaskNumber = 0;
   int iPoints = 0;
   
   char szGrade[8];
   char szFileName[128];
   char szJustification[JUSTIFICATION_SIZE]; 
   char szCandidateId[CANDIDATE_ID_SIZE];

   struct EXAM_CANDIDATE *pHead = NULL;
   struct EXAM_CANDIDATE *pTail = NULL;

   puts("*** GRADING PROGRAM INNITALIZING ***");

   /* Main loop with switch. 9 cases*/

   while (iGrading == 1) {

      PrintMenu();
      iChoice = GetMenuChoice();

      switch (iChoice) {    

         case 1:
              /* add candidate*/
              memset(szCandidateId, 0, sizeof(szCandidateId));

              printf("Input candidate ID: ");
              Status = ReadLine(szCandidateId, sizeof(szCandidateId));
              if (Status < 0 || szCandidateId[0] == '\0') {
	        puts("ERROR: Invalid cand ID. Use format 10042-10001");
                Pause();
                break;
              }
              
              Status = AddCandidate(&pHead, &pTail, szCandidateId);
	      if (Status == 0) {
                 puts("Candidate added");
              }

              Pause();
              break;

         case 2:
              /* Add candidates from file*/
              memset(szFileName, 0,  sizeof(szFileName));

              printf("Enter candidate filename: ");
              Status = ReadLine(szFileName, sizeof(szFileName));
              if (Status < 0 || szFileName[0] == '\0') {
	         puts("ERROR: Invalid filename used");
                 Pause();
                 break;
              }

              Status = AddCandidatesFromFile(&pHead, &pTail, szFileName);
	      if (Status == 0) {
	         puts("File import success!");
              }

              Pause();
              break;   

         case 3:
              /* set task score & justification*/
              memset(szCandidateId, 0, sizeof(szCandidateId));

              printf("Enter candidate ID: ");
	      Status = ReadLine(szCandidateId, sizeof(szCandidateId));
 	      if (Status < 0 || szCandidateId[0] == '\0') {
	         puts("ERROR: Invalid "); 
                 Pause();
                 break;
	      }
              
              /* Loop to make this process less tedious*/
              iTaskNumber = -1;
	      while(iTaskNumber != 0) {
	         puts("\nChoose task to grade for this candidate");
		 puts("1. Task 1");
		 puts("2. Task 2");
		 puts("3. Task 3");
		 puts("4. Task 4");
		 puts("5. Task 5");
		 puts("6. Task 6");
		 puts("0. Done grading this candidate");
		 printf("Task number: ");

	         iTaskNumber = GetMenuChoice();
        
                 if (iTaskNumber == 0) {
                    puts("Returning to menu");
                    break;
                 }

                 if (iTaskNumber < 1 || iTaskNumber > TASK_COUNT) {
                    puts("ERROR: Task number must be 1-6");
                    continue;
                 }
                  
                 memset(szJustification, 0, sizeof(szJustification));
                  
 	         printf("Enter points for task %d: ", iTaskNumber);
                 iPoints = GetMenuChoice();
 
	         printf("Enter short justification: ");
	         Status = ReadLine(szJustification, sizeof(szJustification));
	         if (Status < 0 || szJustification[0] == '\0') {
	            puts("ERROR: Invalid justification");
		    continue;
                 }

                 Status = SetTaskAssessment(pHead, szCandidateId, iTaskNumber, 
		    iPoints, szJustification);

                 if (Status == 0) {
	            puts("Task assessment set");
	         }
              }

              Pause();
	      break;
	
         case 4:
              /* print all candidates that are graded*/
              PrintAllCandidatesGraded(pHead);
              Pause();
              break;
       
         case 5: 
              /* print all current candidates*/
              PrintAllCandidates(pHead);
              Pause();
              break;

         case 6: 
              /* print candidates by any A-F inputted grade*/
              memset(szGrade, 0, sizeof(szGrade));
              printf("Enter Grade A-F: ");

              Status = ReadLine(szGrade, sizeof(szGrade));
	      if (Status < 0 || szGrade[0] == '\0') {
                 puts("ERROR: Invalid grade");
                 Pause();
                 break;
              }

              /* account for lowercase. Convert with -32 & validate*/
              if (szGrade[0] >= 'a'&& szGrade[0] <= 'f') {
                 szGrade[0] = szGrade[0] - 32;
              }
                           
              if (szGrade[0] < 'A' || szGrade[0] > 'F') {
                 puts("ERROR: Grade must be A-F.");
                 Pause();
                 break;
              }

              PrintCandidatesByGrade(pHead, szGrade[0]);
	      Pause();
              break;   

         case 7: 
              /* print cand where not all tasks are fully graded*/
              PrintNotFullyGraded(pHead);
              Pause();
              break;

         case 8: 
              /* print detailed rapport for one candidate*/
              memset(szCandidateId, 0, sizeof(szCandidateId));
	      
              printf("Enter candidate ID: ");

              Status = ReadLine(szCandidateId, sizeof(szCandidateId));
              if (Status < 0 || szCandidateId[0] == '\0') {
                 puts("ERROR: Invalid candidate ID. Verify candidate number");
                 Pause();
                 break;
              }
              
              PrintCandidateRapport(pHead, szCandidateId);
              Pause(); 
              break;

         case 9: 
            /* Exit menu loop*/
            iGrading = 0;
            break;            

         default:
            /* wrong option selected*/
            puts("Wrong menu choice");
            Pause();
            break;
      }  
   }

   /* free all nodes, pointer head-tail*/ 
   FreeCandidateList(&pHead, &pTail);

   puts("***INTELIGrader completed succsessfully***");

   return 0;
}

/*----EOF---------------------------------------------------------------------*/
