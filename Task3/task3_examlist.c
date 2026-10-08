/*TASK3_EXAMLIST.C-------------------------------------------------------------/


   	EXAM PG3401 Task 3
   	
   	Author: Candidate 262
   	Description: Takes care of the doubble linked list of the INTELLIGRADER
	             Adds candidates, imports candidates from file, stores task
		     scores, grades & justification, prints rapports and 
                     frees the list.
                     

------------------------------------------------------------------------------*/

/**** Includes ****************************************************************/

/**** Standard libs*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


/**** Task spesific*/
#include "task3_examlist.h" 

/*******************************************************************************
********LIST METHODS **********************************************************/


/**** Find candidate by cand ID ***********************************************/
struct EXAM_CANDIDATE *FindCandidate(struct EXAM_CANDIDATE *pHead, 
				     char *pszCandidateId) {
   struct EXAM_CANDIDATE *pCurrent = NULL;
   /* search from head | what if not found*/
   pCurrent = pHead;
   
   while(pCurrent != NULL) {
      if(strcmp(pCurrent->szCandidateId, pszCandidateId) == 0) {
         return pCurrent;
      }
      
      pCurrent = pCurrent->pNext;
   }

   return NULL;
}

/**** Prepare to grade tasks **************************************************/
int GetTaskMaxPoints(int iTaskNumber) {
   int aMaxPoints[TASK_COUNT] = {
      TASK1_MAX_POINTS,
      TASK2_MAX_POINTS,
      TASK3_MAX_POINTS,
      TASK4_MAX_POINTS,
      TASK5_MAX_POINTS,
      TASK6_MAX_POINTS
   };

   /* checks if the # of tasks is correct*/
   if(iTaskNumber <1 || iTaskNumber > TASK_COUNT) {
      return -1;
   }
   
   /* make an array for the defined task number in examlist header, 0-5 : 6 */
   return aMaxPoints[iTaskNumber -1];
}

/**** Calculate final score  **************************************************/
void CalculateFinalScore(struct EXAM_CANDIDATE *pCandidate) {
   int i = 0;
   int iScore = 0;

  /* check pointer*/
  if (pCandidate == NULL) {
     return;
  }

  /* sum task points*/
  for (i = 0; i < TASK_COUNT; i++) {
     iScore += pCandidate->aTaskPoints[i];
  }

  /* store final score*/
  pCandidate->iFinalScore = iScore;
}

/**** Convert points to A-F grades ********************************************/
/* Max points 100*/
char CalculateGrade(int iFinalScore) {
   if (iFinalScore >= 80) { return 'A'; }

   if (iFinalScore >= 70) { return 'B'; }

   if (iFinalScore >= 60) { return 'C'; }

   if (iFinalScore >= 50) { return 'D'; }

   if (iFinalScore >= 40) { return 'E'; }

   return 'F'; 
}

/**** Task assessment for one cand ********************************************/
int SetTaskAssessment(struct EXAM_CANDIDATE *pHead, char *pszCandidateId, 
int iTaskNumber, int iPoints, char *pszJustification) {
   /* init */
   int iTaskIndex = 0;
   int iMaxPoints = 0;
   struct EXAM_CANDIDATE *pCandidate = NULL;

   /* check pointers*/
   if (pszCandidateId == NULL || pszJustification == NULL) {
      return -1;
   }
   
   /* Gets max point values for tasks & checks that task number is 1-6*/
   iMaxPoints = GetTaskMaxPoints(iTaskNumber);
   if (iMaxPoints < 0) {
      puts("ERROR: Wrong task number. Use 1-6");
      return -1;
   }

   /* Check if points are within accepted range*/
   if (iPoints < 0 || iPoints > iMaxPoints) {
      printf("ERROR: points for task %d must be inbetween 0 and %d \n",
	 iTaskNumber, iMaxPoints);
      return -1;
   }      

   /* Find Candidate in LL - cand not in LL*/
   pCandidate = FindCandidate(pHead, pszCandidateId);
   if(pCandidate == NULL) {
      puts("ERROR: Candidate not in list, verify number or enter candidate");
      return -1;
   }
   
   iTaskIndex = iTaskNumber -1; /* Change task number 1-6 to array index 0-5*/
   
   /* Store points, grade & justification*/  
   pCandidate->aTaskPoints[iTaskIndex] = iPoints;
   pCandidate->aTaskGraded[iTaskIndex] = 1;   /* XYXX marks the spot*/
   
   strncpy(pCandidate->aTaskJustification[iTaskIndex], 
	pszJustification,JUSTIFICATION_SIZE -1);
   pCandidate->aTaskJustification[iTaskIndex][JUSTIFICATION_SIZE -1] = '\0'; 

   /* Calculate final score*/
   CalculateFinalScore(pCandidate);

   return 0;
}

