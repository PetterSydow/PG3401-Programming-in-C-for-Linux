/* MAIN.C file ----------------------------------------------------------------

   Role:    Task 2 main.c program file
   Version: Dynamic loop 2
   Author:  Candidate 262

   Description: Reads integers from pgexam26_test.txt, testing each number 
   using task2_ helper functions from EWA, for each number writes one 
   binary struct in the provided format to pgexam26_output.bin.     

------------------------------------------------------------------------------*/

/*** Standard C libraries *****************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*** Task include files *******************************************************/
#include "task2_abun.h"
#include "task2_cube.h"
#include "task2_def.h"
#include "task2_fib.h"
#include "task2_kvad.h"
#include "task2_odd.h"
#include "task2_perf.h"
#include "task2_prim.h"

/*** Task data struct *********************************************************/
struct TASK2_NUMBERS_METADATA {
   int iIndex;
   int iNumber;
   int bIsFibonacci;
   int bIsPrimeNumber;
   int bIsSquareNumber;
   int bIsCubeNumber;
   int bIsPerfectNumber;
   int bIsAbundantNumber;
   int bIsDeficientNumber;
   int bIsOddNumber;
};

/*******************************************************************************
*** MAIN FUNCTION
*******************************************************************************/

int main(void) {
   FILE *fInput = NULL;
   FILE *fOutput = NULL;
   struct TASK2_NUMBERS_METADATA *pNumberData = NULL;
   char szLine[64];
   int iNumber = 0;
   int iIndex = 0;   /* EWA verification expects 0 based index*/

   puts("*** Running Task 2 ***");

   /* Allocate memory for struct, reused for each source number read*/
   pNumberData = (struct TASK2_NUMBERS_METADATA *)
      malloc(sizeof(struct TASK2_NUMBERS_METADATA));
   if (pNumberData == NULL) {
      puts("ERROR: Memory allocation failed.");
      return 1;
   }
   /* Made redundant (always initialize instead): */
   memset(pNumberData, 0, sizeof(struct TASK2_NUMBERS_METADATA));

   /* Open the EWA input txt file*/
   fInput = fopen("pgexam26_test.txt", "r");
   if (fInput == NULL) {
      puts("ERROR: Could not open txt file. Check location & permissions.");
      free(pNumberData);
      pNumberData = NULL;
      return 1;
   }

   /* Creates and Writes to the binary output file*/
   fOutput = fopen("pgexam26_output.bin", "wb");
   if (fOutput == NULL) {
      puts("ERROR: Failed to create pgexam26_output.bin. Verify permissions.");
      fclose(fInput);
      free(pNumberData);
      pNumberData = NULL;
      return 1;
   }

   /* Read each number, fill the struct, write it as binary data*/
   while (fgets(szLine, sizeof(szLine), fInput) != NULL) {
      if (sscanf(szLine, "%d", &iNumber) == 1) {

         /* Resetting struct fields before filling with values*/
         memset(pNumberData, 0, sizeof(struct TASK2_NUMBERS_METADATA));    

         pNumberData->iIndex = iIndex;
         pNumberData->iNumber = iNumber;
         pNumberData->bIsFibonacci = isFibonacci(iNumber);
         pNumberData->bIsPrimeNumber = isPrime(iNumber);
         pNumberData->bIsSquareNumber = isSquareNumber(iNumber);
         pNumberData->bIsCubeNumber = isCubeNumber(iNumber);
         pNumberData->bIsPerfectNumber = isPerfectNumber(iNumber);
         pNumberData->bIsAbundantNumber = isAbundantNumber(iNumber);
         pNumberData->bIsDeficientNumber = isDeficientNumber(iNumber);
         pNumberData->bIsOddNumber = isOdd(iNumber);

         /* Write one completed number record to the bin output file*/
         if (fwrite(pNumberData, 
               sizeof(struct TASK2_NUMBERS_METADATA),
               1, 
               fOutput) != 1) {
            puts("ERROR: Failed while writing to pgexam26_output.bin.");
            fclose(fOutput);
            fclose(fInput);
            free(pNumberData);
            pNumberData = NULL;
            return 1;
         }

         iIndex++;
      }
   }

   /* Check if the loop ended because of a read error*/
   if (ferror(fInput)) {
      puts("ERROR: Problem encountered while reading pgexam26_test.txt.");
      fclose(fOutput);
      fclose(fInput);
      free(pNumberData);
      pNumberData = NULL;
      return 1;
   }

   /* Close files and releases allocated memory befoure exiting program*/
   fclose(fOutput);
   fclose(fInput);
   free(pNumberData);
   pNumberData = NULL;

   printf("Wrote %d records to pgexam26_output.bin.\n", iIndex);
   puts("*** Program finished successfully ***");

   return 0;
}

/* END OF FILE ---------------------------------------------------------------*/
