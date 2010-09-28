#include <stdio.h>

int main(void);

int main(void) {

   int x,y;

   for (y = 0; y < 9; y++) {
      for (x = 0; x < 9; x++)
         printf(" [3%i;4%im  3%i;4%im  [40m", x, y, x, y);
      printf("[0m\n");
      for (x = 0; x < 9; x++)
         printf(" [30;40m  30;40m  [40m");
      printf("[0m\n");
      for (x = 0; x < 9; x++)
         printf(" [1;3%i;4%im 1;3%i;4%im [40m", x, y, x, y);
      printf("[0m\n");
      for (x = 0; x < 9; x++)
         printf(" [30;40m  30;40m  [40m");
      printf("[0m\n");
   };
   printf("[0m\n");

   return(0);
};
