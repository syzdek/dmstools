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
#define _SCT_SRC_DMSTOOLS_C 1

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
#define PROGRAM_NAME "urldesc"
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

#define MY_OPT_SCHEME         0x00000001UL
#define MY_OPT_USERINFO       0x00000002UL
#define MY_OPT_HOST           0x00000004UL
#define MY_OPT_PORT           0x00000008UL
#define MY_OPT_PATH           0x00000010UL
#define MY_OPT_QUERY          0x00000020UL
#define MY_OPT_FRAGMENT       0x00000040UL
#define MY_OPT_ADDRESS        0x00000080UL
#define MY_OPT_SERVICE        0x00000100UL
#define MY_OPT_FIELDS         0x000001FFUL

#define MY_OPT_VERBOSE        0x00010000UL
#define MY_OPT_QUIET          0x00020000UL
#define MY_OPT_NORESOLVE      0x00040000UL
#define MY_OPT_SHORT          0x00080000UL
#define MY_OPT_IPV4           0x00100000UL
#define MY_OPT_IPV6           0x00200000UL


/////////////////
//             //
//  Datatypes  //
//             //
/////////////////


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


static int
my_parse_url(
         unsigned                      opts,
         const char *                  url );


static void
my_print_detail(
         unsigned                      opts,
         const char *                  url,
         BindleURLDesc *               budp );


static void
my_print_detail_field(
         unsigned                      opts,
         unsigned                      opt,
         const char *                  fld,
         const char *                  val );


static void
my_print_short(
         unsigned                      opts,
         BindleURLDesc *               budp );


