/*
 *  Syzdek Coding Tools
 *  Copyright (c) 2008, 2009 David M. Syzdek <david@syzdek.net>.
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
 *  @file src/posixregex.c simple POSIX regex testing utility/example
 */
/*
 *  Simple Build:
 *     gcc -W -Wall -O2 -c posixregex.c
 *     gcc -W -Wall -O2 -o posixregex   posixregex.o
 *
 *  GNU Libtool Build:
 *     libtool --mode=compile gcc -W -Wall -g -O2 -c posixregex.c
 *     libtool --mode=link    gcc -W -Wall -g -O2 -o posixregex posixregex.lo
 *
 *  GNU Libtool Install:
 *     libtool --mode=install install -c posixregex /usr/local/bin/posixregex
 *
 *  GNU Libtool Clean:
 *     libtool --mode=clean rm -f posixregex.lo posixregex
 */
#define _SCT_SRC_POSIXREGEX_C 1

/////////////
//         //
//  Notes  //
//         //
/////////////

/*
 *  POSIX        ASCII           Description
 *  [:alnum:]    [A-Za-z0-9]     Alphanumeric characters
 *  [:alpha:]    [A-Za-z]        Alphabetic characters
 *  [:blank:]    [ \t]           Space and tab
 *  [:cntrl:]    [\x00-\x1F\x7F] Control characters
 *  [:digit:]    [0-9]           Digits
 *  [:graph:]    [\x21-\x7E]     Visible characters
 *  [:lower:]    [a-z]           Lowercase letters
 *  [:print:]    [\x20-\x7E]     Visible characters and spaces
 *  [:space:]    [ \t\r\n\v\f]   Whitespace characters
 *  [:upper:]    [A-Z]           Uppercase letters
 *  [:xdigit:]   [A-Fa-f0-9]     Hexadecimal digits
 *  [:punct:]    [-!"#$%&'()*+,./:;<=>?@[\\\]_`{|}~]   Punctuation characters
 */

//   Example Uses:
//      posixregex '^([[:alpha:]]+)://(([[:alnum:]]+)(:([[:alnum:]]+)){0,1}@){0,1}([-.a-z]+)(:([[:digit:]]+))*/([-/[:alnum:]]*)(\?(.*)){0,1}$'  http://jdoe:password@www.foo.org:123/path/to/file?query_string
//      posixregex '\$([[:digit:]]+)\.([[:digit:]]{2,2})' 'Your change is $7.45.'


///////////////
//           //
//  Headers  //
//           //
///////////////

#ifdef HAVE_CONFIG_H
#include <config.h>
#include "common.h"
#endif

#include <regex.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>


///////////////////
//               //
//  Definitions  //
//               //
///////////////////

#undef BUFFER_SIZE
#define BUFFER_SIZE 1024

#undef MAX_MATCHES
#define MAX_MATCHES 128

#ifndef PROGRAM_NAME
#define PROGRAM_NAME "posixregex"
#endif

#ifndef PACKAGE_NAME
#define PACKAGE_NAME ""
#endif

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION ""
#endif

#ifndef PACKAGE_BUGREPORT
#define PACKAGE_BUGREPORT "david@syzdek.net"
#endif

#ifndef PARAMS
#define PARAMS(protos) protos
#endif


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////

/* main statement */
int main PARAMS((int, char **));

/* displays POSIX regular expressions */
void my_posixregex PARAMS((void));

/* displays usage */
void my_usage PARAMS((void));

