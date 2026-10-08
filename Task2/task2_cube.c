/* This file has been created by EWA, and is part of task 2 on the exam for PG3401 2026*/
/* Larger changes, rewritten this > as beyond using bool the method used cbrt() was problematic*/
int isCubeNumber(int n) {
   int i = 0;
   int cube = 0;

   if (n < 0) {
      return 0;
   }

   while (1) {
      cube = i * i * i; /*Could be problematic with very large number*/

      if (cube == n) {
         return 1;
      }

      if (cube > n) {
         return 0;   
      }

      i++;
   }
}

