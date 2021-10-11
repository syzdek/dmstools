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
 *  @file src/bitcopy.c simple utility for quick bitwise operations
 */
/*
 *  Simple Build:
 *     gcc -W -Wall -O2 -c bitcopy.c
 *     gcc -W -Wall -O2 -o bitcopy   bitcopy.o
 *
 *  GNU Libtool Build:
 *     libtool --mode=compile gcc -W -Wall -g -O2 -c bitcopy.c
 *     libtool --mode=link    gcc -W -Wall -g -O2 -o bitcopy bitcopy.lo
 *
 *  GNU Libtool Install:
 *     libtool --mode=install install -c bitcopy /usr/local/bin/bitcopy
 *
 *  GNU Libtool Clean:
 *     libtool --mode=clean rm -f bitcopy.lo bitcopy
 */
#define _SCT_SRC_BITCOPY_C 1

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
#include <string.h>
#include <unistd.h>
#include <getopt.h>


///////////////////
//               //
//  Definitions  //
//               //
///////////////////

#ifndef PROGRAM_NAME
#define PROGRAM_NAME "bitcopy"
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

// inserts a series of bits into a buffer at the specified offset
void bitops_copy(uint8_t * dst, uint8_t * src, uint32_t len, uint32_t offset,
   uint32_t n);

// main statement
int main(int argc, char * argv[]);

// determines endianness of platform
int32_t my_make_littleendian(uint32_t data);

//displays usage information
void my_usage(void);

// displays version information
void my_version(void);

// displays data in binary notation
char * my_byte2str(uint32_t data, char * buff, uint32_t len);

// displays data in binary notation
void my_print_data(uint8_t * data, uint32_t len);


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////

/// inserts a series of bits into a buffer at the specified bit offset
/// @param[in]  dst      pointer to buffer
/// @param[in]  src      array of data to insert into the dst buffer
/// @param[in]  len      length in bytes of the buffer
/// @param[in]  offset   number of bits to offset the data in the buffer
/// @param[in]  n        number of bits to insert into the buffer
void bitops_copy(uint8_t * dst, uint8_t * src, uint32_t len, uint32_t offset,
   uint32_t n)
{
   uint32_t byte_required;
   uint32_t byte_offset;
   uint32_t bit_offset;
   uint32_t byte_n;
   uint32_t bit_n;
   uint32_t mask;
   uint32_t pos;
   uint32_t u;

   // preliminary calculations to determine limits
   byte_required  = ((offset + n) & 0x07) ? 1 : 0;
   byte_required  = ((offset + n) / 0x08) + byte_required;
   byte_offset    = (offset / 0x08);
   bit_offset     = (offset & 0x07);
   byte_n         = (n / 0x08);
   bit_n          = (n & 0x07);

   if (dst == src)
      return;
   if (byte_required > len)
      return;

   pos = 0;
   if (byte_n)
   {
      // generates a mask used for copying bits accross bit boundaries
      mask = 0xFF;
      for(u = 0; u < bit_offset; u++)
         mask = (mask << 1) & 0xFF;

      // copies first byte of data
      dst[byte_offset]     = ((src[0] << bit_offset) & ( mask)) |
                             ((dst[byte_offset])     & (~mask));

      // copies middle bytes
      for(pos = 1; pos < (byte_n); pos++)
         dst[byte_offset+pos] = ((src[pos]   << (bit_offset))   &  (mask)) |
                                ((src[pos-1] >> (8-bit_offset)) & (~mask));

      // copies last byte on an even bit boundary
      dst[byte_offset+pos] = ((dst[byte_offset+pos])         &  (mask)) |
                             ((src[pos-1] >> (8-bit_offset)) & (~mask));

   };

   if ((bit_offset) || (bit_n))
   {
      // generates a mask used to copy remaining least significant
      // bits into the remaining least significant byte
      mask = 0x00;
      for(u = 0; u < bit_n; u++)
         mask = ((mask << 1) | 0x01) & 0xFF;
      for(u = 0; u < bit_offset; u++)
         mask = (mask << 1) & 0xFF;

      // copies remaining bits into the least significant
      // byte (of remaining two bytes)
      dst[byte_offset+pos]   = ((src[pos] << bit_offset) &  (mask)) |
                               ((dst[byte_offset+pos])   & (~mask));

      if ((bit_offset + bit_n) > 8)
      {
         // generates a mask used to copy remaining most significant
         // bits into the remaining most significant byte
         mask = 0x00;
         for(u = 0; u < ((bit_n+bit_offset)%8); u++)
            mask = ((mask << 1) | 0x01) & 0xFF;

         // copies remaining bits into the most significant
         // byte (of remaining two bytes)
         u = 8-bit_offset;
         dst[byte_offset+pos+1] = ((src[pos] >> u)          &  (mask)) |
                                  ((dst[byte_offset+pos+1]) & (~mask));
      };
   };

   return;
}


