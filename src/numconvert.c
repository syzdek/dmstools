/*
 *  DMS Tools and Utilities
 *  Copyright (C) 2009 David M. Syzdek <david@syzdek.net>.
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
 *  @file src/numconvert.c converts string into octal, decimal, hex, and bin
 */
/*
 *  Simple Build:
 *     gcc -W -Wall -O2 -c numconvert.c
 *     gcc -W -Wall -O2 -o numconvert   numconvert.o
 *
 *  GNU Libtool Build:
 *     libtool --mode=compile gcc -W -Wall -g -O2 -c numconvert.c
 *     libtool --mode=link    gcc -W -Wall -g -O2 -o numconvert numconvert.lo
 *
 *  GNU Libtool Install:
 *     libtool --mode=install install -c numconvert /usr/local/bin/numconvert
 *
 *  GNU Libtool Clean:
 *     libtool --mode=clean rm -f numconvert.lo numconvert
 */
#define _DMSTOOLS_SRC_NUMCONVERT_C 1

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
#include <getopt.h>
#include <string.h>


///////////////////
//               //
//  Definitions  //
//               //
///////////////////

#ifndef PROGRAM_NAME
#define PROGRAM_NAME "numconvert"
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

#define MY_OPT_BIN   0x01
#define MY_OPT_DEC   0x02
#define MY_OPT_HEX   0x04
#define MY_OPT_OCT   0x08
#define MY_OPT_SPACE 0x10
#define MY_OPT_LEBYTE   0x20  ///< print binary in little endian byte order
#define MY_OPT_LEBIT    0x40  ///< print binary in little endian bit order
#define MY_OPT_LIMIT    0x80  ///< print hex and binary with least amount of digits
#define MY_OPT_ASCII    0x100

#define MY_TOGGLE(value, bit) ( (value&bit) ? (value&(~bit)) : (value|bit))


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////

// main statement
int main(int argc, char * argv[]);

// print value
void my_print(int32_t opt, uintmax_t byte, const char * order);

//displays usage information
void my_usage(void);

// displays version information
void my_version(void);


/////////////////
//             //
//  Functions  //
//             //
/////////////////

/// main statement
/// @param[in] argc   number of arguments
/// @param[in] argv   array of arguments
int main(int argc, char * argv[])
{
   int            c;
   int            base;
   int            optbase;
   int            opt_index;
   int32_t        opt;
   intmax_t       x;
   intmax_t       y;
   uintmax_t      num;
   char *         endptr;
   const char *   order;

   // getopt options
   static char   short_opt[] = "AaBbDdf:hlOoRrsVXx";
   static struct option long_opt[] =
   {
      {"help",          no_argument, 0, 'h'},
      {"version",       no_argument, 0, 'V'},
      {NULL,            0,           0, 0  }
   };

   opt    = MY_OPT_SPACE;
   base   = 0;
   order  = "abxod";

   while((c = getopt_long(argc, argv, short_opt, long_opt, &opt_index)) != -1)
   {
      switch(c)
      {
         case -1:	/* no more arguments */
         case 0:	/* long options toggles */
         break;

         case 'A':
         opt |= MY_OPT_ASCII;
         break;

         case 'a':
         base = -1;
         break;

         case 'B':
         opt |= MY_OPT_BIN;
         break;

         case 'b':
         base = 2;
         break;

         case 'D':
         opt |= MY_OPT_DEC;
         break;

         case 'd':
         base = 10;
         break;

         case 'f':
         order = optarg;
         break;

         case 'h':
         my_usage();
         return(0);

         case 'l':
         opt |= MY_OPT_LIMIT;
         break;

         case 'O':
         opt |= MY_OPT_OCT;
         break;

         case 'o':
         base = 8;
         break;

         case 'R':
         opt |= MY_OPT_LEBYTE;
         break;

         case 'r':
         opt |= MY_OPT_LEBYTE;
         opt |= MY_OPT_LEBIT;
         break;

         case 's':
         opt = MY_TOGGLE(opt, MY_OPT_SPACE);
         break;

         case 'X':
         opt |= MY_OPT_HEX;
         break;

         case 'x':
         base = 16;
         break;

         case 'V':
         my_version();
         return(0);

         case '?':
         fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
         return(1);

         default:
         fprintf(stderr, "%s: unrecognized option `--%c'\n", PROGRAM_NAME, c);
         fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
         return(1);
      };
   };

   if (!(opt & (MY_OPT_BIN|MY_OPT_DEC|MY_OPT_HEX|MY_OPT_OCT|MY_OPT_ASCII)))
      opt |= (MY_OPT_BIN|MY_OPT_DEC|MY_OPT_HEX|MY_OPT_OCT|MY_OPT_ASCII);

   if ((argc - optind) < 1)
   {
      fprintf(stderr, "%s: missing required argument\n", PROGRAM_NAME);
      fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
      return(1);
   };

   for(x = optind; x < argc; x++)
   {
      // attempts to detect base
      optbase = base;
      if (!(optbase))
      {
         if (!(strncasecmp("0b", argv[x], (size_t)2)))
         {
            optbase = 2;
            argv[x][1] = '0';
         }
         else if (!(strncasecmp("00", argv[x], (size_t)2)))
            optbase = 2;
         else if (!(strncasecmp("0x", argv[x], (size_t)2)))
            optbase = 16;
         else if (!(strncmp("0", argv[x], (size_t)1)))
            optbase = 8;
         else
         {
            for(y = 0; ( ((uint32_t)y) < strlen(argv[x]) && (optbase != -1) ); y++)
            {
               if      ( (optbase < 10) && ((argv[x][y] >= '0') && (argv[x][y] <= '9')) ) optbase = 10;
               else if ( (optbase < 16) && ((argv[x][y] >= 'a') && (argv[x][y] <= 'f')) ) optbase = 16;
               else if ( (optbase < 16) && ((argv[x][y] >= 'a') && (argv[x][y] <= 'F')) ) optbase = 16;
               else optbase = -1;
            };
         };
      };

      // print value
      switch(optbase)
      {
         case -1:
         for(y = 0; (y < (int)strlen(argv[x])); y++)
            my_print(opt, (intmax_t)argv[x][y], order);
         break;

         default:
         num = strtoumax(argv[x], &endptr, optbase);
         if (endptr[0] != '\0')
         {
            switch(optbase)
            {
               case  2: fprintf(stderr, "%s:%i: invalid binary number\n",      PROGRAM_NAME, (int)(x - optind + 1)); return(1);
               case  8: fprintf(stderr, "%s:%i: invalid octal number\n",       PROGRAM_NAME, (int)(x - optind + 1)); return(1);
               case 10: fprintf(stderr, "%s:%i: invalid decimal number\n",     PROGRAM_NAME, (int)(x - optind + 1)); return(1);
               default: fprintf(stderr, "%s:%i: invalid hexadecimal number\n", PROGRAM_NAME, (int)(x - optind + 1)); return(1);
            };
         };
         my_print(opt, num, order);
         break;
      };
   };

   return(0);
}


