/*
 *  DMS Tools and Utilities
 *  Copyright (c) 2008 David M. Syzdek <david@syzdek.net>.
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
 *  @file src/vaportar.c Tool for creating empty tar files
 */
#define _SCT_SRC_VAPORTAR_C 1

/////////////
//         //
//  Notes  //
//         //
/////////////

/*
 *  All,
 *
 *  On the surface this utility may seem like a completely useless tool. In
 *  truth it serves no practical purpose that I can imagine. This program was
 *  written for one purpose and one purpose alone, to server as my prankish
 *  requital to one of my my co-workers.
 *
 *                                                             David M. Syzdek
 *                                                            david@syzdek.net
 *                                                                  2008/06/17
 */


///////////////////
//               //
//  Definitions  //
//               //
///////////////////

#ifndef PROGRAM_NAME
#define PROGRAM_NAME "codetagger"
#endif

#define VTFILE_TYPE_NORMAL1	'0'
#define VTFILE_TYPE_NORMAL2	'\0'
#define VTFILE_TYPE_HARDLINK	'1'
#define VTFILE_TYPE_SYMLINK	'2'
#define VTFILE_TYPE_CHAR	'3'
#define VTFILE_TYPE_BLOCK	'4'
#define VTFILE_TYPE_DIRECTORY	'5'
#define VTFILE_TYPE_FIFO	'6'
#define VTFILE_TYPE_CONTIGUOUS	'7'

///////////////
//           //
//  Headers  //
//           //
///////////////

#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <inttypes.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include "common.h"


/////////////////
//             //
//  Datatypes  //
//             //
/////////////////

/* file header */
union record
{
   uint8_t data[512];
   struct  header
   {
      /* standard header 0x00 - 0x0100 (0 - 255) */
      char name[100];		/* 0x0000    (0) */
      char mode[8];		/* 0x0064  (100) */
      char uid[8];		/* 0x006c  (108) */
      char gid[8];		/* 0x0074  (116) */
      char size[12];		/* 0x007C  (124) */
      char mtime[12];		/* 0x0088  (136) */
      char chksum[8];		/* 0x0094  (148) */
      char linkflag;		/* 0x009c  (156) */
      char linkname[100];	/* 0x009d  (157) */

      /* USTART header */
      char magic[8];		/* 0x0101  (257) */
      char uname[32];		/* 0x0107  (265) */
      char gname[32];		/* 0x0107  (297) */
      char major[8];		/* 0x0107  (329) */
      char minor[8];		/* 0x0107  (337) */
      char prefix[155];         /* 0x0159  (345) */

      /* padding */
      char padding[12];
   } header;
};


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////

/* main statement */
int main PARAMS((int argc, char * argv[]));

/* check sums the header */
int my_checksum PARAMS((union record * header));

/* dumps memory to hex */
void my_hexdump PARAMS((const uint8_t * data, unsigned len));

/* display usage */
void my_usage PARAMS((void));

/* displays usage */
void my_version PARAMS((void));


/////////////////
//             //
//  Functions  //
//             //
/////////////////

