/*
 *  DMS Tools and Utilities
 *  Copyright (C) 2011, 2021 David M. Syzdek <david@syzdek.net>.
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

#include <assert.h>
#include <stdio.h>
#include <getopt.h>
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


#define NETCALC_ALL_NETWORKS          0x0001
#define NETCALC_IPV6_EXPAND           0x0002
#define NETCALC_IPV6_FULL             0x0004
#define NETCALC_NO_MAP                0x0008

#define NETCALC_INET                      4
#define NETCALC_INET6                     6


/////////////////
//             //
//  Datatypes  //
//             //
/////////////////

typedef struct _netcalc_ip netcalc_ip;
typedef struct _netcalc    netcalc;

struct _netcalc_ip
{
   uint32_t   addr[8];
};


struct _netcalc
{
   int32_t               cidr;
   int32_t               cidr_limit;
   int32_t               cidr_incr;
   int32_t               family;
   int32_t               display;
   int32_t               pad32;
   uint64_t              opts;
   size_t                count;
   netcalc_ip *          lower;
   netcalc_ip *          upper;
   int                   len_netmask;
   int                   len_broadcast;
   int                   len_network;
   int                   len_wildmask;
};


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////

int
netcalc_init(
         netcalc **                    cnfp );


void
netcalc_free(
         netcalc *                     cnfi );


int
netcalc_ip_cmp(
         netcalc_ip *                  ip1,
         netcalc_ip *                  ip2 );


void
netcalc_ip_free(
         netcalc_ip *                  ip );


int
netcalc_ip_parse(
         netcalc *                     cnf,
         netcalc_ip **                 ipp,
         const char *                  str );


int
netcalc_ip_parse_ipv4(
         netcalc *                     cnf,
         netcalc_ip *                  ip,
         char *                        str );


int
netcalc_ip_parse_ipv4_str(
         netcalc *                     cnf,
         netcalc_ip *                  ip,
         char *                        str );


int
netcalc_ip_parse_ipv6(
         netcalc *                     cnf,
         netcalc_ip *                  ip,
         char *                        str );


int netcalc_ip_string(
         netcalc *                     cnf,
         netcalc_ip *                  ip,
         char *                        str,
         size_t                        size );


int netcalc_ip_string_ipv4(
         netcalc *                     cnf,
         netcalc_ip *                  ip,
         char *                        str,
         size_t                        size );


int netcalc_ip_string_ipv6(
         netcalc *                     cnf,
         netcalc_ip *                  ip,
         char *                        str,
         size_t                        size );


int
netcalc_ip_parse_ipv6_mapped_ipv4(
         netcalc *                     cnf,
         netcalc_ip *                  ip,
         char *                        node );


void
netcalc_net_broadcast_r(
         netcalc_ip *                  broadcast,
         netcalc_ip *                  ip,
         int32_t                       cidr );


int
netcalc_net_cmp(
         netcalc_ip *                  ip1,
         netcalc_ip *                  ip2,
         int32_t                       cidr );


void
netcalc_net_netmask_r(
         netcalc_ip *                  netmask,
         int32_t                       cidr );


void
netcalc_net_network_r(
         netcalc_ip *                  network,
         netcalc_ip *                  ip,
         int32_t                       cidr );


void
netcalc_net_wildmask_r(
         netcalc_ip *                  wildmask,
         int32_t                       cidr );


void
netcalc_print_ip(
         netcalc *                     cnf,
         netcalc_ip *                  ip,
         int32_t                       cidr,
         int                           header );


// displays usage
void
netcalc_usage( void );


// displays version information
void
netcalc_version( void );


// main statement
int
main(
         int                           argc,
         char *                        argv[] );


char buff[1024];


/////////////////
//             //
//  Functions  //
//             //
/////////////////


void netcalc_free( netcalc * cnf )
{
   if (!(cnf))
      return;

   bzero(cnf, sizeof(netcalc));
   free(cnf);

   return;
}


int netcalc_init( netcalc ** cnfp )
{
   assert(cnfp != NULL);

   if ((*cnfp = malloc(sizeof(netcalc))) == NULL)
   {
      fprintf(stderr, "%s: out of virtual memory\n", PROGRAM_NAME);
      return(1);
   };
   bzero(*cnfp, sizeof(netcalc));

   (*cnfp)->cidr          = 128;
   (*cnfp)->cidr_limit    = -1;
   (*cnfp)->cidr_incr     = -1;
   (*cnfp)->family        = NETCALC_INET;
   (*cnfp)->display       = NETCALC_INET;

   (*cnfp)->len_netmask   = 7;
   (*cnfp)->len_network   = 7;
   (*cnfp)->len_wildmask  = 8;
   (*cnfp)->len_broadcast = 9;

   return(0);
}


int netcalc_ip_cmp( netcalc_ip * ip1, netcalc_ip * ip2 )
{
   int pos;
   assert(ip1 != NULL);
   assert(ip2 != NULL);
   for(pos = 0; ((pos < 7) && (ip1->addr[pos] == ip2->addr[pos])); pos++);
   if (ip1->addr[pos] < ip2->addr[pos])
      return(-1);
   if (ip1->addr[pos] > ip2->addr[pos])
      return(1);
   return(0);
}


void netcalc_ip_free( netcalc_ip * ip )
{
   if (!(ip))
      return;
   bzero(ip, sizeof(netcalc_ip));
   free(ip);
   return;
}


int netcalc_ip_parse( netcalc * cnf, netcalc_ip ** ipp, const char * ipstr )
{
   int             rc;
   netcalc_ip *    ip;
   char *          ptr;
   char *          endptr;
   int32_t         cidr;
   char *          str;

   assert(cnf != NULL);

   // allocate memory
   if ((ip = malloc(sizeof(netcalc_ip))) == NULL)
   {
      fprintf(stderr, "%s: out of virtual memory\n", PROGRAM_NAME);
      return(1);
   };
   if ((str = strdup(ipstr)) == NULL)
   {
      netcalc_ip_free(ip);
      fprintf(stderr, "%s: out of virtual memory\n", PROGRAM_NAME);
      return(1);
   };

   // parses CIDR
   cidr = -1;
   if ((ptr = strrchr(str, '/')) != NULL)
   {
      ptr[0]  = '\0';
      cidr    = (int32_t)strtol(&ptr[1], &endptr, 0);
      if ((endptr[0] != '\0') || (&ptr[1] == endptr))
      {
         fprintf(stderr, "%s: invalid CIDR\n", PROGRAM_NAME);
         netcalc_ip_free(ip);
         free(str);
         return(1);
      };
   };

   // parses string
   if ((ptr = strchr(str, ':')) == NULL)
   {
      // verifies cidr
      if ((cidr < -1) || (cidr > 32))
      {
         fprintf(stderr, "%s: invalid CIDR\n", PROGRAM_NAME);
         netcalc_ip_free(ip);
         free(str);
         return(1);
      };

      // normalizes CIDR for IPv6
      if (cidr >= 0)
         cidr += 128 - 32;

      // parse string
      if ((rc = netcalc_ip_parse_ipv4(cnf, ip, str)) != 0)
      {
         netcalc_ip_free(ip);
         free(str);
         return(1);
      };
   } else {
      // verifies cidr
      if ((cidr < -1) || (cidr > 128))
      {
         fprintf(stderr, "%s: invalid CIDR\n", PROGRAM_NAME);
         netcalc_ip_free(ip);
         free(str);
         return(1);
      };

      // parses string
      if ((rc = netcalc_ip_parse_ipv6(cnf, ip, str)) != 0)
      {
         netcalc_ip_free(ip);
         free(str);
         return(1);
      };
   };

   // normalizes CIDR
   if ((cnf->cidr > cidr) && (cidr != -1))
      cnf->cidr = cidr;

   // frees resources
   free(str);

   *ipp = ip;

   return(0);
}


int netcalc_ip_parse_ipv4( netcalc * cnf, netcalc_ip * ip, char * str )
{
   assert(cnf != NULL);
   assert(ip  != NULL);

   ip->addr[5] = 0xffff;

   // parses IPv4 string
   return(netcalc_ip_parse_ipv4_str(cnf, ip, str));
}


int netcalc_ip_parse_ipv4_str( netcalc * cnf, netcalc_ip * ip, char * str )
{
   uint32_t        addr;
   uint32_t        octet;
   int             pos;
   char *          ptr;
   char *          endptr;
   char *          nodes[4];

   assert(cnf != NULL);
   assert(ip  != NULL);
   assert(str != NULL);

   // set state
   addr = 0;

   // splits octets
   for(pos = 0; (pos < 3); pos++)
   {
      nodes[pos] = str;
      if ((ptr = strchr(str, '.')) == NULL)
      {
         fprintf(stderr, "%s: invalid IPv4 address\n", PROGRAM_NAME);
         return(1);
      };
      ptr[0] = '\0';
      str    = &ptr[1];
   };
   nodes[pos] = str;

   // parses octets
   for(pos = 0; (pos < 4); pos++)
   {
      octet = (uint32_t)strtoul(nodes[pos], &endptr, 10);
      if ((nodes[pos] == endptr) || (endptr[0] != '\0') || (octet > 255))
      {
         fprintf(stderr, "%s: invalid IPv4 address\n", PROGRAM_NAME);
         return(1);
      };
      addr = (addr << 8) | (octet & 0xff);
   };

   // stores address data
   ip->addr[6] = (addr >> 16) & 0xffff;
   ip->addr[7] = (addr >>  0) & 0xffff;

   return(0);
}


int netcalc_ip_parse_ipv6( netcalc * cnf, netcalc_ip * ip, char * str)
{
   int      pos;
   int      rev;
   int      compress;
   char *   nodes[9];
   char *   ptr;
   char *   endptr;

   assert(cnf != NULL);
   assert(ip  != NULL);

   cnf->family = NETCALC_INET6;

   // verifies zero compression
   compress = 0;
   for(pos = 1; (pos < (int)strlen(str)); pos++)
      if ((str[pos-0] == ':') && (str[pos-1] == ':'))
         compress++;
   if (compress > 1)
   {
      fprintf(stderr, "%s:%i: invalid IPv6 address\n", PROGRAM_NAME, __LINE__);
      return(1);
   };

   // set state
   bzero(nodes, sizeof(nodes));

   // splits octets
   for(pos = 0; ((pos < 8) && (str[0] != ':') && ((ptr = strchr(str, ':')) != NULL)); pos++)
   {
      nodes[pos] = str;
      str        = &ptr[1];
      ptr[0]     = '\0';
   };
   for(rev = 7; ((rev > 0) && ((ptr = strrchr(str, ':')) != NULL)); rev--)
   {
      ptr[0]     = '\0';
      nodes[rev] = &ptr[1];
      if ((rev == 7) && ((strchr(nodes[rev], '.'))))
         rev--;
   };
   nodes[rev] = str;
   if (rev < pos)
   {
      fprintf(stderr, "%s:%i: invalid IPv6 address\n", PROGRAM_NAME, __LINE__);
      return(1);
   };

   // converts string to numeric data
   for(pos = 0; (pos < 8); pos++)
   {
      if (!(nodes[pos]))
         continue;
      if (!(nodes[pos][0]))
         continue;
      ip->addr[pos] = (uint32_t)strtoul(nodes[pos], &endptr, 16);
      if ((pos == 7) && (endptr[0] == '.'))
         return(netcalc_ip_parse_ipv6_mapped_ipv4(cnf, ip, nodes[7]));
      if ( (nodes[pos] == endptr) || (endptr[0] != '\0') || (ip->addr[pos] > 0xffff) )
      {
         fprintf(stderr, "%s:%i: invalid IPv6 address\n", PROGRAM_NAME, __LINE__);
         return(1);
      };
   };

   return(0);
}


int netcalc_ip_parse_ipv6_mapped_ipv4( netcalc * cnf, netcalc_ip * ip, char * node )
{
   int          pos;

   assert(cnf  != NULL);
   assert(ip   != NULL);
   assert(node != NULL);

   // verify syntax
   for(pos = 0; (pos < 5); pos++)
   {
      if (ip->addr[pos] != 0)
      {
         fprintf(stderr, "%s:%i: invalid IPv6 address\n", PROGRAM_NAME, __LINE__);
         return(1);
      };
   };
   if (ip->addr[5] != 0xffff)
   {
      fprintf(stderr, "%s:%i: invalid IPv6 address\n", PROGRAM_NAME, __LINE__);
      return(1);
   };

   return(netcalc_ip_parse_ipv4_str(cnf, ip, node));
}


int netcalc_ip_string( netcalc * cnf, netcalc_ip * ip, char * str, size_t size )
{
   if (cnf->display == NETCALC_INET)
      return(netcalc_ip_string_ipv4(cnf, ip, str, size));
   return(netcalc_ip_string_ipv6(cnf, ip, str, size));
}


int netcalc_ip_string_ipv4( netcalc * cnf, netcalc_ip * ip, char * str, size_t size )
{
   assert(cnf != NULL);
   assert(ip  != NULL);
   assert(str != NULL);
   assert(size > 0);
   snprintf(
      str,
      size,
      "%i.%i.%i.%i",
      (ip->addr[6] >> 8),
      (ip->addr[6]  & 0xff),
      (ip->addr[7] >> 8),
      (ip->addr[7]  & 0xff)
   );
   return(0);
}


int netcalc_ip_string_ipv6( netcalc * cnf, netcalc_ip * ip, char * str, size_t size )
{
   char    b[32];
   char *  ptr;
   int     ipv4mapped;
   int     padding;
   int     pos;
   int     zero_offset;
   int     zero_len;
   int     offset;

   assert(cnf != NULL);
   assert(ip  != NULL);
   assert(str != NULL);
   assert(size > 0);

   // set state
   str[0]     = '\0';

   // calculates zero compression
   zero_offset = 0;
   zero_len    = 0;
   for(pos = 0; (pos < 8); pos++)
   {
      if (ip->addr[pos] == 0)
      {
         offset = pos;
         while((ip->addr[pos] == 0) && (pos < 8))
            pos++;
         if ((pos - offset) > zero_len)
         {
            zero_offset = offset;
            zero_len    = pos - offset;
         };
      };
   };

   // determines if IPv4 mapped address
   ipv4mapped = (zero_offset == 0)              ? 1          : 0;
   ipv4mapped = (zero_len == 5)                 ? ipv4mapped : 0;
   ipv4mapped = (ip->addr[5] == 0xffff)         ? ipv4mapped : 0;
   ipv4mapped = (!(cnf->opts & NETCALC_NO_MAP)) ? ipv4mapped : 0;

   // prints full IP address
   if ((cnf->opts & NETCALC_IPV6_FULL) != 0)
   {
      if ((ipv4mapped))
      {
         snprintf(
            str,
            size,
            "0:0:0:0:0:ffff:%i.%i.%i.%i",
            (ip->addr[6] >> 8),
            (ip->addr[6]  & 0xff),
            (ip->addr[7] >> 8),
            (ip->addr[7]  & 0xff)
         );
      } else {
         padding = (cnf->opts & NETCALC_IPV6_EXPAND) ? 4 : 0;
         for(pos = 0; (pos < 7); pos++)
         {
            snprintf(b, sizeof(b), "%0*x:", padding, ip->addr[pos]);
            strncat(str, b, size);
         };
         snprintf(b, sizeof(b), "%0*x", padding, ip->addr[7]);
         strncat(str, b, size);
      };
      return(0);
   };

   // prints IPv4 mapped address
   if ((ipv4mapped))
   {
      snprintf(
         str,
         size,
         "::ffff:%i.%i.%i.%i",
         (ip->addr[6] >> 8),
         (ip->addr[6]  & 0xff),
         (ip->addr[7] >> 8),
         (ip->addr[7]  & 0xff)
      );
      return(0);
   };

   // prints IPv6 prefix
   for(pos = 0; (pos < 8); pos++)
   {
      if ((pos == zero_offset) && (zero_len > 1))
      {
         strncat(str, ((pos == 0) ? "::" : ":"), size);
         pos += zero_len - 1;
         if (pos > 6)
            strncat(str, ":", size);
      } else {
         snprintf(b, sizeof(b), "%x:", ip->addr[pos]);
         strncat(str, b, size);
      };
   };
   ptr = strrchr(str, ':');
   ptr[0] = '\0';

   return(0);
}


void netcalc_net_broadcast_r( netcalc_ip * broadcast, netcalc_ip * ip, int32_t cidr )
{
   int pos;
   netcalc_ip wildmask;
   netcalc_net_wildmask_r(&wildmask, cidr);
   for(pos = 0; (pos < 8); pos++)
      broadcast->addr[pos] = ip->addr[pos] | wildmask.addr[pos];
   return;
}


int netcalc_net_cmp( netcalc_ip * a, netcalc_ip * b, int32_t cidr )
{
   int           i;
   netcalc_ip    m;

   assert(a != NULL);
   assert(b != NULL);

   netcalc_net_netmask_r(&m, cidr);

   for(i = 0; ( (i < 7) && ((a->addr[i]&m.addr[i]) == (b->addr[i]&m.addr[i])) ); i++);

   if ( (a->addr[i] & m.addr[i]) < (b->addr[i] & m.addr[i]) )
      return(-1);

   if ( (a->addr[i] & m.addr[i]) > (b->addr[i] & m.addr[i]) )
      return(1);

   return(0);
}


void netcalc_net_netmask_r( netcalc_ip * netmask, int32_t cidr )
{
   int32_t pos;
   bzero(netmask, sizeof(netcalc_ip));
   for(pos = 0; (pos < (cidr / 16)); pos++)
      netmask->addr[pos] = 0xffff;
   for(cidr %= 16; (cidr > 0); cidr--)
   {
      netmask->addr[pos] |= 0x10000;
      netmask->addr[pos] >>= 1;
   };
   return;
}


void netcalc_net_network_r( netcalc_ip * network, netcalc_ip * ip, int32_t cidr )
{
   int pos;
   netcalc_ip netmask;
   netcalc_net_netmask_r(&netmask, cidr);
   for(pos = 0; (pos < 8); pos++)
      network->addr[pos] = ip->addr[pos] & netmask.addr[pos];
   return;
}


void netcalc_net_wildmask_r( netcalc_ip * wildmask, int32_t cidr )
{
   int32_t pos;
   netcalc_net_netmask_r(wildmask, cidr);
   for(pos = 0; (pos < 8); pos++)
      wildmask->addr[pos] = (~wildmask->addr[pos] & 0xffff);
   return;
}


void netcalc_print_ip( netcalc * cnf, netcalc_ip * ip, int32_t cidr, int header )
{
   int          pos;
   netcalc_ip   netmask;
   netcalc_ip   broadcast;
   netcalc_ip   network;
   netcalc_ip   wildmask;
   char         str_netmask[56];
   char         str_broadcast[56];
   char         str_network[56];
   char         str_wildmask[56];
   char         str_subnets[128];
   uint64_t     count;

   if ((ip))
   {
      netcalc_net_netmask_r(   &netmask,       cidr );
      netcalc_net_wildmask_r(  &wildmask,      cidr );
      netcalc_net_network_r(   &network,   ip, cidr );
      netcalc_net_broadcast_r( &broadcast, ip, cidr );
      netcalc_ip_string( cnf,  &netmask,   str_netmask,   sizeof(str_netmask) );
      netcalc_ip_string( cnf,  &wildmask,  str_wildmask,  sizeof(str_wildmask) );
      netcalc_ip_string( cnf,  &network,   str_network,   sizeof(str_network) );
      netcalc_ip_string( cnf,  &broadcast, str_broadcast, sizeof(str_broadcast) );
   };

   // network count
   if (cnf->cidr_incr >= cnf->cidr)
   {
      count = 1;
      for(pos = 0; (pos < (cnf->cidr_incr  - cnf->cidr)); pos++)
         count *= 2;
   } else {
      count = 0;
   };

   // print IPv4 information
   if (cnf->display == NETCALC_INET)
   {
      if ((header))
      {
         snprintf(str_subnets, sizeof(str_subnets), "/%" PRIu32 "s", (cnf->cidr_incr + 32 - 128));
         // prints field names
         printf("%-15s",   "Network");
         printf("  %-15s", "Broadcast");
         printf("  %-15s", "Netmask");
         printf("  %-15s", "Wildcard");
         printf("  %-4s",  "CIDR");
         printf("  %s",    str_subnets);
         printf("\n");
         return;
      };
      printf("%-15s",       str_network);
      printf("  %-15s",     str_broadcast);
      printf("  %-15s",     str_netmask);
      printf("  %-15s",     str_wildmask);
      printf("  %-4i",      cidr + 32 - 128);
      printf("  %" PRIu64,  count);
      printf("\n");
      return;
   };

   cnf->len_netmask   = (cnf->len_netmask   < (int)strlen(str_netmask))   ? (int)strlen(str_netmask)   : cnf->len_netmask;
   cnf->len_network   = (cnf->len_network   < (int)strlen(str_network))   ? (int)strlen(str_network)   : cnf->len_network;
   cnf->len_wildmask  = (cnf->len_wildmask  < (int)strlen(str_wildmask))  ? (int)strlen(str_wildmask)  : cnf->len_wildmask;
   cnf->len_broadcast = (cnf->len_broadcast < (int)strlen(str_broadcast)) ? (int)strlen(str_broadcast) : cnf->len_broadcast;

   // print IPv6 information
   if ((header))
   {
      snprintf(str_subnets, sizeof(str_subnets), "/%" PRIu32 "s", cnf->cidr_incr);
      // prints field names
      printf("%-*s",   cnf->len_network,   "Network");
      printf("  %-*s", cnf->len_broadcast, "Broadcast");
      printf("  %-*s", cnf->len_netmask,   "Netmask");
      printf("  %-*s", cnf->len_wildmask,  "Wildcard");
      printf("  %-4s",                     "CIDR");
      printf("  %s",                       str_subnets);
      printf("\n");
      return;
   };
   printf("%-*s",   cnf->len_network,   str_network);
   printf("  %-*s", cnf->len_broadcast, str_broadcast);
   printf("  %-*s", cnf->len_netmask,   str_netmask);
   printf("  %-*s", cnf->len_wildmask,  str_wildmask);
   printf("  %-4i",                     cidr);
   if ((cnf->cidr_incr >= cnf->cidr) && ((cnf->cidr_incr  - cnf->cidr) < 64))
      printf("  %" PRIu64,                 count);
   else
      printf("  n/a");
   printf("\n");

   return;
}


/// displays usage
void netcalc_usage(void)
{
   printf("Usage: %s [OPTIONS] address1 address2 ... addressN\n", PROGRAM_NAME);
   printf("  -a                        display all matching networks\n");
   printf("  -c cidr                   requested network size in CIDR notation\n");
   printf("  -f                        print full IPv6 notation (do not compress zeros)\n");
   printf("  -h, --help                print this help and exit\n");
   printf("  -i cidr                   increment size of network list\n");
   printf("  -m                        do not display IPv4 mapped addresses\n");
   printf("  -V, --version             print version number and exit\n");
   printf("  -x                        print IPv6 expanded notation (print leading zeros)\n");
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
   int           rc;
   int           opt_index;
   netcalc *     cnf;
   netcalc_ip *  ip;

   static char   short_opt[] = "6ac:fhi:mVx";
   static struct option long_opt[] =
   {
      {"help",          no_argument, 0, 'h'},
      {"version",       no_argument, 0, 'V'},
      {NULL,            0,           0, 0  }
   };

   if ((rc = netcalc_init(&cnf)) != 0)
      return(1);

   while((c = getopt_long(argc, argv, short_opt, long_opt, &opt_index)) != -1)
   {
      switch(c)
      {
         case -1:	/* no more arguments */
         case 0:	/* long options toggles */
         break;

         case '6':
         cnf->display = NETCALC_INET6;
         break;

         case 'a':
         cnf->opts |= NETCALC_ALL_NETWORKS;
         cnf->len_wildmask  = 39;
         cnf->len_broadcast = 39;
         break;

         case 'c':
         cnf->cidr_limit = (int32_t)strtol(optarg, NULL, 0);
         if ((cnf->cidr_limit < 0) || (cnf->cidr_limit > 128))
         {
            fprintf(stderr, "%s: invalid CIDR\n", PROGRAM_NAME);
            netcalc_free(cnf);
            return(1);
         };
         break;

         case 'f':
         cnf->opts |= NETCALC_IPV6_FULL;
         break;

         case 'h':
         netcalc_usage();
         return(0);

         case 'i':
         cnf->cidr_incr =  (int32_t)strtol(optarg, NULL, 0);
         if ((cnf->cidr_incr < 0) || (cnf->cidr_incr > 128))
         {
            fprintf(stderr, "%s: invalid CIDR\n", PROGRAM_NAME);
            netcalc_free(cnf);
            return(1);
         };
         break;

         case 'm':
         cnf->opts |= NETCALC_NO_MAP;
         break;

         case 'V':
         netcalc_version();
         return(0);

         case 'x':
         cnf->opts |= NETCALC_IPV6_EXPAND;
         cnf->opts |= NETCALC_IPV6_FULL;
         cnf->opts |= NETCALC_NO_MAP;
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

   if (optind >= argc)
   {
      fprintf(stderr, "%s: missing required argument\n", PROGRAM_NAME);
      fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
      return(1);
   };

   for(c = optind; c < argc; c++)
   {
      if ((rc = netcalc_ip_parse(cnf, &ip, argv[c])) != 0)
      {
         netcalc_free(cnf);
         return(1);
      };

      // initialize bounds
      if (!(cnf->lower))
      {
         if ((cnf->lower = malloc(sizeof(netcalc_ip))) == NULL)
         {
            fprintf(stderr, "%s: out of virtual memory\n", PROGRAM_NAME);
            netcalc_ip_free(ip);
            netcalc_free(cnf);
            return(1);
         };
         memcpy(cnf->lower, ip, sizeof(netcalc_ip));
      };
      if (!(cnf->upper))
      {
         if ((cnf->upper = malloc(sizeof(netcalc_ip))) == NULL)
         {
            fprintf(stderr, "%s: out of virtual memory\n", PROGRAM_NAME);
            netcalc_ip_free(ip);
            netcalc_free(cnf);
            return(1);
         };
         memcpy(cnf->upper, ip, sizeof(netcalc_ip));
      };

      // updates bounds
      if (netcalc_ip_cmp(ip, cnf->lower) < 0)
      {
         netcalc_ip_free(cnf->lower);
         cnf->lower = ip;
      } else if (netcalc_ip_cmp(ip, cnf->upper) > 0) {
         netcalc_ip_free(cnf->upper);
         cnf->upper = ip;
      } else {
         netcalc_ip_free(ip);
      };
   };

   // sets defaults and adjusts cidr
   cnf->display        = (cnf->family == NETCALC_INET6) ? NETCALC_INET6 : cnf->display;
   if (cnf->family == NETCALC_INET)
   {
      cnf->cidr_limit  = ((cnf->cidr_limit == -1)       ? 32            : cnf->cidr_limit) + 128 - 32;
      cnf->cidr_incr   = ((cnf->cidr_incr  == -1)       ? 32            : cnf->cidr_incr)  + 128 - 32;
      cnf->cidr        = (cnf->cidr < cnf->cidr_limit)  ? cnf->cidr     : cnf->cidr_limit;
   } else {
      cnf->cidr_limit  = (cnf->cidr_limit == -1)        ? 64            : cnf->cidr_limit;
      cnf->cidr_incr   = (cnf->cidr_incr  == -1)        ? 64            : cnf->cidr_incr;
      cnf->cidr        = (cnf->cidr < cnf->cidr_limit)  ? cnf->cidr     : cnf->cidr_limit;
   };

   // calculate inclusive CIDR
   while ((rc = netcalc_net_cmp(cnf->lower, cnf->upper, cnf->cidr)) != 0)
      cnf->cidr--;

   // print header
   netcalc_print_ip(cnf, cnf->lower, cnf->cidr, 1);
   netcalc_print_ip(cnf, cnf->lower, cnf->cidr, 0);

   // print all matching subnets
   if ((cnf->opts & NETCALC_ALL_NETWORKS))
   {
      if (cnf->family == NETCALC_INET6)
      {
         while(cnf->cidr > 0)
         {
            cnf->cidr--;
            netcalc_print_ip(cnf, cnf->lower, cnf->cidr, 0);
         };
      } else {
         while(cnf->cidr > (128-32))
         {
            cnf->cidr--;
            netcalc_print_ip(cnf, cnf->lower, cnf->cidr, 0);
         };
      };
   };

   return(0);
}

/* end of source file */
