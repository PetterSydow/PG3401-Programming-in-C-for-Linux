/* This file has been created by EWA, and is part of task 4 on the exam for PG3401 2026*/
/* MAIN.C file -----------------------------------------------------------------

   Task:	4 part 1 - main.c program file
   Version:	1.0 "The Earthquake"
   Author: 	EWA 
   Alterations: Candidate 262
   
   Description: Fixed issues, removed partial boolean true/false logic, removed 
   global-variables - by using a new struct for this "threaddata".  
   Added mutexes with explicit initilzation, to protect shared pointer access of
   file read and node spawning.
   Changed code from hardcoded input to take cmdln program+<filename> input.

================================================================================

   Update:      part 2 - main.c
   Version;     1.b "xtean"
   Author:      Candidate 262

   Description: Has a new function, testIfBengtIsCool. Any other changes I marked 
   with Task B. Encrypted file is read into rbuffer decrypted into wbuffer, and 
   results tested with the known start "BENGT". 
   A shared iFound flag was used to stop threads ASAP. A new foundMutex was used 
   to protect iFound.

    PART II:
    The reason why I needed your help to calculate prime numbers is that      
    a file I have is encrypted by XTEA using one of those prime numbers.      
    I found the hash of the decrypted file. Could you crack task4_code.bin?   
     1 For each prime your threads find try to decrypt the file
     2 The first 5 characters are known to be BENGT, check for that result
     3 If you can crack it, save the file as task4_plain.txt
     4 Calculate a IP-checksum of the file, write hash to task4_plain.hash
     5 Once you find a match signal the other threads so both can quit asap
    Base your code on the samples I created for you; ipc.c and xtea.c.


------------------------------------------------------------------------------*/

/*** Standard C libraries *****************************************************/
#include <stddef.h> /* Added as part B included the use of this*/
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* Seems all good*/ 
#include <unistd.h> /* Linux os related, not strictly standard, 
                       but supported c89, should compile */

/*** Task spesific includes ***************************************************/

#include "task4_prime.h"

/*** Task B spesific **********************************************************/

void tean(unsigned int *const v, unsigned int *const w,
   const unsigned int *const k, int N);

unsigned int tcp_checksum(const unsigned char *data, size_t length);

/*** Task spesific data & thread function *************************************/ 

/* Linked list storing prime numbers found*/
struct PGPRIMENUMBER {
   unsigned int uiPrime;
   struct PGPRIMENUMBER *next;
};

/* Shared thread data struct passed from main to both worker threads,
   replacing the original global file & list pointers*/
struct THREADDATA {
   FILE *fInputFile;
   struct PGPRIMENUMBER *pPrimeList;
   int iFound;   /* Task B set to 1 when right key is found,shared stop signal*/
   pthread_mutex_t fileMutex;
   pthread_mutex_t listMutex;
   pthread_mutex_t foundMutex; /* Task B new Mutex to protect iFound*/
};

/*** Task B spesific **********************************************************/
/* Decrypt task4_code.bin using the identified prime numbers as XTEA keys*/
int testIfBengtIsCool(unsigned int key) {
   FILE *fInput = NULL;
   FILE *fOutput = NULL;
   FILE *fHash = NULL;
   unsigned char rbuffer[128];
   unsigned char wbuffer[128];
   size_t i = 0;
   size_t readBytes = 0;
   unsigned int checksum = 0;
   unsigned int k[4];   /* xtea 4 keys*/
   unsigned int v[2];   /* input block 2 v[0] v[1]*/
   unsigned int w[2];   /* output block 2 w[0] w[1]*/
   /* Initalize the buffers*/
   memset(rbuffer, 0, sizeof(rbuffer));
   memset(wbuffer, 0, sizeof(wbuffer));
   
   /* Check xtea, same prime key for all positions*/
   k[0] = key;
   k[1] = key;
   k[2] = key;
   k[3] = key;

   fInput = fopen("task4_code.bin","rb");
   if (fInput == NULL) {
      return 0;
   }

   /* Reads encypted data*/
   readBytes = fread(rbuffer, 1, sizeof(rbuffer), fInput);
   fclose(fInput);

   /* 2x unsiged int, decrypts in 8 byte blocks*/
   for (i = 0; i + 7 < readBytes; i += 8) {
      memcpy(v, &rbuffer[i], 8);   /* load encrypted 64 bit block*/

   /* Need N < 0, negative to decode with tean and + to encode*/
      tean(v, w, k, -32);

   /* Store decrypted 64b results*/
      memcpy(&wbuffer[i], w, 8); 
   }

   /* Checks if data stored in buffer corresponds to BENGT*/
   if (readBytes >= 5) {
      if(wbuffer[0] == 'B' &&
         wbuffer[1] == 'E' &&
         wbuffer[2] == 'N' &&
         wbuffer[3] == 'G' &&
         wbuffer[4] == 'T') {

	 /* Writes the decrypted file*/
         fOutput = fopen("task4_plain.txt", "wb");
         if (fOutput != NULL) {
            fwrite(wbuffer, 1, readBytes, fOutput);
            fclose(fOutput);
         }

         /* Calculates and writes checksum*/
         {
            checksum = tcp_checksum(wbuffer, readBytes);

            fHash = fopen("task4_plain.hash", "w");
            if (fHash != NULL) {
               fprintf(fHash, "%u\n", checksum);
               fclose(fHash);
            }
         }

         return 1;
      }
   }

   return 0;
}
   
