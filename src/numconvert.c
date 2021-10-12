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

#define MY_OPT_BIN      0x0001
#define MY_OPT_DEC      0x0002
#define MY_OPT_HEX      0x0004
#define MY_OPT_OCT      0x0008
#define MY_OPT_SPACE    0x0010
#define MY_OPT_LEBYTE   0x0020  ///< print binary in little endian byte order
#define MY_OPT_LEBIT    0x0040  ///< print binary in little endian bit order
#define MY_OPT_LIMIT    0x0080  ///< print hex and binary with least amount of digits
#define MY_OPT_ASCII    0x0100
#define MY_OPT_HEADER   0x0200
#define MY_OPT_QUIET    0x0400

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
   int            bases;
   int32_t        opt;
   intmax_t       x;
   intmax_t       y;
   uintmax_t      num;
   char *         endptr;
   const char *   order;

   // getopt options
   static char   short_opt[] = "AaBbDdf:hlOoqRrsVXx";
   static struct option long_opt[] =
   {
      {"help",          no_argument, 0, 'h'},
      {"version",       no_argument, 0, 'V'},
      {NULL,            0,           0, 0  }
   };

   opt    = MY_OPT_SPACE | MY_OPT_LIMIT;
   base   = 0;
   bases  = 0;
   order  = NULL;

   while((c = getopt_long(argc, argv, short_opt, long_opt, &opt_index)) != -1)
   {
      switch(c)
      {
         case -1:	/* no more arguments */
         case 0:	/* long options toggles */
         break;

         case 'A':
         opt |= MY_OPT_ASCII;
         bases++;
         break;

         case 'a':
         base = -1;
         break;

         case 'B':
         opt |= MY_OPT_BIN;
         bases++;
         break;

         case 'b':
         base = 2;
         break;

         case 'D':
         opt |= MY_OPT_DEC;
         bases++;
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
         opt = MY_TOGGLE(opt, MY_OPT_LIMIT);
         break;

         case 'O':
         opt |= MY_OPT_OCT;
         bases++;
         break;

         case 'o':
         base = 8;
         break;

         case 'q':
         opt |= MY_OPT_QUIET;
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
         bases++;
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

   if ((order))
   {
      bases = 0;
      for(x = 0; (x < (int)strlen(order)); x++)
      {
         switch(order[x])
         {
            case 'B':
            case 'b':
            opt |= MY_OPT_BIN;
            bases++;
            break;

            case 'D':
            case 'd':
            opt |= MY_OPT_DEC;
            bases++;
            break;

            case 'X':
            case 'x':
            opt |= MY_OPT_HEX;
            bases++;
            break;

            case 'O':
            case 'o':
            opt |= MY_OPT_OCT;
            bases++;
            break;

            case 'A':
            case 'a':
            opt |= MY_OPT_ASCII;
            bases++;
            break;

            default:
            fprintf(stderr, "%s: unknown order option -- %c\n", PROGRAM_NAME, order[x]);
            fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
            return(1);
         };
      };
   };
   if (!(order))
      order  = "abxod";

   if (!(opt & (MY_OPT_BIN|MY_OPT_DEC|MY_OPT_HEX|MY_OPT_OCT|MY_OPT_ASCII)))
   {
      opt |= (MY_OPT_BIN|MY_OPT_DEC|MY_OPT_HEX|MY_OPT_OCT|MY_OPT_ASCII);
      bases = 5;
   };

   if ((argc - optind) < 1)
   {
      fprintf(stderr, "%s: missing required argument\n", PROGRAM_NAME);
      fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
      return(1);
   };

   if ( ((argc - optind) > 1) || (bases > 1) )
      if (!(opt & MY_OPT_QUIET))
         my_print(opt|MY_OPT_HEADER, 0, order);

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
            optbase = 10;
            for(y = 0; ( ((uint32_t)y) < strlen(argv[x]) && (optbase == 10) ); y++)
               if ((argv[x][y] < '0') || (argv[x][y] > '9'))
                  optbase = -1;
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
   char      buff[56];
   unsigned  pos;
   intmax_t  y;
   intmax_t  z;
   uintmax_t len;
   uintmax_t max;
   uintmax_t byte;

   len = 0;

   // determine padding
   max = sizeof(intmax_t);
   if (opt & MY_OPT_LIMIT)
      max = 4;

   for(pos = 0; (pos < strlen(order)); pos++)
   {
      switch(order[pos])
      {
         // print ASCII value
         case 'A':
         case 'a':
         if (!(opt & MY_OPT_ASCII))
            break;
         if (len)
            printf((opt & MY_OPT_SPACE) ? ", " : ",");
         if ((opt & MY_OPT_HEADER))
         {
            len = printf("%s", "TXT");
            break;
         };
         len = printf("'%c'", (((num >= 32) && (num <= 126)) ? (char)num : '.'));
         break;

         // print binary value
         case 'B':
         case 'b':
         if (!(opt & MY_OPT_BIN))
            break;
         if (len)
            printf((opt & MY_OPT_SPACE) ? ", " : ",");
         if ((opt & MY_OPT_HEADER))
         {
            len = printf("%*s", ((!(opt & MY_OPT_SPACE)) ? 0 : (((int)max*8) + ((int)max-1))), "Binary");
            break;
         };
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
         if (len)
            printf((opt & MY_OPT_SPACE) ? ", " : ",");
         if ((opt & MY_OPT_HEADER))
         {
            len = printf("%*s", (unsigned)(max*2)+2, "Hex");
            break;
         };
         len = printf("0x%0*jX", (unsigned)(max*2), num);
         break;

         // print octal value
         case 'O':
         case 'o':
         if ((opt & MY_OPT_OCT) == 0)
            break;
         if (len)
            printf((opt & MY_OPT_SPACE) ? ", " : ",");
         if ((opt & MY_OPT_HEADER))
         {
            len = printf("%*s", ((int)max*3), "Octal");
            break;
         };
         snprintf(buff, sizeof(buff), "0%jo", num);
         len = printf("%*s", ((3*(int)max)), buff);
         break;

         // print decimal value
         case 'D':
         case 'd':
         if ((opt & MY_OPT_DEC) == 0)
            break;
         if (len)
            printf((opt & MY_OPT_SPACE) ? ", " : ",");
         if ((opt & MY_OPT_HEADER))
         {
            len = printf("%*s", (((int)max/4)*10), "Decimal");
            break;
         };
         len = printf("%*ju", (((int)max/4)*10), num);
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
   printf("  -l                        print output assuming 64 bit numbers instead of 32 bit numbers\n");
   printf("  -O                        enable octal output\n");
   printf("  -o                        assume octal notation for input\n");
   printf("  -q                        suppress column names\n");
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
   printf("  0bNNNNNNNN                input number is in binary\n");
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
