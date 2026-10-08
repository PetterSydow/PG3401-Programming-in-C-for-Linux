/*TASK3_EXAMLIST.H--------------------------------------------------------------

   	EXAM PG3401 Task 3
   	
   	Author: Candidate 262
   	Description: Header for task 3, contains the task defines, the candidate
	             struct. And prototypes for list handeling methods, grading,
                     printing & rapports.


------------------------------------------------------------------------------*/

#ifndef TASK3_EXAMLIST_H
#define TASK3_EXAMLIST_H

/**** DEFINES *****************************************************************/
/* Max points 100*/
#define TASK1_MAX_POINTS 5
#define TASK2_MAX_POINTS 15
#define TASK3_MAX_POINTS 20
#define TASK4_MAX_POINTS 20
#define TASK5_MAX_POINTS 20
#define TASK6_MAX_POINTS 20


#define TASK_COUNT 6
#define CANDIDATE_ID_SIZE 16
#define JUSTIFICATION_SIZE 128

/**** STRUCTS  ****************************************************************/

struct EXAM_CANDIDATE {
   struct EXAM_CANDIDATE *pPrev;
   struct EXAM_CANDIDATE *pNext;
   
   char szCandidateId[CANDIDATE_ID_SIZE];

   char aTaskJustification[TASK_COUNT][JUSTIFICATION_SIZE];

   int aTaskPoints[TASK_COUNT];
   int aTaskGraded[TASK_COUNT]; 
   int iFinalScore;
};

/******************************************************************************/
/**** List Methods ************************************************************/

/* Add A candidate to end of LL*/
int AddCandidate(struct EXAM_CANDIDATE **ppHead,
		 struct EXAM_CANDIDATE **ppTail,
		 char *pszCandidateId);

/* Reading in a List of candidates*/
int AddCandidatesFromFile(struct EXAM_CANDIDATE **pphead,
 struct EXAM_CANDIDATE **ppTail, char *pszFileName);

/* Find candidate by cand ID*/
struct EXAM_CANDIDATE *FindCandidate(struct EXAM_CANDIDATE *pHead, 
				     char *pszCandidateId);

/* Free candidate nodes on exit (todo write to list)*/
void FreeCandidateList(struct EXAM_CANDIDATE **ppHead,
		       struct EXAM_CANDIDATE **ppTail);


/******************* Grading & Scores *****************************************/

int GetTaskMaxPoints(int iTaskNumber);

void CalculateFinalScore(struct EXAM_CANDIDATE *pCandidate);

char CalculateGrade(int iFinalScore);


/* Assesses and set points, justification for one task*/
int SetTaskAssessment(struct EXAM_CANDIDATE *pHead, char *pszCandidateId, 
int iTaskNumber, int iPoints, char *pszJustification);

/* check is all tasks are graded for a given a candidate*/
int IsCandidateFullyGraded(struct EXAM_CANDIDATE *pCandidate);

/******************* Printing results *****************************************/

/* Print all candidates currently added to graded list*/
void PrintAllCandidates(struct EXAM_CANDIDATE *pHead);

/* all cand graded*/
void PrintAllCandidatesGraded(struct EXAM_CANDIDATE *pHead);

/* all cand matching provided A-F grade*/
void PrintCandidatesByGrade(struct EXAM_CANDIDATE *pHead, char cSuppliedGrade);

/* all cand not fully graded yet*/
void PrintNotFullyGraded(struct EXAM_CANDIDATE *pHead);

/* Print detailed rapport for 1 candidate*/
void PrintCandidateRapport(struct EXAM_CANDIDATE *pHead, char *pszCandidateId);   


#endif

/*----EOF---------------------------------------------------------------------*/