/*** Task A thread function ***************************************************/

   void* threadFunction(void* arg) {
      struct THREADDATA *pData = NULL;
      unsigned int auiNumbers[10] = {0};
      int iNumbersRead = 0;
      int iIndex = 0;
      struct PGPRIMENUMBER *newNode = NULL;

      pData = (struct THREADDATA *)arg;

      while (1) { /* Will not try #define true/false = 1/0 just change to 1/0*/
         /* Task B check if another thread has found the right solution in the 
	    shared iFound is(=1). Using Mutex locks here for controlled access. 
	    If found stops immediatly (asap). Additional unlock here because of 
	    the break, without an unlock prior to break > deadlock*/
         pthread_mutex_lock(&(pData->foundMutex));
         if (pData->iFound) {
            pthread_mutex_unlock(&(pData->foundMutex));
            break;
      }
      pthread_mutex_unlock(&(pData->foundMutex));
   
      iNumbersRead = 0;

      /* Mutex lock placed due to shared file reader pointer (fscanf) for 
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

            /* Task B Signal to halt, If a thread finds the correct key it 
               signals by setting iFound to 1. Mutex lock here is to avoid race
	       conditions*/
            if (testIfBengtIsCool(auiNumbers[iIndex])) {
               pthread_mutex_lock(&(pData->foundMutex));
               pData->iFound = 1;
               pthread_mutex_unlock(&(pData->foundMutex));
            }

            /* Adds prime to linked list*/
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
   threadData.iFound = 0;   /* Task B Initialize befoure opening the file*/
   threadData.fInputFile = NULL;
   threadData.pPrimeList = NULL;
   
   /* Checks if file was provided a text file in cmdln parameter*/
   if (argc != 2) {
      puts("Run: ./program4b <filename>"); /* forgot this updated it for b*/
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
   pthread_mutex_init(&threadData.foundMutex, NULL); /* Task B mutex*/

   /* Spawn worker thread 1 destroy on error*/
   rc = pthread_create(&thread1, NULL, threadFunction, &threadData);
   if (rc != 0) {
      /* Thread 2 has not spawned yet, nothing to join*/
      /* Waiting for somone not there could be harmful*/ 
      pthread_mutex_destroy(&threadData.fileMutex); 
      pthread_mutex_destroy(&threadData.listMutex);
      pthread_mutex_destroy(&threadData.foundMutex); /* Task B destroy*/
      fclose(threadData.fInputFile);
      return 1;
   }
 
   /* Spawn worker thread 2 join & destroy on error*/
   rc = pthread_create(&thread2, NULL, threadFunction, &threadData);
   if (rc != 0) {
      pthread_join(thread1, NULL); /* waits for thread 1 to complete*/
      pthread_mutex_destroy(&threadData.fileMutex);
      pthread_mutex_destroy(&threadData.listMutex);
      pthread_mutex_destroy(&threadData.foundMutex); /* Task B Mutex*/
      fclose(threadData.fInputFile);
      return 1;
   }

   /* Always preform pthread_join befoure exiting created threads*/
   pthread_join(thread1, NULL); /* waits for thread 1 to complete*/
   pthread_join(thread2, NULL); /* waits for thread 2 to complete*/
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
   pthread_mutex_destroy(&threadData.foundMutex);   /* Task B Mutex*/

   return 0;
}
/* END OF FILE----------------------------------------------------------------*/
