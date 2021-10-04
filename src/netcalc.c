/*
 *  DMS Tools and Utilities
 *  Copyright (C) 2011 David M. Syzdek <david@syzdek.net>.
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
 *  @file src/netcalc.c calculates network information for IP addresses
 */
/*
 *  Simple Build:
 *     gcc -W -Wall -O2 -c netcalc.c
 *     gcc -W -Wall -O2 -o netcalc   netcalc.o
 *
 *  GNU Libtool Build:
 *     libtool --mode=compile gcc -W -Wall -g -O2 -c netcalc.c
 *     libtool --mode=link    gcc -W -Wall -g -O2 -o netcalc netcalc.lo
 *
 *  GNU Libtool Install:
 *     libtool --mode=install install -c netcalc /usr/local/bin/netcalc
 *
 *  GNU Libtool Clean:
 *     libtool --mode=clean rm -f netcalc.lo netcalc
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
#include <arpa/inet.h>
#include <string.h>
#include <inttypes.h>
#include <stdlib.h>


///////////////////
//               //
//  Definitions  //
//               //
///////////////////

#ifndef PROGRAM_NAME
#define PROGRAM_NAME "netcalc"
#endif
#ifndef PACKAGE_BUGREPORT
#define PACKAGE_BUGREPORT "david@syzdek.net"
#endif
#ifndef PACKAGE_COPYRIGHT
#define PACKAGE_COPYRIGHT "Copyright David M. Syzdek"
#endif
#ifndef PACKAGE_NAME
#define PACKAGE_NAME "dmstools"
#endif
#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION ""
#endif

#ifndef PARAMS
#define PARAMS(protos) protos
#endif


#define NETCALC_OPT_ALL_NETWORKS          0x0001


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////

// converts IPv4 to string
const char * netcalc_ipv4tostr(uint64_t ipv4);

// tests if string represents an IP address
int netcalc_is_ipv4 PARAMS((const char * str, uint64_t * ipp, uint64_t * cidrp));

// displays usage
void netcalc_usage PARAMS((void));

// displays version information
void netcalc_version PARAMS((void));

// main statement
int main PARAMS((int argc, char * argv[]));

char buff[1024];


/////////////////
//             //
//  Functions  //
//             //
/////////////////

/// converts IPv4 to string
const char * netcalc_ipv4tostr(uint64_t ipv4)
{
   int c;
   int offset;
   offset = 0;
   for(c = 0; c < 4; c++)
      offset += snprintf(&buff[offset], 1024-offset, "%" PRIu64 ".", ((ipv4 >> (24 - (8*c)))&0xff) );
   buff[offset-1] = '\0';
   return(buff);
}


/// tests if string represents an IP address
int netcalc_is_ipv4(const char * str, uint64_t * ipv4_ptr, uint64_t * cidrp)
{
   int            c;
   int            value;
   uint64_t       ipv4;
   char         * ptr;

   // reset IP address buffer
   if ((ipv4_ptr))
      *ipv4_ptr = 0;

   // reset initial IP address
   ipv4 = 0;
   ptr  = NULL;

   // loops through octets
   for(c = 0; c < 4; c++)
   {
      // reads number from string
      ptr = NULL;
      value = (int)strtol(str, &ptr, 10);

      // exits if string is not a number or IP address
      if (!(ptr))
         return(1);
      if ((ptr[0] != '.') && (ptr[0] != '\0') && (ptr[0] != '/'))
         return(1);

      // exits if number is out of bounds for IPv4 octet
      if ((value < 0) || (value > 255))
         return(1);

      // adds number to IPv4 address and moves position within string
      ipv4 = ((ipv4 << 8) | (((uint64_t)value) & 0xFF));
      str = &ptr[1];

      // finishes up if at end of string
      if ((ptr[0] == '\0') || (ptr[1] == '\0'))
      {
         ipv4 = ipv4 << ((3 - c) * 8);
         if ((ipv4_ptr))
            *ipv4_ptr = ipv4;
         return(0);
      };

      // process CIDR
      if (ptr[0] == '/')
      {
         value = (int)strtol(&ptr[1], &ptr, 10);
         if (!(ptr))
            return(1);
         if (ptr[0] != '\0')
            return(1);
         if ((value < 0) || (value > 32))
            return(1);
         if ((cidrp))
            *cidrp = value;
         return(0);
      };
   };

   // if we made it here, then the string is not an IPv4 address
   return(1);
}


/// displays usage
void netcalc_usage(void)
{
   printf("Usage: %s [OPTIONS] address1 address2 ... addressN\n", PROGRAM_NAME);
   printf("  -a                        display all matching networks\n");
   printf("  -c num                    requested network size in CIDR notation\n");
   printf("  -h, --help                print this help and exit\n");
   printf("  -i num                    requested network size in amount of IP addresses\n");
   printf("  -V, --version             print version number and exit\n");
   printf("\n");
   printf("Report bugs to <%s>.\n", PACKAGE_BUGREPORT);
   return;
}


/// displays version information
void netcalc_version(void)
{
   printf("%s (%s) %s\n", PROGRAM_NAME, PACKAGE_NAME, PACKAGE_VERSION);
   printf("Written by David M. Syzdek.\n");
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
   int           opt;
   int           opt_index;
   uint64_t      cidr;
   uint64_t      opt_cidr_high;
   uint64_t      opt_ip_count;
   uint64_t      ipv4;
   uint64_t      ipv4_low;
   uint64_t      ipv4_high;
   uint64_t      network;
   uint64_t      broadcast;
   uint64_t      netmask;

   static char   short_opt[] = "ac:hi:V";
   static struct option long_opt[] =
   {
      {"help",          no_argument, 0, 'h'},
      {"version",       no_argument, 0, 'V'},
      {NULL,            0,           0, 0  }
   };

   // set defaults
   opt           = 0;
   opt_cidr_high = 33;
   opt_ip_count  = 0;

   while((c = getopt_long(argc, argv, short_opt, long_opt, &opt_index)) != -1)
   {
      switch(c)
      {
         case -1:	/* no more arguments */
         case 0:	/* long options toggles */
            break;
         case 'a':
            opt = opt | NETCALC_OPT_ALL_NETWORKS;
            break;
         case 'c':
            opt_cidr_high = strtoll(optarg, NULL, 0);
            break;
         case 'h':
            netcalc_usage();
            return(0);
         case 'i':
            opt_ip_count = strtoll(optarg, NULL, 0);
            break;
         case 'V':
            netcalc_version();
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

   if (optind >= argc)
   {
      fprintf(stderr, "%s: missing required argument\n", PROGRAM_NAME);
      fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
      return(1);
   };

   // determines the high and low IP address in the list
   ipv4_high = 0;
   ipv4_low  = 0;
   for(c = optind; c < argc; c++)
   {
      if (netcalc_is_ipv4(argv[c], &ipv4, &opt_cidr_high))
      {
         printf("%s: invalid IPv4 address: %s\n", PROGRAM_NAME, argv[c]);
         return(1);
      };
      if (c == optind)
      {
         ipv4_low  = ipv4;
         ipv4_high = ipv4;
      };
      if (ipv4 < ipv4_low)
         ipv4_low = ipv4;
      if (ipv4 > ipv4_high)
         ipv4_high = ipv4;
   };

   // calculates the smallest network which matches criteria
   cidr    = 32LL;
   netmask = 0x00;
   while ( ((~netmask & ipv4_high) != (~netmask & ipv4_low)) ||
           ((opt_cidr_high < cidr)) ||
           (((uint64_t)opt_ip_count > (netmask+1))) )
   {
      netmask = (netmask << 1) | 0x01;
      cidr--;
   };
   netmask = ~netmask;
   network = netmask & ipv4_high;

   // prints field names
   printf("%-18s", "Network");
   printf("%-8s", "CIDR");
   printf("%-18s", "Netmask");
   printf("%-18s", "Broadcast");
   printf("%s", "IP Count");
   printf("\n");

   // prints list of matching networks
   while(cidr >= 0)
   {
      // calculates network and broadcast
      network   = network & netmask;
      broadcast = network | (~netmask);

      // prints information
      printf("%-18s", netcalc_ipv4tostr(network));
      printf("%-8" PRIu64, cidr);
      printf("%-18s", netcalc_ipv4tostr(netmask));
      printf("%-18s", netcalc_ipv4tostr(broadcast));
      printf("%" PRIu64, (~netmask + 1));
      printf("\n");

      // moves to next netmask/cidr
      if (!(opt & NETCALC_OPT_ALL_NETWORKS))
         return(0);
      netmask = ~((~netmask << 1) | 0x01);
      cidr--;
   };

   // Okay, time to rest after a job well done
   return(0);
}

/* end of source file */
