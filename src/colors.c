/*
 *  DMS Tools and Utilities
 *  Copyright (C) 2010 David M. Syzdek <david@syzdek.net>.
 *
 *  @SYZDEK_LICENSE_HEADER_START@
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License Version 2 as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 *  @SYZDEK_LICENSE_HEADER_END@
 */
/**
 *  @file src/colors.c displays various colors using ANSI codes
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


///////////////////
//               //
//  i18l Support //
//               //
///////////////////

#ifdef HAVE_GETTEXT
#   include <gettext.h>
#   include <libintl.h>
#   define _(String) gettext (String)
#   define gettext_noop(String) String
#   define N_(String) gettext_noop (String)
#else
#   define _(String) (String)
#   define N_(String) String
#   define textdomain(Domain)
#   define bindtextdomain(Package, Directory)
#endif


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

// main statement
int main PARAMS((void));


/////////////////
//             //
//  Functions  //
//             //
/////////////////

/// main statement
/// @param[in]  argc  number of arguments passed to program
/// @param[in]  argv  array of arguments passed to program
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

/* end of source file */
