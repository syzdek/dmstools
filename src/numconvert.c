/*
 *  DMS Tools and Utilities
 *  Copyright (C) 2009 David M. Syzdek <david@syzdek.net>.
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

#define MY_TOGGLE(value, bit) ( (value&bit) ? (value&(~bit)) : (value|bit))


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////

// main statement
int main(int argc, char * argv[]);

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
   int       c;
   int       base;
   int       opt_index;
   int32_t   opt;
   intmax_t  x;
   intmax_t  y;
   uintmax_t len;
   uintmax_t num;
   uintmax_t byte;

   // getopt options
   static char   short_opt[] = "aBbDdhOosVXx";
   static struct option long_opt[] =
   {
      {"help",          no_argument, 0, 'h'},
      {"version",       no_argument, 0, 'V'},
      {NULL,            0,           0, 0  }
   };

   opt    = MY_OPT_SPACE;
   base   = 0;

   while((c = getopt_long(argc, argv, short_opt, long_opt, &opt_index)) != -1)
   {
      switch(c)
      {
         case -1:	/* no more arguments */
         case 0:	/* long options toggles */
            break;
         case 'a':
            opt |= (MY_OPT_BIN|MY_OPT_DEC|MY_OPT_HEX|MY_OPT_OCT);
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
         case 'h':
            my_usage();
            return(0);
         case 'O':
            opt |= MY_OPT_OCT;
            break;
         case 'o':
            base = 8;
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
            fprintf(stderr, _("Try `%s --help' for more information.\n"), PROGRAM_NAME);
            return(1);
         default:
            fprintf(stderr, _("%s: unrecognized option `--%c'\n"
                  "Try `%s --help' for more information.\n"
               ),  PROGRAM_NAME, c, PROGRAM_NAME
            );
            return(1);
      };
   };

   if (!(opt & (MY_OPT_BIN|MY_OPT_DEC|MY_OPT_HEX|MY_OPT_OCT)))
      opt |= (MY_OPT_BIN|MY_OPT_DEC|MY_OPT_HEX|MY_OPT_OCT);

   if ((argc - optind) < 1)
   {
      fprintf(stderr, _("%s: missing required argument\n"
                        "Try `%s --help' for more information.\n"),
                        PROGRAM_NAME, PROGRAM_NAME);
      return(1);
   };

   for(x = optind; x < argc; x++)
   {
      len = 0;
      if (!(base))
      {
         base = 10;
         if (!(strncmp("00", argv[x], 2)))
            base = 2;
         else if (!(strncmp("0x", argv[x], 2)))
            base = 16;
         else if (!(strncmp("0", argv[x], 1)))
            base = 8;
         else
            for(y = 0; y < strlen(argv[x]); y++)
               if ( ((argv[x][y] >= 'a') && (argv[x][y] <= 'z')) ||
                    ((argv[x][y] >= 'A') && (argv[x][y] <= 'Z')) )
                  base = 16;
      };
      num = strtoumax(argv[x], NULL, base);
      if (opt & MY_OPT_OCT) len = printf((len ? ",0%jo"    : "0%jo"),    num);
      if (opt & MY_OPT_DEC) len = printf((len ? ",%ju"     : "%ju"),     num);
      if (opt & MY_OPT_HEX) len = printf((len ? ",0x%0*jX" : "0x%0*jX"), (unsigned)(sizeof(uintmax_t)*2), num);
      if (opt & MY_OPT_BIN)
      {
         if (len) printf(",");
         for(y = 0; y < sizeof(uintmax_t); y++)
         {
            byte = num >> (8*(sizeof(uintmax_t)-y-1));
            printf("%c%c%c%c%c%c%c%c",
                  (byte & 0x80) ? '1' : '0',
                  (byte & 0x40) ? '1' : '0',
                  (byte & 0x20) ? '1' : '0',
                  (byte & 0x10) ? '1' : '0',
                  (byte & 0x08) ? '1' : '0',
                  (byte & 0x04) ? '1' : '0',
                  (byte & 0x02) ? '1' : '0',
                  (byte & 0x01) ? '1' : '0');
            if (opt & MY_OPT_SPACE) printf(" ");
         };
      };
      printf("\n");
   };

   return(0);
}


/// displays usage information
void my_usage()
{
   // TRANSLATORS: The following strings provide usage for command. These
   // strings are displayed if the program is passed `--help' on the command
   // line. The two strings referenced are: PROGRAM_NAME, and
   // PACKAGE_BUGREPORT
   printf(_("Usage: %s num1 num2 ... numX\n"
         "  -B                        enable binary output\n"
         "  -b                        assume binary notation for input\n"
         "  -D                        enable decimal output\n"
         "  -d                        assume decimal notation for input\n"
         "  -h, --help                print this help and exit\n"
         "  -O                        enable octal output\n"
         "  -o                        assume octal notation for input\n"
         "  -s                        display binary value in 8 bit blocks\n"
         "  -X                        enable hexadecimal output\n"
         "  -x                        assume hexadecimal notation for input\n"
         "  -V, --version             print version number and exit\n"
         "\n"
         "Input Notation:\n"
         "  NNNNNNNN                  input number is in decimal\n"
         "  0xNNNNNNNN                input number is in hexadecimal\n"
         "  0NNNNNNNN                 input number is in octal\n"
         "  00NNNNNNNN                input number is in binary\n"
         "\n"
         "Report bugs to <%s>.\n"
      ), PROGRAM_NAME, PACKAGE_BUGREPORT
   );
   return;
}


/// displays version information
void my_version(void)
{
   // TRANSLATORS: The following strings provide version and copyright
   // information if the program is passed --version on the command line.
   // The three strings referenced are: PROGRAM_NAME, PACKAGE_NAME,
   // PACKAGE_VERSION.
   printf(_("%s (%s) %s\n"
         "Written by David M. Syzdek.\n"
         "\n"
         "%s\n"
         "This is free software; see the source for copying conditions.  There is NO\n"
         "warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n"
      ), PROGRAM_NAME, PACKAGE_NAME, PACKAGE_VERSION, PACKAGE_COPYRIGHT
   );
   return;
}

/* end of source code */