// print value
void my_print(int32_t opt, uintmax_t num, const char * order)
{
   char      buff[9];
   unsigned  pos;
   intmax_t  y;
   intmax_t  z;
   uintmax_t len;
   uintmax_t max;
   uintmax_t byte;

   max = sizeof(intmax_t);
   len = 0;

   // determine padding
   if (opt & MY_OPT_LIMIT)
   {
      max = 1;
      for(y = 0; y < (intmax_t)sizeof(intmax_t); y++)
         if ( (num >> (y*8)) & 0xFF )
            max = (y+1);
      if (max < 2)
         max = 4;
   };

   for(pos = 0; (pos < strlen(order)); pos++)
   {
      switch(order[pos])
      {
         // print ASCII value
         case 'A':
         case 'a':
         if ((opt & MY_OPT_ASCII) == 0)
            break;
         len = printf((len ? ((opt & MY_OPT_SPACE) ? ", '%c'" : ",'%c'") : "'%c'"), ((((num & 0xFF) >= 32) && ((num & 0xFF) <= 126)) ? (char)(num & 0xFF) : '.'));
         break;

         // print binary value
         case 'B':
         case 'b':
         if ((opt & MY_OPT_BIN) == 0)
            break;
         if (len)
            printf((opt & MY_OPT_SPACE) ? ", " : ",");
         for(y = 0; ((uint32_t)y) < max; y++)
         {
            if (opt & MY_OPT_LEBYTE)
               byte = num >> (8*y);
            else
               byte = num >> (8*(max-y-1));
            if (opt & MY_OPT_LEBIT)
               for(z = 0; z < 8; z++)
                  buff[z] = (byte & (0x01 << z)) ? '1' : '0';
            if (!(opt & MY_OPT_LEBIT))
               for(z = 0; z < 8; z++)
                  buff[z] = (byte & (0x01 << (7-z))) ? '1' : '0';
            buff[z] = '\0';
            printf("%s", buff);
            if ((opt & MY_OPT_SPACE) && (((uint32_t)y+1) < max))
               printf(" ");
         };
         len = 1;
         break;

         // print hexadecimal value
         case 'X':
         case 'x':
         if ((opt & MY_OPT_HEX) == 0)
            break;
         len = printf((len ? ((opt & MY_OPT_SPACE) ? ", 0x%0*jX" : ",0x%0*jX") : "0x%0*jX"), (unsigned)(max*2), num);
         break;

         // print octal value
         case 'O':
         case 'o':
         if ((opt & MY_OPT_OCT) == 0)
            break;
         len = printf((len ? ((opt & MY_OPT_SPACE) ? ", 0%jo"    : ",0%jo")    : "0%jo"),    num);
         break;

         // print decimal value
         case 'D':
         case 'd':
         if ((opt & MY_OPT_DEC) == 0)
            break;
         len = printf((len ? ((opt & MY_OPT_SPACE) ? ", %ju"     : ",%ju")     : "%ju"),     num);
         break;

         default:
         break;
      };
   };

   printf("\n");

   return;
}


/// displays usage information
void my_usage()
{
   printf("Usage: %s num1 num2 ... numX\n", PROGRAM_NAME);
   printf("  -A                        enable ASCII output\n");
   printf("  -a                        assume binary notation for input\n");
   printf("  -B                        enable binary output\n");
   printf("  -b                        assume binary notation for input\n");
   printf("  -D                        enable decimal output\n");
   printf("  -d                        assume decimal notation for input\n");
   printf("  -f order                  order of output values (default: \"abxod\")\n");
   printf("  -h, --help                print this help and exit\n");
   printf("  -l                        print minimum number of bytes in hex and binary\n");
   printf("  -O                        enable octal output\n");
   printf("  -o                        assume octal notation for input\n");
   printf("  -R                        display binary in little endian byte order\n");
   printf("  -r                        display binary in little endian bit nad byte order\n");
   printf("  -s                        display binary value in 8 bit blocks\n");
   printf("  -X                        enable hexadecimal output\n");
   printf("  -x                        assume hexadecimal notation for input\n");
   printf("  -V, --version             print version number and exit\n");
   printf("\n");
   printf("Input Notation:\n");
   printf("  NNNNNNNN                  input number is in decimal\n");
   printf("  0xNNNNNNNN                input number is in hexadecimal\n");
   printf("  0NNNNNNNN                 input number is in octal\n");
   printf("  00NNNNNNNN                input number is in binary\n");
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

/* end of source code */