/**** Prints all graded candidates *********************************************/
void PrintAllCandidatesGraded(struct EXAM_CANDIDATE *pHead) {
   struct EXAM_CANDIDATE *pCurrent = NULL;
   char cGrade = 'F';

   /* check if list is empty*/
   if (pHead == NULL) {
      puts("No stored candidates found. Try loading candidates in from file");  
      return;
   }

   puts("\nAll candidates:");
   puts("==========================================");
   puts("Candidate ID			Score	Grade");
   puts("=========================================="); /* Look adjust*/

   pCurrent = pHead;  /* Going through the graded list and print score & grade*/
   while (pCurrent != NULL) {
      cGrade = CalculateGrade(pCurrent->iFinalScore);

      printf("%-18s %-8d %c\n", pCurrent->szCandidateId, pCurrent->iFinalScore,
				 cGrade);

      pCurrent = pCurrent->pNext;
   }
   
   puts("==========================================");
}

/**** checks if all tasks are graded for a given a candidate*******************/
int IsCandidateFullyGraded(struct EXAM_CANDIDATE *pCandidate) {
   int i = 0;

   /* check cand pointer*/
   if (pCandidate == NULL) {
      return 0;
   }
   /* any task is missing, cand is not fully graded*/
   for (i = 0; i < TASK_COUNT; i++) {
      if(pCandidate->aTaskGraded[i] == 0) {
         return 0;
      }
   }

   return 1;
}

/**** Print cand matching provided A-F grade***********************************/
void PrintCandidatesByGrade(struct EXAM_CANDIDATE *pHead, char cSuppliedGrade) {
   struct EXAM_CANDIDATE *pCurrent = NULL;
   char cGrade = 'F';
   int iFound = 0; 

   if (pHead == NULL) {
      puts("No valid candidates in list"); /* checks if list is empty*/
      return;
   }

   puts("\nCandidates with the supplied grade");
   puts("==========================================");
   puts("Candidate ID			Score	Grade");
   puts("==========================================");

   /* Go through list and print candidates with matching grade to input*/
   pCurrent = pHead;
   while (pCurrent != NULL) {
      cGrade = CalculateGrade(pCurrent->iFinalScore);

      if (cGrade == cSuppliedGrade) {
         printf("%-18s %-8d %c\n", 
	    pCurrent->szCandidateId,
            pCurrent->iFinalScore, cGrade);
         iFound = 1;
      }
           
      pCurrent = pCurrent->pNext;
   }

   /* Case no candidates of requested grade*/    
   if (iFound == 0) {
      puts("No candidates of that grade was found");
   }

   puts("==========================================");
}

/**** Prints candidates not fully graded **************************************/
void PrintNotFullyGraded(struct EXAM_CANDIDATE *pHead) {
   struct EXAM_CANDIDATE *pCurrent = NULL;
   int iFound = 0;

   /* Check if list is empty*/
   if (pHead == NULL) {
      puts("No candidates in the list");
      return;
   }

   puts("\nCandidates not yet fully graded: ");
   puts("==========================================");

   /* Go through the list and print cand lacking fully graded status*/
   pCurrent = pHead;
   while (pCurrent !=NULL) {
      if(IsCandidateFullyGraded(pCurrent) == 0) {
         printf("Candidate ID: %s | Score: %d | Grade: %c\n",
	    pCurrent->szCandidateId,
            pCurrent->iFinalScore,
            CalculateGrade(pCurrent->iFinalScore));
         iFound = 1;
      }

      pCurrent = pCurrent->pNext;
   }
   
   /* Case all candidates are fully graded*/
   if (iFound == 0) {
      puts("All candidates found where graded");
   }

   puts("==========================================");   
}

/**** Print detailed rapport for 1 candidate***********************************/
void PrintCandidateRapport(struct EXAM_CANDIDATE *pHead, char *pszCandidateId) {
   struct EXAM_CANDIDATE *pCandidate = NULL;
   int i = 0;

   /*Check pointer*/
   if (pszCandidateId == NULL) {
      puts("ERROR: Invalid candidate ID");
      return;
   }

   /* Find input cand in list*/
   pCandidate = FindCandidate(pHead, pszCandidateId);
   if (pCandidate == NULL) {
      puts("EERROR: That candidate is not in the list.");
      return;
   }

   puts("\nCandidate rapport: ");
   puts("==========================================");
   printf("Candidate ID: %s\n", pCandidate->szCandidateId);
   printf("Final score : %d\n", pCandidate->iFinalScore);
   printf("Grade       : %c\n", CalculateGrade(pCandidate->iFinalScore));
   puts("==========================================");

   /* for all tasks print score and justification*/
   for (i = 0; i < TASK_COUNT; i++) {
      printf("Task %d:\n", i + 1);
      
      if (pCandidate->aTaskGraded[i] == 1) {
         printf("  Points       : %d / %d\n",
            pCandidate->aTaskPoints[i],
            GetTaskMaxPoints(i + 1));
         printf("  Justification: %s\n", pCandidate->aTaskJustification[i]);
      } else {
         puts("  Not graded yet");
      }
   }

   puts("==========================================");
}   