static void
my_print_url(
         unsigned                      opts,
         BindleURLDesc *               budp );


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
   int               opt;
   int               rc;
   unsigned          opts;

   // getopt options
   static char          short_opt[] = "46hnVvqs";
   static struct option long_opt[] =
   {
      {"debug",            no_argument,       NULL, 'd' },
      {"help",             no_argument,       NULL, 'h' },
      {"quiet",            no_argument,       NULL, 'q' },
      {"silent",           no_argument,       NULL, 'q' },
      {"short",            no_argument,       NULL, 's' },
      {"version",          no_argument,       NULL, 'V' },
      {"verbose",          no_argument,       NULL, 'v' },
      {"scheme",           no_argument,       NULL, MY_OPT_SCHEME },
      {"userinfo",         no_argument,       NULL, MY_OPT_USERINFO },
      {"host",             no_argument,       NULL, MY_OPT_HOST },
      {"service",          no_argument,       NULL, MY_OPT_SERVICE },
      {"port",             no_argument,       NULL, MY_OPT_PORT },
      {"path",             no_argument,       NULL, MY_OPT_PATH },
      {"query",            no_argument,       NULL, MY_OPT_QUERY },
      {"fragment",         no_argument,       NULL, MY_OPT_FRAGMENT },
      {"address",          no_argument,       NULL, MY_OPT_ADDRESS },
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

         case MY_OPT_SCHEME:
         case MY_OPT_USERINFO:
         case MY_OPT_HOST:
         case MY_OPT_PORT:
         case MY_OPT_SERVICE:
         case MY_OPT_PATH:
         case MY_OPT_QUERY:
         case MY_OPT_FRAGMENT:
         case MY_OPT_ADDRESS:
         opts |= c;
         break;

         case '4':
         opts |= MY_OPT_IPV4;
         opts &= ~MY_OPT_IPV6;
         break;

         case '6':
         opts |= MY_OPT_IPV6;
         opts &= ~MY_OPT_IPV4;
         break;

         case 'd':
         opt = BNDL_DEBUG_ANY;
         dmstools_set_option(NULL, BNDL_OPT_DEBUG_LEVEL,  &opt);
         break;

         case 'h':
         printf("Usage: %s [OPTIONS] url1 [ url2 [ ... [ urlN ] ] ]\n", PROGRAM_NAME);
         printf("OPTIONS:\n");
         printf("  -4                        use IPv4 addresses only\n");
         printf("  -6                        use IPv6 addresses only\n");
         printf("  -d, --debug               print debug messages\n");
         printf("  -h, --help                print this help and exit\n");
         printf("  -n                        do not resolve hostname\n");
         printf("  -q, --quiet, --silent     do not print messages\n");
         printf("  -V, --version             print version number and exit\n");
         printf("  -v, --verbose             print verbose messages\n");
         printf("  --scheme                  print URL scheme\n");
         printf("  --userinfo                print URL user info\n");
         printf("  --host                    print URL host\n");
         printf("  --service                 print URL service\n");
         printf("  --port                    print URL port\n");
         printf("  --path                    print URL path\n");
         printf("  --query                   print URL query\n");
         printf("  --fragment                print URL fragment\n");
         printf("  --address                 print URL address\n");
         printf("\n");
         return(0);

         case 'n':
         opts |= MY_OPT_NORESOLVE;
         break;

         case 'q':
         opts |=  MY_OPT_QUIET;
         opts &= ~MY_OPT_VERBOSE;
         break;

         case 's':
         opts |= MY_OPT_SHORT;
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
   while (optind < argc)
   {
      if ((rc = my_parse_url(opts, argv[optind])) != 0)
         return(rc);
      optind++;
   };

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


int
my_parse_url(
         unsigned                      opts,
         const char *                  url )
{
   int                  rc;
   int                  family;
   BindleURLDesc *      budp;

   if ((rc = dmstools_urldesc_parse(url, &budp)) != 0)
   {
      my_error(opts, "dmstools_urldesc_parse(): %s", strerror(rc));
      return(1);
   };

   if (!(opts & MY_OPT_NORESOLVE))
   {
      family = ((opts & MY_OPT_IPV4)) ? AF_INET  : 0;
      family = ((opts & MY_OPT_IPV6)) ? AF_INET6 : family;
      if ((rc = dmstools_urldesc_resolve(budp, family)) != 0)
      {
         my_error(opts, "dmstools_urldesc_resolve(): %s", gai_strerror(rc));
         dmstools_urldesc_free(budp);
         return(2);
      };
   };

   if ((opts & MY_OPT_QUIET))
   {
      dmstools_urldesc_free(budp);
      return(0);
   };

   if ((opts & MY_OPT_SHORT))
   {
      my_print_short(opts, budp);
   } else if ((opts & MY_OPT_VERBOSE))
   {
      if (!(opts & MY_OPT_FIELDS))
         opts |= MY_OPT_FIELDS;
      my_print_detail(opts, url, budp);
   } else
   {
      if (!(opts & MY_OPT_FIELDS))
         opts |= MY_OPT_FIELDS;
      my_print_url(opts, budp);
   };

   dmstools_urldesc_free(budp);

   return(0);
}


void
my_print_detail(
         unsigned                      opts,
         const char *                  url,
         BindleURLDesc *               budp )
{
   int                     pos;
   char                    buff[INET6_ADDRSTRLEN+INET_ADDRSTRLEN];
   struct sockaddr **      sas;
   struct sockaddr_in      sa;
   struct sockaddr_in6     sa6;

   printf("%s\n", url);
   my_print_detail_field(opts, MY_OPT_SCHEME,   "Scheme:",     budp->bud_scheme);
   my_print_detail_field(opts, MY_OPT_USERINFO, "User Info:",  budp->bud_userinfo);
   my_print_detail_field(opts, MY_OPT_HOST,     "Host:",       budp->bud_host);
   if ((budp->bud_sockaddrs))
   {
      sas = (struct sockaddr **)budp->bud_sockaddrs;
      for(pos = 0; ((sas[pos])); pos++)
      {
         if (sas[pos]->sa_family == AF_INET)
         {
            memcpy(&sa, sas[pos], sizeof(sa));
            inet_ntop(AF_INET, &sa.sin_addr, buff, sizeof(buff));
         } else {
            memcpy(&sa6, sas[pos], sizeof(sa6));
            inet_ntop(AF_INET6, &sa6.sin6_addr, buff, sizeof(buff));
         };
         my_print_detail_field(opts, MY_OPT_ADDRESS, ((!(pos)) ? "Address:" : ""), buff);
      };
   };
   if ((budp->bud_port))
   {
      snprintf(buff, sizeof(buff), "%u", (unsigned)budp->bud_port);
      my_print_detail_field(opts, MY_OPT_PORT,  "Port:",    buff);
   };
   my_print_detail_field(opts, MY_OPT_SERVICE,  "Service:",    budp->bud_service);
   my_print_detail_field(opts, MY_OPT_PATH,     "Path:",       budp->bud_path);
   my_print_detail_field(opts, MY_OPT_QUERY,    "Query:",      budp->bud_query);
   my_print_detail_field(opts, MY_OPT_FRAGMENT, "Fragment:",   budp->bud_fragment);
   printf("\n");

   return;
}


void
my_print_detail_field(
         unsigned                      opts,
         unsigned                      opt,
         const char *                  fld,
         const char *                  val )
{
   if (!(opts & opt))
      return;
   if (!(val))
      return;
   printf("   %-15s %s\n", fld, val);
   return;
}


void
my_print_short(
         unsigned                      opts,
         BindleURLDesc *               budp )
{
   int                     family;
   int                     pos;
   char                    addr[INET6_ADDRSTRLEN+INET_ADDRSTRLEN];
   struct sockaddr **      sas;
   struct sockaddr_in      sa;
   struct sockaddr_in6     sa6;

   if ( ((opts & MY_OPT_SCHEME))   && ((budp->bud_scheme)) )   printf("%s\n", budp->bud_scheme);
   if ( ((opts & MY_OPT_USERINFO)) && ((budp->bud_userinfo)) ) printf("%s\n", budp->bud_userinfo);
   if ( ((opts & MY_OPT_HOST))     && ((budp->bud_host)) )     printf("%s\n", budp->bud_host);
   if ( ((opts & MY_OPT_SERVICE))  && ((budp->bud_service)) )  printf("%s\n", budp->bud_service);
   if ( ((opts & MY_OPT_PATH))     && ((budp->bud_path)) )     printf("%s\n", budp->bud_path);
   if ( ((opts & MY_OPT_QUERY))    && ((budp->bud_query)) )    printf("%s\n", budp->bud_query);
   if ( ((opts & MY_OPT_FRAGMENT)) && ((budp->bud_fragment)) ) printf("%s\n", budp->bud_fragment);

   if ( ((opts & MY_OPT_PORT)) && ((budp->bud_port)) )
      printf("%u\n", (unsigned)budp->bud_port);

   if (!(opts & MY_OPT_ADDRESS))
      return;
   if (!(budp->bud_sockaddrs))
      return;

   family = ((opts & MY_OPT_IPV4)) ? AF_INET  : AF_UNSPEC;
   family = ((opts & MY_OPT_IPV6)) ? AF_INET6 : family;
   sas    = (struct sockaddr **)budp->bud_sockaddrs;

   for(pos = 0; ((sas[pos])); pos++)
   {
      if ((sas[pos]->sa_family == AF_INET6) && ((family == AF_INET6)||(family == AF_UNSPEC)))
      {
         memcpy(&sa6, sas[pos], sizeof(sa6));
         inet_ntop(AF_INET6, &sa6.sin6_addr, addr, sizeof(addr));
         printf("%s\n", addr);
         return;
      };
   };
   for(pos = 0; ((sas[pos])); pos++)
   {
      if ((sas[pos]->sa_family == AF_INET) && ((family == AF_INET)||(family == AF_UNSPEC)))
      {
         memcpy(&sa, sas[pos], sizeof(sa));
         inet_ntop(AF_INET, &sa.sin_addr, addr, sizeof(addr));
         printf("%s\n", addr);
         return;
      };
   };

   return;
}


void
my_print_url(
         unsigned                      opts,
         BindleURLDesc *               budp )
{
   BindleURLDesc           bud;
   char                    port[16];
   int                     family;
   int                     pos;
   char                    addr[INET6_ADDRSTRLEN+INET_ADDRSTRLEN];
   struct sockaddr **      sas;
   struct sockaddr_in      sa;
   struct sockaddr_in6     sa6;
   char *                  url;

   memset(&bud, 0, sizeof(bud));

   if ( ((opts & MY_OPT_PORT)) && ((budp->bud_port)) )
   {
      snprintf(port, sizeof(port), "%u", (unsigned)budp->bud_port);
      bud.bud_service = port;
   };

   if ( ((opts & MY_OPT_ADDRESS)) && ((budp->bud_sockaddrs)) )
   {
      family = ((opts & MY_OPT_IPV4)) ? AF_INET  : AF_UNSPEC;
      family = ((opts & MY_OPT_IPV6)) ? AF_INET6 : family;
      sas    = (struct sockaddr **)budp->bud_sockaddrs;
      for(pos = 0; ((sas[pos])); pos++)
      {
         if ((sas[pos]->sa_family == AF_INET6) && ((family == AF_INET6)||(family == AF_UNSPEC)))
         {
            family = AF_INET6;
            memcpy(&sa6, sas[pos], sizeof(sa6));
            inet_ntop(AF_INET6, &sa6.sin6_addr, addr, sizeof(addr));
         };
         if ((sas[pos]->sa_family == AF_INET) && ((family == AF_INET)||(family == AF_UNSPEC)))
         {
            memcpy(&sa, sas[pos], sizeof(sa));
            inet_ntop(AF_INET, &sa.sin_addr, addr, sizeof(addr));
         };
         bud.bud_host = addr;
      };
   };

   if ((opts & MY_OPT_SCHEME))      bud.bud_scheme    = budp->bud_scheme;
   if ((opts & MY_OPT_USERINFO))    bud.bud_userinfo  = budp->bud_userinfo;
   if ((opts & MY_OPT_HOST))        bud.bud_host      = budp->bud_host;
   if ((opts & MY_OPT_SERVICE))     bud.bud_service   = budp->bud_service;
   if ((opts & MY_OPT_PATH))        bud.bud_path      = budp->bud_path;
   if ((opts & MY_OPT_QUERY))       bud.bud_query     = budp->bud_query;
   if ((opts & MY_OPT_FRAGMENT))    bud.bud_fragment  = budp->bud_fragment;

   if ((url = dmstools_urldesc2str(&bud)) == NULL)
   {
      my_error(opts, "out of virtual memory");
      return;
   };
   printf("%s\n", url);
   free(url);

   return;
}


// end of source file
