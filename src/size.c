
#include <unistd.h>
#include <stdio.h>

int main(void)
{
   printf("size: %u\n", getdtablesize());
   return(0);
}
