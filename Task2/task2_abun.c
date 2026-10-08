/* This file has been created by EWA, and is part of task 2 on the exam for PG3401 2026*/
/* I comment out #include <stdbool.h> and change bool to int */
int isAbundantNumber(int n) {
   int i = 0;
   int sum = 0;
   for (i = 1; i < n; i++) {
      if (n % i == 0) {
         sum += i;
      }
   }
   return (sum > n);
}