/**** ADD a Candidate (by tail of LL) *****************************************/
int AddCandidate(struct EXAM_CANDIDATE **ppHead,
		 struct EXAM_CANDIDATE **ppTail,
		 char *pszCandidateId) {
   struct EXAM_CANDIDATE *pNewCandidate = NULL;
   struct EXAM_CANDIDATE *pFound = NULL;

   if (ppHead == NULL || ppTail == NULL || pszCandidateId == NULL) {
      return -1;
   }
   
   /* duplicate check*/
   pFound = FindCandidate(*ppHead, pszCandidateId);
   if (pFound != NULL) {
      puts("ERROR: Candidate ID is already taken");   
      return -1;
   }

   /* allocate memory for a new candidate*/
   pNewCandidate = (struct EXAM_CANDIDATE *)malloc(sizeof(struct EXAM_CANDIDATE));
   if (pNewCandidate == NULL) {
      puts("ERROR: Failed to add new candidate");
      return -1;
   } 
   
   /* clear node prior to filling in fields*/
   memset(pNewCandidate, 0, sizeof(struct EXAM_CANDIDATE));
   
   /* copies new candidate into candidate id list*/
   strncpy(pNewCandidate->szCandidateId, pszCandidateId, CANDIDATE_ID_SIZE -1);
   pNewCandidate->szCandidateId[CANDIDATE_ID_SIZE -1] = '\0';

   /* List empty case*/
   if (*ppHead == NULL) {
      *ppHead = pNewCandidate;
      *ppTail = pNewCandidate;
      return 0;
   }
 
   /* Otherwise add node after tail*/
   pNewCandidate->pPrev = *ppTail;
   if (*ppTail !=NULL) {
      (*ppTail)->pNext = pNewCandidate;
   }
   
   /* move pointer to new last node*/
   *ppTail = pNewCandidate;

   return 0;
}

/**** Add Candidates from file ************************************************/
int AddCandidatesFromFile(struct EXAM_CANDIDATE **ppHead, 
	struct EXAM_CANDIDATE **ppTail, char *pszFileName) {

   FILE *fInput = NULL;
   char szCandidateId[CANDIDATE_ID_SIZE];
   int Status = 0;
   int iAdded = 0;
   int iSkipped = 0;
   int iLength = 0;

   /* check pointers*/
   if (ppHead == NULL || ppTail == NULL || pszFileName == NULL) {
      return -1;
   }

   fInput = fopen(pszFileName, "r");
   if (fInput == NULL) {
      puts("ERROR Could not read file. Verify permissions and file location");
      return -1;
   }

   while (fgets(szCandidateId, sizeof(szCandidateId), fInput) != NULL) {

      /* sets empty & newline to \0*/
      iLength = strlen(szCandidateId);
      if (iLength > 0 && szCandidateId[iLength -1] == '\n') {
         szCandidateId[iLength -1] = '\0';
      }  

      /* then skips \0*/
      if (szCandidateId[0] == '\0') {
         iSkipped++;
         continue;
      }

      /* Adds candidate to LL from file*/
      Status = AddCandidate(ppHead, ppTail, szCandidateId);
      if (Status == 0) {
         iAdded++;
      } else {
         iSkipped++;
      }

      /* Clear buffer after reading each line*/
      memset(szCandidateId, 0, sizeof(szCandidateId));
   }

   fclose(fInput);  /* Close candidate file*/

   /*print imported candidates*/
   printf("Candidates added from file: %d\n", iAdded);

   return 0;
}

/**** Print all candidates ****************************************************/
void PrintAllCandidates(struct EXAM_CANDIDATE *pHead) {
   struct EXAM_CANDIDATE *pCurrent = NULL;

   /* check if list is empty*/
   if (pHead == NULL) {
      puts("List is empty. No candidates present.");
      return;
   }

   puts("\nCandidate List");                       
   puts("==========================================");

   /* Go through list and print cand IDs*/
   pCurrent = pHead;
   while (pCurrent != NULL) {
      printf("Candidate ID: %s | Final Score: %d\n", pCurrent->szCandidateId,
	 pCurrent->iFinalScore);
      pCurrent = pCurrent->pNext;
   }

   puts("==========================================");
}

/**** Remove (free) all candidates ********************************************/
void FreeCandidateList(struct EXAM_CANDIDATE **ppHead,
		       struct EXAM_CANDIDATE **ppTail) {
   struct EXAM_CANDIDATE *pCurrent = NULL;
   struct EXAM_CANDIDATE *pTemp = NULL;

   /* check pointers*/
   if (ppHead == NULL || ppTail == NULL) {
      return;
   }

   /* Go through list and free nodes*/
   pCurrent = *ppHead;
   while (pCurrent != NULL) {
      pTemp = pCurrent;
      pCurrent = pCurrent->pNext;
      free(pTemp);
   }

   /*clear list pointers after free*/
   *ppHead = NULL;
   *ppTail = NULL;
}

/*----EOF---------------------------------------------------------------------*/
