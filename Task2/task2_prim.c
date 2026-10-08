/* This file has been created by EWA, and is part of task 2 on the exam for PG3401 2026*/
/* comment out #include <stdbool.h>, set bool > int, and I change false/true to 0/1*/
int isPrime(int n) {
   int i = 0;
   if (n <= 1) return 0;
   for (i = 2; i < n; i++) {
      if (n % i == 0) {
         return 0;
      }
   }
   return 1;
}