/* main statement */
int main(int argc, char * argv[])
{
   /* declares local vars */
   int           c;
   int           fd;
   int           rd;
   int           len;
   int           opt_index;
   uint32_t      stamp;
   uint8_t       data[4096];
   uint8_t       white[4096];
   union record header;

   /* getopt options */
   static char   short_opt[] = "f:htvV";
   static struct option long_opt[] =
   {
      {"help",          no_argument, 0, 'h'},
      {"verbose",       no_argument, 0, 'v'},
      {"version",       no_argument, 0, 'V'},
      {NULL,            0,           0, 0  }
   };

   /* initialize variables */
   opt_index = 0;

   /* loops through arguments */
   while((c = getopt_long(argc, argv, short_opt, long_opt, &opt_index)) != -1)
   {
      switch(c)
      {
         case -1:	/* no more arguments */
         case 0:	/* long options toggles */
            break;

         case 'h':
            my_usage();
            return(0);
         case 'V':
            my_version();
            return(0);
         case 'v':
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

   memset(data,    1, 1024);
   memset(&header, 0, sizeof(union record));
   strcpy(header.header.name,		"mac-on-stick/ ");
   strcpy(header.header.mode,		"0000000");
   strcpy(header.header.uid,		"0000000");
   strcpy(header.header.gid,		"0000000");
   strcpy(header.header.size,		"00000000010");
   strcpy(header.header.mtime,		"00001000000");
   strcpy(header.header.chksum,		"        ");
   header.header.linkflag		= '0';
   strcpy(header.header.linkname,	"");
   strcpy(header.header.magic,		"ustar  ");
   strcpy(header.header.uname,		"");
   strcpy(header.header.gname,		"");
   strcpy(header.header.major,		"");
   strcpy(header.header.minor,		"");
   strcpy(header.header.prefix,		"test/");

   my_checksum(&header);

   /* prints hex dump */
   my_hexdump(header.data, sizeof(header));

   if ((rd = open("/dev/random", O_RDONLY)) == -1)
   {
      perror("open()");
      return(1);
   };

   if ((fd = open("vaportar.tar", O_WRONLY|O_CREAT|O_TRUNC)) == -1)
   {
      perror("open()");
      return(1);
   };

   memset(white, 0, 4096);
   stamp = 327315640;

   for(c = 0; c < 1980; c++)
   {
      memset(data, 1, 4096);
      read(rd, &len, sizeof(len));
      len = ((unsigned)len) % 4096;
      len = read(rd, data, len);
      snprintf(header.header.size,  12, "%011o", len);
      snprintf(header.header.mtime, 12, "%011o", stamp);
      my_checksum(&header);
      write(fd, &header, 512);
      write(fd, data, len);
      if (len % 512)
         write(fd, white, 512 - (len%512));
//      stamp += 2592000;
   };

   close(fd);
   close(rd);

   /* ends function */
   return(0);
}


/* check sums the header */
int my_checksum(union record * header)
{
   uint32_t pos;
   uint32_t sum;

   memcpy(header->header.chksum, "        ", 8);

   sum = 0;
   for(pos = 0; pos < 512; pos++)
      sum += header->data[pos];

   snprintf(header->header.chksum, 7, "%06o", sum);
   header->header.chksum[7] = ' ';

   return(sum);
}


/* dumps memory to hex */
void my_hexdump(const uint8_t * data, unsigned len)
{
   int32_t  oset;
   uint32_t pos;
   uint32_t mod256;

   if (!(len))
      return;

   for(pos = 0; pos < len; pos++)
   {
      mod256 = pos % 256;

      if (!(mod256))
         printf(" offset    0  1  2  3   4  5  6  7   8  9  a  b   c  d  e  f  0123456789abcdef\n");

      switch(pos % 16)
      {
         case 0:
            printf("%08x ", pos);
            printf(" %02x", data[pos]);
            break;
         case 3:
         case 7:
         case 11:
            printf(" ");
            printf(" %02x", data[pos]);
            break;
         case 15:
            printf(" %02x", data[pos]);
            printf("  ");
            for(oset = 15; oset >= 0; oset--)
               if ((data[pos-oset] >= ' ') && (data[pos-oset] <= '~'))
                  printf("%c", (char) data[pos-oset]);
               else
                  printf(".");
            printf("\n");
            break;
         default:
            printf(" %02x", data[pos]);
            break;
      };

      if (mod256 == 255)
         printf("\n");
   };

   printf("\n");

   return;
}


/* displays usage */
void my_usage(void)
{
   printf("Usage: %s [OPTIONS] files\n", PROGRAM_NAME);
   printf("  -h, --help                print this help and exit\n");
   printf("  -v, --verbose             print verbose messages\n");
   printf("  -V, --version             print version number and exit\n");
   printf("\n");
#ifdef PACKAGE_BUGREPORT
   printf("Report bugs to <%s>.\n", PACKAGE_BUGREPORT);
#endif
   return;
}


/* displays version */
void my_version(void)
{
#if defined(PACKAGE_NAME) && defined(PACKAGE_VERSION) && defined(PACKAGE_COPYRIGHT)
   printf("%s (%s) %s\n", PROGRAM_NAME, PACKAGE_NAME, PACKAGE_VERSION);
   printf("Written by David M. Syzdek.\n");
   printf("\n");
   printf("%s\n", PACKAGE_COPYRIGHT);
   printf("This is free software; see the source for copying conditions.  There is NO\n");
   printf("warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n");
#endif
   return;
}

/* end of source code */
