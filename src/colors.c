/*
 *  DMS Tools and Utilities
 *  Copyright (C) 2010 David M. Syzdek <david@syzdek.net>.
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
 *  @file src/colors.c displays various colors using ANSI escape codes
 */
/*
 *  Simple Build:
 *     gcc -W -Wall -O2 -c colors.c
 *     gcc -W -Wall -O2 -o colors   colors.o
 *
 *  GNU Libtool Build:
 *     libtool --mode=compile gcc -W -Wall -g -O2 -c colors.c
 *     libtool --mode=link    gcc -W -Wall -g -O2 -o colors colors.lo
 *
 *  GNU Libtool Install:
 *     libtool --mode=install install -c colors /usr/local/bin/colors
 *
 *  GNU Libtool Clean:
 *     libtool --mode=clean rm -f colors.lo colors
 */
#define _DMSTOOLS_SRC_COLORS_C 1

///////////////
//           //
//  Headers  //
//           //
///////////////

#ifdef HAVE_COMMON_H
#include "common.h"
#endif

#include <stdio.h>
#include <getopt.h>


///////////////////
//               //
//  Definitions  //
//               //
///////////////////

#ifndef PROGRAM_NAME
#define PROGRAM_NAME "codetagger"
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


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////

// displays usage
void colors_usage PARAMS((void));

// displays version information
void colors_version PARAMS((void));

// main statement
int main PARAMS((int argc, char * argv[]));


/////////////////
//             //
//  Functions  //
//             //
/////////////////

/// displays usage
void colors_usage(void)
{
   printf("Usage: %s [OPTIONS]\n", PROGRAM_NAME);
   printf("  -c, --colors-only         only display escape codes for colors\n");
   printf("  -h, --help                print this help and exit\n");
   printf("  -V, --version             print version number and exit\n");
   printf("\n");
   printf("Report bugs to <%s>.\n", PACKAGE_BUGREPORT);
   return;
}


/// displays version information
void colors_version(void)
{
   printf("%s (%s) %s\n", PROGRAM_NAME, PACKAGE_NAME, PACKAGE_VERSION);
   printf(      "Written by David M. Syzdek.\n");
   printf("\n");
   printf("%s\n", PACKAGE_COPYRIGHT);
   printf("This is free software; see the source for copying conditions.  There is NO\n");
   printf("warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n");
   return;
}


/// main statement
/// @param[in]  argc  number of arguments passed to program
/// @param[in]  argv  array of arguments passed to program
int main(int argc, char * argv[])
{
   int           c;
   int           x;
   int           y;
   int           opt;
   int           opt_index;

   static char   short_opt[] = "chV";
   static struct option long_opt[] =
   {
      {"colors-only",   no_argument, 0, 'c'},
      {"help",          no_argument, 0, 'h'},
      {"version",       no_argument, 0, 'V'},
      {NULL,            0,           0, 0  }
   };

#ifdef HAVE_GETTEXT
   setlocale (LC_ALL, "");
   bindtextdomain (PACKAGE, LOCALEDIR);
   textdomain (PACKAGE);
#endif

   opt = 0;

   while((c = getopt_long(argc, argv, short_opt, long_opt, &opt_index)) != -1)
   {
      switch(c)
      {
         case -1:	/* no more arguments */
         case 0:	/* long options toggles */
            break;
         case 'c':
            opt = opt | 0x01;
            break;
         case 'h':
            colors_usage();
            return(0);
         case 'V':
            colors_version();
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

   for (y = 0; y < 9; y++)
   {
      printf("\033[40m");

      // displays odd rows of colors
      for (x = 0; x < 9; x++)
         printf(" \033[0;3%i;4%im 0;3%i;4%im \033[40m", x, y, x, y);
      printf(" \033[0m\n\033[40m");

      // displays even rows of colors with bright
      for (x = 0; x < 9; x++)
         printf(" \033[1;3%i;4%im 1;3%i;4%im \033[40m", x, y, x, y);
      printf(" \033[0m\n\033[40m");

      // displays even rows of colors with faint
      for (x = 0; x < 9; x++)
         printf(" \033[2;3%i;4%im 2;3%i;4%im \033[40m", x, y, x, y);
      printf(" \033[0m\n\033[40m");

      // adds row of spaces
      for (x = 0; x < 9; x++)
         printf(" \033[30;40m          ");
      printf(" \033[0m\n\033[40m");
   };

   if (!(opt & 0x01))
   {
      // displays different text modifications
      for(y = 1; y < 10; y++)
         printf("  \033[0;40m\033[%i;37;48m%i;37;48m\033[0;40m ", y, y);
      printf(" \033[0m\n\033[40m");

      // adds row of spaces
      for (x = 0; x < 9; x++)
         printf(" \033[30;40m          ");
      printf(" \033[0m\n\033[40m");
   };

   printf("\033[0m\n");

   return(0);
}

/* end of source file */