/* displays usage */
void my_version PARAMS((void));


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
   /* declare local vars */
   int          c;
   int          x;
   int          y;
   int          err;
   int          code;
   int          strlen;
   int          quiet;
   int          verbosity;
   int          opt_index;
   char         arg[BUFFER_SIZE];
   char         msg[BUFFER_SIZE];
   char         str[BUFFER_SIZE];
   regex_t      regex;
   regmatch_t   matches[MAX_MATCHES];
   const char * restr;

   /* getopt options */
   static char   short_opt[] = "hpqr:vV";
   static struct option long_opt[] =
   {
      {"help",          no_argument, 0, 'h'},
      {"posixregex",    no_argument, 0, 'p'},
      {"quiet",         no_argument, 0, 'q'},
      {"silent",        no_argument, 0, 'q'},
      {"verbose",       no_argument, 0, 'v'},
      {"version",       no_argument, 0, 'V'},
      {NULL,            0,           0, 0  }
   };

   /* initialize variables */
   code      = 0;
   restr     = NULL;
   quiet     = 0;
   verbosity = 0;
   opt_index = 0;

   /* loops through arguments */
   while((c = getopt_long(argc, argv, short_opt, long_opt, &opt_index)) != -1)
   {
      switch(c)
      {
         case -1:       /* no more arguments */
         case 0:        /* long options toggles */
            break;
         case 'h':
            my_usage();
            return(0);
         case 'p':
            my_posixregex();
            return(0);
         case 'q':
            quiet = 1;
            verbosity = 0;
            break;
         case 'r':
           restr = optarg;
           break;
         case 'V':
            my_version();
            return(0);
         case 'v':
            quiet = 0;
            verbosity++;
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

   if (!(restr))
      restr = getenv("POSIXREGEX");
   if (!(restr))
   {
      fprintf(stderr, "%s: missing required argument `-- r'\n", PROGRAM_NAME);
      fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
      return(1);
   };

   /* compile regular expression for later use */
   if (!(quiet))
      printf("regex: %s\n",restr);
   if ((err = regcomp(&regex, restr, REG_EXTENDED|REG_ICASE)))
   {
      regerror(err, &regex, msg, BUFFER_SIZE-1);
      printf("%s\n", msg);
      return(1);
   };

   /*
    *  Loops through command line arguments and tests the compiled expressions
    *  against each argument.
    */
   for (x = optind; x < argc; x++)
   {
      /* copies string into buffer and prints to screen */
      strncpy(arg, argv[x], BUFFER_SIZE-1);
      if (!(quiet))
         printf("%3i: %s  ==> ", x-optind+1, arg);

      /* tests the buffer against the regular expression */
      if ((err = regexec(&regex, arg, MAX_MATCHES, matches, 0)))
      {
         if (!(quiet))
            printf("not found\n");
         code = 1;
      } else {
         if (!(quiet))
            printf(" found\n");
         strlen = 1;

         /* copies sub matches in buffer string */
         if (verbosity)
         {
            for(y = 0; ((y < MAX_MATCHES) && (matches[y].rm_eo > -1)); y++)
            {
               memset(str, 0, BUFFER_SIZE);
               if ((strlen = matches[y].rm_eo - matches[y].rm_so))
               {
                  strncpy(str, &arg[matches[y].rm_so], strlen);
                  printf("     submatch %i: %s\n", y, str);
               } else if (matches[y].rm_eo > 0) {
                  printf("   submatch %i:\n", y);
               };
            };
            printf("\n");
         };
      };
   };

   /* free regular expression */
   regfree(&regex);

   /* end function */
   return(code);
}


/// displays POSIX regular expressions
void my_posixregex(void)
{
   printf("POSIX        ASCII           Description\n");
   printf("[:alnum:]    [A-Za-z0-9]     Alphanumeric characters\n");
   printf("[:alpha:]    [A-Za-z]        Alphabetic characters\n");
   printf("[:blank:]    [ \\t]           Space and tab\n");
   printf("[:cntrl:]    [\\x00-\\x1F\\x7F] Control characters\n");
   printf("[:digit:]    [0-9]           Digits\n");
   printf("[:graph:]    [\\x21-\\x7E]     Visible characters\n");
   printf("[:lower:]    [a-z]           Lowercase letters\n");
   printf("[:print:]    [\\x20-\\x7E]     Visible characters and spaces\n");
   printf("[:space:]    [ \\t\\r\\n\\v\\f]   Whitespace characters\n");
   printf("[:upper:]    [A-Z]           Uppercase letters\n");
   printf("[:xdigit:]   [A-Fa-f0-9]     Hexadecimal digits\n");
   printf("%s", "[:punct:]    [-!\"#$%&'()*+,./:;<=>?@[\\]_`{|}~]   Punctuation characters\n");
   printf("\n");
   printf("Example Uses:\n");
   printf("    posixregex -r '^([[:alpha:]]+)://(([[:alnum:]]+)(:([[:alnum:]]+)){0,1}@){0,1}([-.a-z]+)(:([[:digit:]]+))*/([-/[:alnum:]]*)(\\?(.*)){0,1}$'  http://jdoe:password@www.foo.org:123/path/to/file?query_string\n");
   printf("    posixregex -r '\\$([[:digit:]]+)\\.([[:digit:]]{2,2})' 'Your change is $7.45.'\n");
   return;
}


/// displays usage
void my_usage(void)
{
   printf("Usage: %s [OPTIONS] string1 string2 ... stringN\n", PROGRAM_NAME);
   printf("  -h, --help                print this help and exit\n");
   printf("  -p, --posixregex          print regular expression patterns\n");
   printf("  -q, --quiet, --silent     do not print messages\n");
   printf("  -r regex                  regular expression to use for testing strings\n");
   printf("  -v, --verbose             print verbose messages\n");
   printf("  -V, --version             print version number and exit\n");
   printf("Variables:\n");
   printf("  POSIXREGEX                default regular expression to use for testing strings\n");
   printf("\n");
#ifdef PACKAGE_BUGREPORT
   printf("Report bugs to <%s>.\n", PACKAGE_BUGREPORT);
#endif
   return;
}


/// displays version
void my_version(void)
{
   printf("%s (%s) %s\n", PROGRAM_NAME, PACKAGE_NAME, PACKAGE_VERSION);
   printf("Written by David M. Syzdek.\n");
   printf("\n");
#ifdef PACKAGE_COPYRIGHT
   printf("%s\n", PACKAGE_COPYRIGHT);
#endif
   printf("This is free software; see the source for copying conditions.  There is NO\n");
   printf("warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n");
   return;
}

/* end of source file */
