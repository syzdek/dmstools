#include <stdio.h>

int main(void);

int main(void)
{
   int x;
   int y;

   for (y = 0; y < 9; y++)
   {
      printf("\033[40m");
      // displays odd rows of colors without bold
      for (x = 0; x < 9; x++)
         printf(" \033[3%i;4%im  3%i;4%im  \033[40m", x, y, x, y);
      printf(" \033[0m\n\033[40m");
      // adds row of spaces
      for (x = 0; x < 9; x++)
         printf(" \033[30;40m          ");
      printf(" \033[0m\n\033[40m");
      // displays even rows of colors with bold
      for (x = 0; x < 9; x++)
         printf(" \033[1;3%i;4%im 1;3%i;4%im \033[40m", x, y, x, y);
      printf(" \033[0m\n\033[40m");
      // adds row of spaces
      for (x = 0; x < 9; x++)
         printf(" \033[30;40m          ");
      printf(" \033[0m\n\033[40m");
   };
   printf("\033[0m\n");

   return(0);
};
