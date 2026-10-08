/* This file has been created by EWA, and is part of task 4 on the exam for PG3401 2026*/
/* MAIN.C file -----------------------------------------------------------------

   Role:	Task 4 part 1 main.c program file
   Version:	1.0 "The Earthquake"
   Author: 	EWA 
   Alterations: Candidate 262
   
   Description: Fixed issues, removed partial boolean true/false logic, removed 
   global-variables - by using a new struct for this "threaddata".  
   Added mutexes with explicit initilzation, to protect shared pointer access of
   file read and node spawning.
   Changed code from hardcoded input to take cmdln program+<filename> input.

------------------------------------------------------------------------------*/

/*** Standard C libraries *****************************************************/
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* Seems all good*/ 
#include <unistd.h> /* Linux os related, not strictly standard, 
                       but supported c89, should compile */

/*** Task spesific includes ***************************************************/

#include "task4_prime.h"

/*** Task spesific data & thread function *************************************/ 

/* use typedef? _SHARED if time maybe, PGPR... hard to spell right*/
struct PGPRIMENUMBER {
   unsigned int uiPrime;
   struct PGPRIMENUMBER *next;
};

/* Shared thread data passed from main to both worker threads,
   replacing the original global file & list pointer*/
struct THREADDATA {
   FILE *fInputFile;
   struct PGPRIMENUMBER *pPrimeList;
   pthread_mutex_t fileMutex;
   pthread_mutex_t listMutex;
};

   void* threadFunction(void* arg) {
      struct THREADDATA *pData = NULL;
      unsigned int auiNumbers[10] = {0};
      int iNumbersRead = 0;
      int iIndex = 0;
      struct PGPRIMENUMBER *newNode = NULL;

      pData = (struct THREADDATA *)arg;

      while (1) { /* Will not try #define true/false = 1/0 just change to 1/0*/
         iNumbersRead = 0;

      /* Mutex lock placed due to shared file reader pointer (from fscanf) for 
         the threads. Without this they could both end up trying to use this at 
	 the same time: could lead to missed, duplicated or corrupted reads.*/
      pthread_mutex_lock(&(pData->fileMutex));
      while (iNumbersRead < 10) {
         if (fscanf(pData->fInputFile, "%u", &(auiNumbers[iNumbersRead])) == 1) {
            iNumbersRead++; /* Set %u unsigned here as data was unsigned int*/
         }
         else {
            break;
         }
      }
      pthread_mutex_unlock(&(pData->fileMutex));

      if (iNumbersRead == 0) {
         break;
      }

      for (iIndex = 0; iIndex < iNumbersRead; iIndex++) {
         if (isPrime(auiNumbers[iIndex])) {
            newNode = (struct PGPRIMENUMBER *)malloc(sizeof(struct PGPRIMENUMBER));
            if (newNode != NULL) {
               newNode->uiPrime = auiNumbers[iIndex];
			   
	       /*Mutex lock due shared linked list while creating a new node, 
		 without it pPrimeList could corrupt or lose a node*/
	       pthread_mutex_lock(&(pData->listMutex));
               newNode->next = pData->pPrimeList;
               pData->pPrimeList = newNode;
	       pthread_mutex_unlock(&(pData->listMutex));

	       newNode = NULL;
            }
         }
      }
   }

   return NULL;
}

/*******************************************************************************
*** MAIN FUNCTION
*******************************************************************************/

int main(int argc, char *argv[]){
   struct THREADDATA threadData;
   pthread_t thread1;
   pthread_t thread2;
   struct PGPRIMENUMBER *pPtr = NULL;
   int rc = 0;
   /* Initialize befoure opening the file*/
   threadData.fInputFile = NULL;
   threadData.pPrimeList = NULL;
   
   /* Checks if file was provided a text file in cmdln parameter*/
   if (argc != 2) {
      puts("Run: ./program4a <filename>");
      return 1;
   }
   /* Change read file from hardcoded file to taking cmdln parameter*/
   threadData.fInputFile = fopen(argv[1], "r");
   if (threadData.fInputFile == NULL) {
      puts("ERROR: Cant open file. Verify file name, location & permissions");
      return 1;
   }

   /* Initialize mutexes prior to creating threads*/
   pthread_mutex_init(&threadData.fileMutex, NULL);
   pthread_mutex_init(&threadData.listMutex, NULL);

   /* Spawn worker thread 1 destroy on error*/
   rc = pthread_create(&thread1, NULL, threadFunction, &threadData);
   if (rc != 0) {
      /* Thread 2 has not spawned yet, nothing to join*/
      /* Waiting for somone not there could be harmful*/ 
      pthread_mutex_destroy(&threadData.fileMutex); 
      pthread_mutex_destroy(&threadData.listMutex);
      fclose(threadData.fInputFile);
      return 1;
   }
 
   /* Spawn worker thread 2 join & destroy on error*/
   rc = pthread_create(&thread2, NULL, threadFunction, &threadData);
   if (rc != 0) {
      pthread_join(thread1, NULL); /* waits for thread 1 to complete*/
      pthread_mutex_destroy(&threadData.fileMutex);
      pthread_mutex_destroy(&threadData.listMutex);
      fclose(threadData.fInputFile);
      return 1;
   }

   pthread_join(thread1, NULL); /* waits for thread 1 to complete*/
   pthread_join(thread2, NULL); /* waits for thread 2 to complete*/
   /* Always preform pthread_join befoure exiting created threads*/
   fclose(threadData.fInputFile);

   /* Print prime results*/
   printf("\r\nPrime numbers found : \r\n"); /* carrige return + newline*/
   pPtr = threadData.pPrimeList;
   while (pPtr != NULL) {
      printf("%u\r\n", pPtr->uiPrime); /* unsigned int > u% unsigned printf*/
      pPtr = pPtr->next;
   }

   /* Free list*/
   while (threadData.pPrimeList != NULL) {
      pPtr = threadData.pPrimeList;
      threadData.pPrimeList = threadData.pPrimeList->next;
      free(pPtr); pPtr = NULL;
   }

   /* Removes/shuts down mutexes befoure exiting*/
   pthread_mutex_destroy(&threadData.fileMutex);
   pthread_mutex_destroy(&threadData.listMutex);

   return 0;
}
/* END OF FILE----------------------------------------------------------------*/
