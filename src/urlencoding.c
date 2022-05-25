/*
 *  DMS Tools and Utilities
 *  Copyright (C) 2022 David M. Syzdek <david@syzdek.net>.
 *  All rights reserved.
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
 */
#define _SRC_URLENCODING_C 1

///////////////
//           //
//  Headers  //
//           //
///////////////

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <stdarg.h>
#include <getopt.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netdb.h>

#include <bindle_prefix.h>


///////////////////
//               //
//  Definitions  //
//               //
///////////////////

#ifndef PROGRAM_NAME
#define PROGRAM_NAME "urlencoding"
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

#define MY_OPT_ENCODE         0x00000001UL
#define MY_OPT_DECODE         0x00000002UL
#define MY_OPT_VERBOSE        0x00010000UL
#define MY_OPT_QUIET          0x00020000UL


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////

int
main(
         int                           argc,
         char *                        argv[] );


static int
my_error(
         unsigned                      opts,
         const char *                  fmt,
         ... );


/////////////////
//             //
//  Functions  //
//             //
/////////////////

int
main(
         int                           argc,
         char *                        argv[] )
{
   int               c;
   int               opt_index;
   unsigned          opts;
   char *            buff;
   void *            ptr;
   size_t            size;

   // getopt options
   static char          short_opt[] = "dehVvqs";
   static struct option long_opt[] =
   {
      {"decode",           no_argument,       NULL, 'd' },
      {"encode",           no_argument,       NULL, 'e' },
      {"help",             no_argument,       NULL, 'h' },
      {"quiet",            no_argument,       NULL, 'q' },
      {"silent",           no_argument,       NULL, 'q' },
      {"version",          no_argument,       NULL, 'V' },
      {"verbose",          no_argument,       NULL, 'v' },
      { NULL, 0, NULL, 0 }
   };

   opts = 0;

   while((c = getopt_long(argc, argv, short_opt, long_opt, &opt_index)) != -1)
   {
      switch(c)
      {
         case -1:       /* no more arguments */
         case 0:        /* long options toggles */
         break;

         case 'd':
         opts |=  MY_OPT_DECODE;
         opts &= ~MY_OPT_ENCODE;
         break;

         case 'e':
         opts |=  MY_OPT_ENCODE;
         opts &= ~MY_OPT_DECODE;
         break;

         case 'h':
         printf("Usage: %s [OPTIONS] string1 [ string2 [ ... [ stringN ] ] ]\n", PROGRAM_NAME);
         printf("OPTIONS:\n");
         printf("  -d, --decode              decode string\n");
         printf("  -e, --encode              encode string\n");
         printf("  -h, --help                print this help and exit\n");
         printf("  -q, --quiet, --silent     do not print messages\n");
         printf("  -V, --version             print version number and exit\n");
         printf("  -v, --verbose             print verbose messages\n");
         printf("\n");
         return(0);

         case 'q':
         opts |=  MY_OPT_QUIET;
         opts &= ~MY_OPT_VERBOSE;
         break;

         case 'V':
         printf("%s (%s) %s\n", PACKAGE_NAME, PACKAGE_NAME, PACKAGE_VERSION);
         return(0);

         case 'v':
         opts |=  MY_OPT_VERBOSE;
         opts &= ~MY_OPT_QUIET;
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

   if (optind == argc)
   {
      fprintf(stderr, "%s: missing required argument\n", PROGRAM_NAME);
      fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
      return(1);
   };

   // loop through URLs
   buff = NULL;
   while (optind < argc)
   {
      if ((opts & MY_OPT_DECODE))
      {
         size = strlen(argv[optind]) + 1;
         if ((ptr = realloc(buff, size)) == NULL)
         {
            my_error(opts, "out of virtual memory");
            free(buff);
            return(1);
         };
         buff = ptr;
         dmstools_url_decode(argv[optind], buff, size);
         printf("%s\n", buff);
      } else
      {
         size = dmstools_url_encode_len(argv[optind]);
         if ((ptr = realloc(buff, size)) == NULL)
         {
            my_error(opts, "out of virtual memory");
            free(buff);
            return(1);
         };
         buff = ptr;
         dmstools_url_encode(argv[optind], buff, size);
         printf("%s\n", buff);
      };
      optind++;
   };

   free(buff);

   return(0);
}


int
my_error(
         unsigned                      opts,
         const char *                  fmt,
         ... )
{
   va_list args;
   if ((opts & MY_OPT_QUIET))
      return(1);
   fprintf(stderr, "%s: ", PROGRAM_NAME);
   va_start(args, fmt);
   vfprintf(stderr, fmt, args);
   va_end(args);
   fprintf(stderr, "\n");
   return(1);
}


// end of source file
