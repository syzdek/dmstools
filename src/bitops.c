/*
 *  DMS Tools and Utilities
 *  Copyright (C) 2008 David M. Syzdek <david@syzdek.net>.
 *
 *  @SYZDEK_LICENSE_HEADER_START@
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of David M. Syzdek nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 *  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL DAVID M. SYZDEK BE LIABLE FOR
 *  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 *  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 *  SUCH DAMAGE.
 *
 *  @SYZDEK_LICENSE_HEADER_END@
 */
/**
 *  @file src/bitops.c simple utility for quick bitwise operations
 */
/*
 *  Simple Build:
 *     gcc -W -Wall -O2 -c bitops.c
 *     gcc -W -Wall -O2 -o bitops   bitops.o
 *
 *  GNU Libtool Build:
 *     libtool --mode=compile gcc -W -Wall -g -O2 -c bitops.c
 *     libtool --mode=link    gcc -W -Wall -g -O2 -o bitops bitops.lo
 *
 *  GNU Libtool Install:
 *     libtool --mode=install install -c bitops /usr/local/bin/bitops
 *
 *  GNU Libtool Clean:
 *     libtool --mode=clean rm -f bitops.lo bitops
 */
#define _SCT_SRC_BITOPS_C 1

///////////////
//           //
//  Headers  //
//           //
///////////////

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>


///////////////////
//               //
//  Definitions  //
//               //
///////////////////

#ifndef PROGRAM_NAME
#define PROGRAM_NAME "bitops"
#endif
#ifndef PACKAGE_BUGREPORT
#define PACKAGE_BUGREPORT "david@syzdek.net"
#endif
#ifndef PACKAGE_COPYRIGHT
#define PACKAGE_COPYRIGHT ""
#endif
#ifndef PACKAGE_NAME
#define PACKAGE_NAME ""
#endif
#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION ""
#endif

#ifndef PARAMS
#define PARAMS(protos) protos
#endif

#define STR_LEN     ((sizeof(uint32_t) * 8) + 4)

#define MY_OPT_BIN  0x01
#define MY_OPT_DEC  0x02
#define MY_OPT_HEX  0x00
#define MY_OPT_OCT  0x04


/////////////////
//             //
//  Variables  //
//             //
/////////////////

static uint32_t my_opts = MY_OPT_HEX;
char      buff1[STR_LEN];
char      buff2[STR_LEN];
char      buff3[STR_LEN];


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////

// main statement
int main(int argc, char * argv[]);

const char * my_notation (char * dst);

// prints number
const char * my_uint2str(char * dst, uint32_t num);

const char * my_spaces(char * dst);

// converts string to numeric value
uint32_t my_strtouint(const char * str);

//displays usage information
void my_usage(void);

// displays version information
void my_version(void);

//////////////////
//              //
//  Prototypes  //
//              //
//////////////////

/// main statement
/// @param[in] argc   number of arguments
/// @param[in] argv   array of arguments
int main(int argc, char * argv[])
{
   int      c;
   int      opt_index;
   uint32_t opts;
   uint32_t var1;
   uint32_t var2;

   // getopt options
   static char   short_opt[] = "BDhOVX";
   static struct option long_opt[] =
   {
      {"help",          no_argument, 0, 'h'},
      {"version",       no_argument, 0, 'V'},
      {NULL,            0,           0, 0  }
   };
   
   opts = MY_OPT_HEX;

   while((c = getopt_long(argc, argv, short_opt, long_opt, &opt_index)) != -1)
   {
      switch(c)
      {
         case -1:	/* no more arguments */
         case 0:	/* long options toggles */
            break;
         case 'B':
            opts = MY_OPT_BIN;
            break;
         case 'D':
            opts = MY_OPT_DEC;
            break;
         case 'h':
            my_usage();
            return(0);
         case 'O':
            opts = MY_OPT_OCT;
            break;
         case 'V':
            my_version();
            return(0);
         case 'X':
            opts = MY_OPT_HEX;
            break;
         case '?':
            fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
            return(1);
         default:
            fprintf(stderr, "%s: unrecognized option `--%c'\n", PROGRAM_NAME, c);
            fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
            return(1);
      };
   };

   if ((argc - optind) != 2)
   {
      my_usage();
      return(1);
   };

   var1 = my_strtouint(argv[optind+0]);
   var2 = my_strtouint(argv[optind+1]);

   my_opts = MY_OPT_BIN;
   printf("             %12s      %12s\n",     "First Value", "Second Value");
   snprintf(buff1, STR_LEN, "0%o", var1);
   snprintf(buff2, STR_LEN, "0%o", var2);
   printf("Octal:       %12s      %12s\n",   buff1, buff2);
   snprintf(buff1, STR_LEN, "%u", var1);
   snprintf(buff2, STR_LEN, "%u", var2);
   printf("Decimal:     %12s      %12s\n",   buff1, buff2);
   snprintf(buff1, STR_LEN, "0x%08X", var1);
   snprintf(buff2, STR_LEN, "0x%08X", var2);
   printf("Hexadecimal: %12s      %12s\n",   buff1, buff2);

   printf("\n");

   printf("Binary:      %s First Value\n", my_uint2str(buff1, var1));
   printf("             %s Second Value\n", my_uint2str(buff2, var2));   snprintf(buff1, STR_LEN, "0%o", var1);

   printf("\n");

   my_opts = opts;
   printf("Left Shift:   %s  <<  %s == %s\n", my_uint2str(buff1, var1), my_uint2str(buff2, var2), my_uint2str(buff3, (var1 << var2)));
   printf("Right Shift:  %s  >>  %s == %s\n", my_uint2str(buff1, var1), my_uint2str(buff2, var2), my_uint2str(buff3, (var1 >> var2)));
   printf("AND:          %s   &  %s == %s\n", my_uint2str(buff1, var1), my_uint2str(buff2, var2), my_uint2str(buff3, (var1 & var2)));
   printf("Inclusive OR: %s   |  %s == %s\n", my_uint2str(buff1, var1), my_uint2str(buff2, var2), my_uint2str(buff3, (var1 | var2)));
   printf("OR:           %s   ^  %s == %s\n", my_uint2str(buff1, var1), my_uint2str(buff2, var2), my_uint2str(buff3, (var1 ^ var2)));
   printf("NEGATION:     %s   ~  %s == %s\n", my_spaces(buff3),         my_uint2str(buff1, var1), my_uint2str(buff2, ~var1));

   printf("\n");

   printf("Unset Bit:   %s & (~%s) == %s\n", my_uint2str(buff1, var1), my_uint2str(buff2, var2), my_uint2str(buff3, (var1 & (~var2))));
   
   return(0);
}


