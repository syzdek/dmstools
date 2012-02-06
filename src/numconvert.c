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
   char      buff[9];
   int32_t   opt;
   intmax_t  x;
   intmax_t  y;
   intmax_t  z;
   uintmax_t len;
   uintmax_t num;
   uintmax_t max;
   uintmax_t byte;

   // getopt options
   static char   short_opt[] = "AaBbDdhlOoRrsVXx";
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
         case 'A':
            opt |= MY_OPT_ASCII;
            break;
         case 'a':
            base = -1;
            //opt |= (MY_OPT_BIN|MY_OPT_DEC|MY_OPT_HEX|MY_OPT_OCT|);
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

   if (!(opt & (MY_OPT_BIN|MY_OPT_DEC|MY_OPT_HEX|MY_OPT_OCT|MY_OPT_ASCII)))
      opt |= (MY_OPT_BIN|MY_OPT_DEC|MY_OPT_HEX|MY_OPT_OCT|MY_OPT_ASCII);

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
         if (!(strncmp("00", argv[x], (size_t)2)))
            base = 2;
         else if (!(strncmp("0x", argv[x], (size_t)2)))
            base = 16;
         else if (!(strncmp("0", argv[x], (size_t)1)))
            base = 8;
         else
            for(y = 0; ((uint32_t)y) < strlen(argv[x]); y++)
               if ( ((argv[x][y] >= 'a') && (argv[x][y] <= 'z')) ||
                    ((argv[x][y] >= 'A') && (argv[x][y] <= 'Z')) )
                  base = 16;
      };
      max = sizeof(intmax_t);
      if (base == -1)
         num = argv[x][0];
      else
         num = strtoumax(argv[x], NULL, base);
      if (opt & MY_OPT_LIMIT)
      {
         max = 1;
         for(y = 0; y < (intmax_t)sizeof(intmax_t); y++)
            if ( (num >> (y*8)) & 0xFF )
               max = (y+1);
      };
      if (opt & MY_OPT_ASCII) len = printf((len ? ((opt & MY_OPT_SPACE) ? ", '%c'" : ",'%c'") : "'%c'"), ((((num & 0xFF) >= 32) && ((num & 0xFF) <= 126)) ? (char)(num & 0xFF) : '.'));
      if (opt & MY_OPT_BIN)
      {
         if (len) printf((opt & MY_OPT_SPACE) ? ", " : ",");
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
      };
      if (opt & MY_OPT_HEX) len = printf((len ? ((opt & MY_OPT_SPACE) ? ", 0x%0*jX" : ",0x%0*jX") : "0x%0*jX"), (unsigned)(max*2), num);
      if (opt & MY_OPT_OCT) len = printf((len ? ((opt & MY_OPT_SPACE) ? ", 0%jo"    : ",0%jo")    : "0%jo"),    num);
      if (opt & MY_OPT_DEC) len = printf((len ? ((opt & MY_OPT_SPACE) ? ", %ju"     : ",%ju")     : "%ju"),     num);
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
         "  -A                        enable ASCII output\n"
         "  -a                        assume binary notation for input\n"
         "  -B                        enable binary output\n"
         "  -b                        assume binary notation for input\n"
         "  -D                        enable decimal output\n"
         "  -d                        assume decimal notation for input\n"
         "  -h, --help                print this help and exit\n"
         "  -l                        print minimum number of bytes in hex and binary\n"
         "  -O                        enable octal output\n"
         "  -o                        assume octal notation for input\n"
         "  -R                        display binary in little endian byte order\n"
         "  -r                        display binary in little endian bit nad byte order\n"
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