/// main statement
/// @param[in] argc   number of arguments
/// @param[in] argv   array of arguments
int main(int argc, char * argv[])
{
   int         c;
   int         opt_index;
   uint8_t   * buff;
   uint32_t    u;
   uint32_t    len;
   uint32_t    data;
   uint32_t    offset;
   uint32_t    verbose;
   uint32_t    buff_len;

   // getopt options
   static char   short_opt[] = "B:b:d:hL:l:O:o:Vv";
   static struct option long_opt[] =
   {
      {"help",          no_argument, 0, 'h'},
      {"verbose",       no_argument, 0, 'v'},
      {"version",       no_argument, 0, 'V'},
      {NULL,            0,           0, 0  }
   };

   len      = 0;
   data     = 0;
   offset   = 0;
   verbose  = 0;
   buff_len = 40;
   
   while((c = getopt_long(argc, argv, short_opt, long_opt, &opt_index)) != -1)
   {
      switch(c)
      {
         case -1:	/* no more arguments */
         case 0:	/* long options toggles */
            break;
         case 'B':
            buff_len = (uint32_t)strtoul(optarg, NULL, 0);
            break;
         case 'b':
            u        = (uint32_t)strtoul(optarg, NULL, 0);
            buff_len = (u & 0x07) ? ((u/8) + 1) : (u/8);
            break;
         case 'd':
            data = (uint32_t)strtoul(optarg, NULL, 0);
            break;
         case 'h':
            my_usage();
            return(0);
         case 'L':
            len  = (uint32_t)strtoul(optarg, NULL, 0);
            len *= 8;
            if (len > 32)
               len = 32;
            break;
         case 'l':
            len = (uint32_t)strtoul(optarg, NULL, 0);
            if (len > 32)
               len = 32;
            break;
         case 'O':
            offset  = (uint32_t)strtoul(optarg, NULL, 0);
            offset *= 8;
            break;
         case 'o':
            offset = (uint32_t)strtoul(optarg, NULL, 0);
            break;
         case 'V':
            my_version();
            return(0);
         case 'v':
            verbose++;
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

   if (verbose > 1)
      printf("%s (%s) %s\n", PROGRAM_NAME, PACKAGE_NAME, PACKAGE_VERSION);
   if (verbose > 2)
   {
      printf("settings:\n");
      printf("   %-15s %8u\n",       "verbose level:", verbose);
      printf("   %-15s %8u bits (~ %2u bytes)\n", "buffer size:",   buff_len*8, buff_len);
      printf("   %-15s %8u bits (~ %2u bytes)\n", "data offset:",   offset,  (offset&0x07) ? (offset/8)+1 : (offset/8));
      printf("   %-15s %8u bits (~ %2u bytes)\n", "data length:",   len,     (len&0x07)    ?    (len/8)+1 :    (len/8));
   };

   if (verbose)
      printf("allocating memory...\n");
   if (!(buff = malloc((size_t)buff_len)))
   {
      perror("malloc()");
      return(1);
   };
   memset(buff, 0x00, (size_t)buff_len);

   data = my_make_littleendian(data);

   if (verbose)
      printf("copying memory...\n");
   bitops_copy(buff, (uint8_t *)&data, buff_len, offset, len);

   if (verbose)
      printf("printing memory...\n");
   my_print_data(buff, buff_len);

   if (verbose)
      printf("freeing memory...\n");
   free(buff);

   return(0);
}


// determines endianness of platform
int32_t my_make_littleendian(uint32_t data)
{
   int32_t i = 0x00FF;
   if ( (*(uint8_t *)&i) != 0 )
      return(data);
   i = data;
   i = (( i << 16 ) & 0xFFFF0000 ) | (( i >> 16 ) & 0x0000FFFF );
   i = (( i <<  8 ) & 0xFF00FF00 ) | (( i >>  8 ) & 0x00FF00FF );
   return(i);
}


/// displays usage information
void my_usage()
{
   printf("Usage: %s [options]\n", PROGRAM_NAME);
   printf("  -B bytes                  size of buffer to use\n");
   printf("  -b bits                   size of buffer to use\n");
   printf("  -d data                   data to insert\n");
   printf("  -h, --help                print this help and exit\n");
   printf("  -L bytes                  length of data in bytes\n");
   printf("  -l bits                   length of data in bits\n");
   printf("  -O bytes                  offset in bytes to store data\n");
   printf("  -o bits                   offset in bits to store data\n");
   printf("  -V, --version             print verbose messages\n");
   printf("  -v, --verbose             print version number and exit\n");
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


// displays a byte in binary notation
char * my_byte2str(uint32_t data, char * buff, uint32_t len)
{
   uint32_t b;
   if ( (!(buff)) || (!(len)) )
      return(buff);
   for(b = 0; ((b < 8) && (b < (len - 1))); b++)
      buff[b] = (data & (0x01 << b)) ? '1' : '0';
   buff[b] = '\0';
   return(buff);
}


// displays data in binary notation
void my_print_data(uint8_t * data, uint32_t len)
{
   uint32_t u;
   char     buff[9];
   printf("offset     000      010      020      030      040      050      060      070   \n");
   printf("000000: ");
   for(u = 0; u < len; u++)
   {
      printf("%s", my_byte2str(data[u], buff, 9));
      if ( ((u%8) == 7) && ((u+1) < len) )
         printf("\n0%03o00: ", (u/8)+1);
      else
         printf(" ");
   };
   printf("\n");
   return;
}


// end of source file
