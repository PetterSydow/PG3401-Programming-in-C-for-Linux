/* This file has been created by EWA, and is part of task 2 on the exam for PG3401 2026*/
/* comment out #include <stdbool.h> set bool to int*/
/* chose to drop sqrt() and rewrite here also: no need for #include <math.h>*/
int isSquareNumber(int n) {
   int i = 0;
   int square = 0;

   if (n < 0) {
      return 0;
   }

   while (1) {
      square = i * i; /* Large numbers could need a long int*/

      if (square == n) {
         return 1;
      }

      if (square > n) {
         return 0;
      }

      i++;
   }   
}