/// prints notation type
/// @param[in] dst   output buffer
const char * my_notation(char * dst)
{
   dst[0] = '\0';
   switch(my_opts)
   {
      case MY_OPT_BIN:
         snprintf(dst, STR_LEN, "binary");
         return(dst);
      case MY_OPT_DEC:
         snprintf(dst, STR_LEN, "decimal");
         return(dst);
      case MY_OPT_HEX:
         snprintf(dst, STR_LEN, "hexadecimal");
         return(dst);
      case MY_OPT_OCT:
         snprintf(dst, STR_LEN, "octal");
         return(dst);
      default:
         snprintf(dst, STR_LEN, "decimal");
         break;
   };
   return(dst);
}


/// prints number
/// @param[in] dst   output buffer
/// @param[in] num   output number
const char * my_uint2str(char * dst, uint32_t num)
{
   int  i;
   int  pos;
   char fmt[STR_LEN];
   char str[STR_LEN];
   memset(dst, 0, STR_LEN);
   switch(my_opts)
   {
      case MY_OPT_BIN:
         pos = 0;
         for(i = 0; i < 32; i++)
         {
            if ((i > 0) && (!(i % 8)) )
            {
               dst[pos] = ' ';
               pos++;
            };
            dst[pos] = (num & (0x01 << (31-i))) ? '1' : '0';
            pos++;
         };
         return(dst);
      case MY_OPT_DEC:
         snprintf(fmt, STR_LEN, "%%%uu", 10);
         snprintf(dst, STR_LEN, fmt, num);
         return(dst);
      case MY_OPT_HEX:
         snprintf(str, STR_LEN, "0x%08X", num);
         snprintf(dst, STR_LEN, "%s", str);
         return(dst);
      case MY_OPT_OCT:
         snprintf(str, STR_LEN, "0%o", num);
         snprintf(dst, STR_LEN, "%17s", str);
         return(dst);
      default:
         break;
   };
   return(dst);
}


/// converts string to numeric value
/// @param[in] str   string to convert
uint32_t my_strtouint(const char * str)
{
   if (!(strncmp(str, "0x", (size_t)2)))
      return((uint32_t)strtoul(&str[2], NULL, 16));
   if (!(strncmp(str, "0X", (size_t)2)))
      return((uint32_t)strtoul(&str[2], NULL, 16));
   if (!(strncmp(str, "00", (size_t)2)))
      return((uint32_t)strtoul(str, NULL, 2));
   if (!(strncmp(str, "0", (size_t)1)))
      return((uint32_t)strtoul(str, NULL, 8));
   return((uint32_t)strtoul(str, NULL, 10));
}


/// prints number
/// @param[in] opt   output options
const char * my_spaces(char * dst)
{
   dst[0] = '\0';
   switch(my_opts)
   {
      case MY_OPT_BIN:
         snprintf(dst, STR_LEN, "%35s", " ");
         return(dst);
      case MY_OPT_DEC:
         snprintf(dst, STR_LEN, "%10s", " ");
         return(dst);
      case MY_OPT_HEX:
         snprintf(dst, STR_LEN, "%10s", " ");
         return(dst);
      case MY_OPT_OCT:
         snprintf(dst, STR_LEN, "%-17s", " ");
         return(dst);
      default:
         break;
   };
   return(dst);
}


/// displays usage information
void my_usage()
{
   printf("Usage: %s [options] num1 num2\n", PROGRAM_NAME);
   printf("  -B                        display output in binary notation\n");
   printf("  -D                        display output in decimal notation\n");
   printf("  -O                        display output in octal notation\n");
   printf("  -X                        display output in hexadecimal notation\n");
   printf("\n");
   printf("Input Notation:\n");
   printf("  00NNNNNNNN                input number is in binary\n");
   printf("  NNNNNNNN                  input number is in decimal\n");
   printf("  0xNNNNNNNN                input number is in hexadecimal\n");
   printf("  0NNNNNNNN                 input number is in octal\n");
   printf("\n");
   printf("Report bugs to <%s>.\n", PACKAGE_BUGREPORT);
   return;
}


/// displays version information
void my_version(void)
{
   printf("%s (%s) %s\n", PROGRAM_NAME, PACKAGE_NAME, PACKAGE_VERSION);
   printf("Written by David M. Syzdek.\n");
   printf("\n");
   printf("%s\n", PACKAGE_COPYRIGHT);
   printf("This is free software; see the source for copying conditions.  There is NO\n");
   printf("warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n");
   return;
}


// end of source file
