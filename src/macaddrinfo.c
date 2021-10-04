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
 *  @file src/macaddrinfo.c converts string into octal, decimal, hex, and bin
 */
/*
 *  Simple Build:
 *     gcc -W -Wall -O2 -c macaddrinfo.c
 *     gcc -W -Wall -O2 -o macaddrinfo   macaddrinfo.o
 *
 *  GNU Libtool Build:
 *     libtool --mode=compile gcc -W -Wall -g -O2 -c macaddrinfo.c
 *     libtool --mode=link    gcc -W -Wall -g -O2 -o macaddrinfo macaddrinfo.lo
 *
 *  GNU Libtool Install:
 *     libtool --mode=install install -c macaddrinfo /usr/local/bin/macaddrinfo
 *
 *  GNU Libtool Clean:
 *     libtool --mode=clean rm -f macaddrinfo.lo macaddrinfo
 */
#define _DMSTOOLS_SRC_MACADDRUTIL_C 1

///////////////
//           //
//  Headers  //
//           //
///////////////
#ifdef DMSTOOLS_PMARK
#pragma mark Headers
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <strings.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <stdarg.h>
#include <assert.h>
#include <regex.h>
#include <sys/socket.h>
#include <signal.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>


///////////////////
//               //
//  Definitions  //
//               //
///////////////////
#ifdef DMSTOOLS_PMARK
#pragma mark - Definitions
#endif

#ifndef PROGRAM_NAME
#define PROGRAM_NAME "macaddrinfo"
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


#define MAU_GETOPT_SHORT "hqVv"
#define MAU_GETOPT_LONG \
   {"colon",            no_argument,       NULL, 'c' }, \
   {"dot",              no_argument,       NULL, 'D' }, \
   {"dash",             no_argument,       NULL, 'd' }, \
   {"help",             no_argument,       NULL, 'h' }, \
   {"lower",            no_argument,       NULL, 'l' }, \
   {"quiet",            no_argument,       NULL, 'q' }, \
   {"raw",              no_argument,       NULL, 'R' }, \
   {"silent",           no_argument,       NULL, 'q' }, \
   {"upper",            no_argument,       NULL, 'u' }, \
   {"version",          no_argument,       NULL, 'V' }, \
   {"verbose",          no_argument,       NULL, 'v' }, \
   { NULL, 0, NULL, 0 }


#define MAU_MASK_CASE        0xf0000000
#define MAU_MASK_NOTATION    0x000000FF
#define MAU_CASE_LOWER       0x00000000
#define MAU_CASE_UPPER       0x10000000
#define MAU_NOTATION_COLON   0x00000000
#define MAU_NOTATION_DASH    0x00000001
#define MAU_NOTATION_DOT     0x00000002
#define MAU_NOTATION_RAW     0x00000003

#define MAU_CASE(notation)                (notation & MAU_MASK_CASE)
#define MAU_NOTATION(notation)            (notation & MAU_MASK_NOTATION)
#define MAU_SET_CASE(notation, flag)      ((notation & (~ MAU_MASK_CASE))     | (flag & MAU_MASK_CASE))
#define MAU_SET_NOTATION(notation, flag)  ((notation & (~ MAU_MASK_NOTATION)) | (flag & MAU_MASK_NOTATION))


#define MAU_BUFF_LEN    1024


// definitions providing default parameters for downloading the OUI database
#define MAU_DOWNLOAD    "http://bindlebinaries.com/bindle-oui.txt"
//#define MAU_DOWNLOAD    "http://standards.ieee.org/develop/regauth/oui/oui.txt"
#define MAU_REGEX       "^[[:space:]]{1,}([[:xdigit:]]{2,2})-([[:xdigit:]]{2,2})-([[:xdigit:]]{2,2})" \
                        "[[:space:]]{1,}\(hex\)" \
                        "[[:space:]]{1,}([[:alnum:][:punct:]][[:print:]]{1,})$"
#define MAU_REGEX_OCT1  1  ///< sub-match index of first octet of MAC address
#define MAU_REGEX_OCT2  2  ///< sub-match index of second octet of MAC address
#define MAU_REGEX_OCT3  3  ///< sub-match index of third octet of MAC address
#define MAU_REGEX_ORG   4  ///< sub-match index of OUI organization name
#define MAU_REGEX_MAX   20 ///< number of regex sub-matches allowed


//////////////////
//              //
//  Data Types  //
//              //
//////////////////
#ifdef DMSTOOLS_PMARK
#pragma mark - Data Types
#endif

typedef uint8_t mauaddr_t[6];    // Media Access Control address (MAC address)
typedef uint8_t mauoui_t[3];     // Organizationally Unique Identifier (OUI)
typedef uint8_t maueui64_t[8];   // 64-bit Global Identifier (EUI-64)
typedef char    maustr_t[INET6_ADDRSTRLEN+1];

typedef struct mau_config mau_config;
typedef struct mau_command mau_command;

struct mau_config
{
   int                  quiet;
   int                  verbose;
   int                  notation;
   int                  use_oui;
   int                  opt_index;
   int                  cmd_argc;
   char              ** cmd_argv;
   const char         * cmd_name;
   const char         * rnd_file;
   size_t               cmd_len;
   const mau_command  * cmd;
   regex_t              regex;
   const char         * regex_str;
   regmatch_t           matches[MAU_REGEX_MAX];
   char                 buff[MAU_BUFF_LEN];
   mauaddr_t            oui;
   uint8_t              pad8;
   uint8_t              pad8a;
};


struct mau_command
{
   const char * cmd_name;
   int  (*cmd_func)(mau_config * cnf);
   const char * cmd_shortopts;
   struct option * long_opt;
   int          min_arg;
   int          max_arg;
   const char * cmd_help;
   const char * cmd_desc;
};


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
#ifdef DMSTOOLS_PMARK
#pragma mark - Prototypes
#endif

// main statement
int main(int argc, char * argv[]);


int mau_cmd_eui64(mau_config * cnf);

// generate command
int mau_cmd_generate(mau_config * cnf);

int mau_cmd_info(mau_config * cnf);

int mau_cmd_link_local(mau_config * cnf);

int mau_cmd_macaddress(mau_config * cnf);

int mau_cmd_test(mau_config * cnf);

int mau_cmd_update(mau_config * cnf);

void mau_cmd_update_usage(void);

int mau_conv_eui2sin(mau_config * cnf, const maueui64_t eui, struct sockaddr_in6  * sin);
int mau_conv_eui2str(mau_config * cnf, const maueui64_t eui, maustr_t str);
int mau_conv_mac2eui(mau_config * cnf, const mauaddr_t addr, maueui64_t eui);
int mau_conv_mac2str(mau_config * cnf, const mauaddr_t addr, maustr_t str);
int mau_conv_eui2mac(mau_config * cnf, const maueui64_t eui, mauaddr_t addr);
int mau_conv_sin2eui(mau_config * cnf, const struct sockaddr_in6 * sin, maueui64_t eui);
int mau_conv_sin2str(mau_config * cnf, struct sockaddr_in6  * sin, maustr_t str);
int mau_conv_str2eui(mau_config * cnf, const maustr_t str, maueui64_t eui);
int mau_conv_str2mac(mau_config * cnf, const maustr_t str, mauaddr_t addr);
int mau_conv_str2sin(mau_config * cnf, const maustr_t str, struct sockaddr_in6 * sin);


int mau_getopt(mau_config * cnf, int argc, char * const * argv,
   const char * short_opt, const struct option * long_opt, int * opt_index);

// print error message
void mau_log_err(mau_config * cnf, const char * fmt, ...);

// print verbose information
void mau_log_verbose(mau_config * cnf, const char * fmt, ...);

// print warning message
void mau_log_warn(mau_config * cnf, const char * fmt, ...);

void mau_logv(mau_config * cnf, const char * fmt, va_list args);

//displays usage information
void mau_usage(mau_config * cnf);

// displays version information
void mau_version(void);



/////////////////
//             //
//  Variables  //
//             //
/////////////////
#ifdef DMSTOOLS_PMARK
#pragma mark - Variables
#endif

const mau_command mau_cmdmap[] =
{
   {
      "debugger",                                     // command name
      mau_cmd_test,                                   // entry function
      "abcdefghijklmnopqrstuvwxyz"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "0123456789",                                   // getopt short options
      (struct option []){ MAU_GETOPT_LONG },          // getopt long options
      -1, -1,                                         // min/max arguments
      NULL,                                           // cli usage
      NULL, // "print entire OUI database"                     // command description
   },
   {
      "dump",                                         // command name
      mau_cmd_test,                                   // entry function
      MAU_GETOPT_SHORT,                               // getopt short options
      (struct option []){ MAU_GETOPT_LONG },          // getopt long options
      0, 0,                                           // min/max arguments
      NULL,                                           // cli usage
      NULL, // "print entire OUI database"            // command description
   },
   {
      "eui64",                                        // command name
      mau_cmd_eui64,                                  // entry function
      "lu" MAU_GETOPT_SHORT,                          // getopt short options
      (struct option []){ MAU_GETOPT_LONG },          // getopt long options
      1, 1,                                           // min/max arguments
      " <address>",                                   // cli usage
      "display modified EUI-64 Identifier"            // command description
   },
   {
      "generate",                                     // command name
      mau_cmd_generate,                               // entry function
      "cDdlRr:uxMNQXW" MAU_GETOPT_SHORT,              // getopt short options
      (struct option [])
      {
         {"microsoft", no_argument, NULL, 'M' },
         {"nsa",       no_argument, NULL, 'N' },
         {"qemu",      no_argument, NULL, 'Q' },
         {"vmware",    no_argument, NULL, 'W' },
         {"xen",       no_argument, NULL, 'X' },
         MAU_GETOPT_LONG
      },                                              // getopt long options
      0, 0,                                           // min/max arguments
      NULL,                                           // cli usage
      "generate random MAC address"
   },
   {
      "information",                                  // command name
      mau_cmd_info,                                   // entry function
      "cDdlRu" MAU_GETOPT_SHORT,                      // getopt short options
      (struct option []){ MAU_GETOPT_LONG },          // getopt long options
      1, 1,                                           // min/max arguments
      " <address>",                                   // cli usage
      "display meta information about MAC address"    // command description
   },
   {
      "link-local",                                   // command name
      mau_cmd_link_local,                             // entry function
      MAU_GETOPT_SHORT,                               // getopt short options
      (struct option []){ MAU_GETOPT_LONG },          // getopt long options
      1, 1,                                           // min/max arguments
      " <address>",                                   // cli usage
      "display derived IPv6 link-local address"       // command description
   },
   {
      "macaddress",                                   // command name
      mau_cmd_macaddress,                             // entry function
      "cDdlRu" MAU_GETOPT_SHORT,                      // getopt short options
      (struct option []){ MAU_GETOPT_LONG },          // getopt long options
      1, 1,                                           // min/max arguments
      " <address>",                                   // cli usage
      "display MAC address using notation flags"      // command description
   },
   {
      "update",                                       // command name
      mau_cmd_update,                                 // entry function
      "cDdlRUu" MAU_GETOPT_SHORT,                     // getopt short options
      (struct option []){ MAU_GETOPT_LONG },          // getopt long options
      0, 0,                                           // min/max arguments
      NULL,                                             // cli usage
      "update local OUI database"                     // command description
   },
   {
      "vendor",                                       // command name
      mau_cmd_test,                                   // entry function
      "cDdlRu" MAU_GETOPT_SHORT,                      // getopt short options
      (struct option []){ MAU_GETOPT_LONG },          // getopt long options
      1, 1,                                           // min/max arguments
      " <organization>",                              // cli usage
      NULL, // "search for OUI by vendor name"                 // command description
   },
   { NULL, NULL, NULL, NULL, -1, -1, NULL, NULL }
};


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#ifdef DMSTOOLS_PMARK
#pragma mark - Functions
#endif

/// main statement
/// @param[in] argc   number of arguments
/// @param[in] argv   array of arguments
int main(int argc, char * argv[])
{
   int            i;
   int            c;
   int            opt_index;
   mau_config     cnf;

   // getopt options
   static char   short_opt[] = "+" MAU_GETOPT_SHORT;
   static struct option long_opt[] = { MAU_GETOPT_LONG };

   memset(&cnf, 0, sizeof(cnf));
   cnf.rnd_file = "/dev/random";

   while((c = mau_getopt(&cnf, argc, argv, short_opt, long_opt, &opt_index)) != -1)
   {
      switch(c)
      {
         case -2: /* captured by common options */
         case -1:	/* no more arguments */
         case 0:	/* long options toggles */
         break;

         case 1:
         return(0);

         case 2:
         return(1);

         case '?':
         fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
         return(1);

         default:
         fprintf(stderr, "%s: unrecognized option `--%c'\n", PROGRAM_NAME, c);
         fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
         return(1);
      };
   };

   if ((argc - optind) < 1)
   {
      fprintf(stderr, "%s: missing required argument\n", PROGRAM_NAME);
      fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
      return(1);
   };
   cnf.cmd_argc = (argc - optind);
   cnf.cmd_argv = &argv[optind];
   cnf.cmd_name = cnf.cmd_argv[0];
   cnf.cmd_len  = strlen(cnf.cmd_name);

   for(i = 0; (mau_cmdmap[i].cmd_name != NULL); i++)
   {
      if ((strncmp(cnf.cmd_name, mau_cmdmap[i].cmd_name, cnf.cmd_len)))
         continue;

      if ((cnf.cmd))
      {
         fprintf(stderr, "%s: ambiguous command -- \"%s\"\n", PROGRAM_NAME, cnf.cmd_name);
         fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
         return(1);
      };

      cnf.cmd = &mau_cmdmap[i];
   };
   if (!(cnf.cmd))
   {
      fprintf(stderr, "%s: unknown command -- \"%s\"\n", PROGRAM_NAME, cnf.cmd_name);
      fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
      return(1);
   };
   if (!(cnf.cmd->cmd_func))
   {
      fprintf(stderr, "%s: command not implemented -- \"%s\"\n", PROGRAM_NAME, cnf.cmd->cmd_name);
      fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
      return(1);
   };

   optind = 1;
   while((c = mau_getopt(&cnf, cnf.cmd_argc, cnf.cmd_argv, cnf.cmd->cmd_shortopts, cnf.cmd->long_opt, &cnf.opt_index)) != -1)
   {
      switch(c)
      {
         case -2: /* captured by common options */
         case -1:	/* no more arguments */
         case 0:	/* long options toggles */
         break;

         case 1:
         return(0);
         case 2:
         return(1);

         case '?':
         fprintf(stderr, "Try `%s %s --help' for more information.\n", PROGRAM_NAME, cnf.cmd_name);
         return(1);

         default:
         fprintf(stderr, "%s: %s: unrecognized option `--%c'\n", PROGRAM_NAME, cnf.cmd_name, c);
         fprintf(stderr, "Try `%s %s --help' for more information.\n", PROGRAM_NAME, cnf.cmd_name);
         return(1);
      };
   };

   if ((cnf.cmd_argc - optind) < cnf.cmd->min_arg)
   {
      fprintf(stderr, "%s: missing required argument\n", PROGRAM_NAME);
      fprintf(stderr, "Try `%s %s --help' for more information.\n", PROGRAM_NAME, cnf.cmd_name);
      return(1);
   };
   if ((optind+cnf.cmd->max_arg) > cnf.cmd_argc)
   {
      fprintf(stderr, "%s: unrecognized argument `-- %s'\n", PROGRAM_NAME, cnf.cmd_argv[optind+1]);
      fprintf(stderr, "Try `%s %s --help' for more information.\n", PROGRAM_NAME, cnf.cmd_name);
      return(1);
   };

   return(cnf.cmd->cmd_func(&cnf));
}


int mau_cmd_eui64(mau_config * cnf)
{
   mauaddr_t      addr;
   maueui64_t     eui;
   maustr_t       eui_str;

   if ((mau_conv_str2mac(cnf, cnf->cmd_argv[optind], addr)))
      return(1);
   mau_conv_mac2eui(cnf, addr, eui);
   mau_conv_eui2str(cnf, eui, eui_str);

   printf("%s\n", eui_str);

   return(0);
}


/// generate command
int mau_cmd_generate(mau_config * cnf)
{
   int            fd;
   mauaddr_t      addr;
   maustr_t       str;

   // generates random MAC
   mau_log_verbose(cnf, "opening file (%s)\n", cnf->rnd_file);
   if ((fd = open(cnf->rnd_file, O_RDONLY)) == -1)
   {
      mau_log_err(cnf, "%s: %s\n", cnf->rnd_file, strerror(errno));
      return(1);
   };
   mau_log_verbose(cnf, "%s: reading random data\n", PROGRAM_NAME);
   if (read(fd, addr, sizeof(addr)) == -1)
   {
      mau_log_err(cnf, "%s: %s\n", cnf->rnd_file, strerror(errno));
      mau_log_verbose(cnf, "%s: closing file\n", PROGRAM_NAME);
      close(fd);
      return(1);
   };
   mau_log_verbose(cnf, "%s: closing file\n", PROGRAM_NAME);
   close(fd);
   addr[0] = (addr[0] & 0xFC) | 0x02;


   // applies OUI
   if ((cnf->use_oui))
      memcpy(addr, cnf->oui, 3);

   mau_conv_mac2str(cnf, addr, str);
   printf("%s\n", str);

   return(0);
}


int mau_cmd_info(mau_config * cnf)
{
   mauaddr_t      addr;
   maustr_t       addr_str;
   maueui64_t     eui;
   maustr_t       eui_str;
   maustr_t       sin_str;
   struct sockaddr_in6   sin;

   if ((mau_conv_str2mac(cnf, cnf->cmd_argv[optind], addr)))
      return(1);

   mau_conv_mac2str(cnf, addr, addr_str);
   mau_conv_mac2eui(cnf, addr, eui);
   mau_conv_eui2str(cnf, eui, eui_str);
   mau_conv_eui2sin(cnf, eui, &sin);
   mau_conv_sin2str(cnf, &sin, sin_str);

   printf("MAC Address:      %s\n", addr_str);
   printf("U/L Bit:          %s\n", ((addr[0] & 0x02) == 0x02) ? "1 (locally administered)" : "0 (universally administered)");
   printf("Multicast Bit:    %s\n", ((addr[0] & 0x01) == 0x01) ? "1 (multicast)" : "0 (unicast)");
   printf("Modified EUI-64:  %s\n", eui_str);
   printf("IPv6 Link-local:  %s\n", sin_str);

   return(0);
}


int mau_cmd_link_local(mau_config * cnf)
{
   mauaddr_t      addr;
   maueui64_t     eui;
   maustr_t       sin_str;
   struct sockaddr_in6   sin;

   if ((mau_conv_str2mac(cnf, cnf->cmd_argv[optind], addr)))
      return(1);

   mau_conv_mac2eui(cnf, addr, eui);
   mau_conv_eui2sin(cnf, eui, &sin);
   mau_conv_sin2str(cnf, &sin, sin_str);

   printf("%s\n", sin_str);

   return(0);
}


int mau_cmd_macaddress(mau_config * cnf)
{
   maustr_t       str;
   mauaddr_t      addr;

   if ((mau_conv_str2mac(cnf, cnf->cmd_argv[optind], addr)))
      return(1);

   mau_conv_mac2str(cnf, addr, str);

   printf("%s\n", str);

   return(0);
}


int mau_cmd_test(mau_config * cnf)
{
   printf("%s\n", cnf->cmd_name);
   return(0);
}


int mau_cmd_update(mau_config * cnf)
{
   printf("%s\n", cnf->cmd_name);
   return(0);
}


void mau_cmd_update_usage(void)
{
   printf(
      "UPDATE OPTIONS:\n"
      "  -1 idx                    index of REGEX sub-match for first octet of OUI\n"
      "  -2 idx                    index of REGEX sub-match for second octet of OUI\n"
      "  -3 idx                    index of REGEX sub-match for third octet of OUI\n"
      "  -r regex                  regulare expression matching a single record in the OUI database\n"
      "  -O idx                    index of REGEX sub-match for OUI organization\n"
      "  -o file                   output file to save formatted OUI database\n"
      "  -u url                    download URL for source OUI database\n"
   );
   return;
}


int mau_conv_eui2mac(mau_config * cnf, const maueui64_t eui, maueui64_t addr)
{
   assert(cnf != NULL);
   addr[0] = (eui[0] ^ 0x02);
   addr[1] =  eui[1];
   addr[2] =  eui[2];
   addr[3] =  eui[5];
   addr[4] =  eui[6];
   addr[5] =  eui[7];
   return(0);
}


int mau_conv_eui2sin(mau_config * cnf, const maueui64_t eui, struct sockaddr_in6  * sin)
{
   assert(cnf != NULL);
   memset(sin, 0, sizeof(struct sockaddr_in6));
   sin->sin6_family = AF_INET6;
   sin->sin6_addr.s6_addr[0]  = 0xfe;
   sin->sin6_addr.s6_addr[1]  = 0x80;
   memcpy(&sin->sin6_addr.s6_addr[8], eui, 8);
   return(0);
}


int mau_conv_eui2str(mau_config * cnf, const maueui64_t eui, maustr_t str)
{
   switch (cnf->notation & MAU_MASK_CASE)
   {
      case MAU_CASE_LOWER:
      snprintf(str, sizeof(maustr_t), "%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x", eui[0],eui[1],eui[2],eui[3],eui[4],eui[5],eui[6],eui[7]);
      break;

      case MAU_CASE_UPPER:
      default:
      snprintf(str, sizeof(maustr_t), "%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X", eui[0],eui[1],eui[2],eui[3],eui[4],eui[5],eui[6],eui[7]);
      break;
   }
   return(0);
}


int mau_conv_mac2eui(mau_config * cnf, const mauaddr_t addr, maueui64_t eui)
{
   assert(cnf  != NULL);
   assert(eui  != NULL);
   assert(addr != NULL);

   eui[0] = (addr[0] ^ 0x02);
   eui[1] =  addr[1];
   eui[2] =  addr[2];
   eui[3] =  0xff;
   eui[4] =  0xfe;
   eui[5] =  addr[3];
   eui[6] =  addr[4];
   eui[7] =  addr[5];

   return(0);
}


int mau_conv_mac2str(mau_config * cnf, const mauaddr_t addr, maustr_t str)
{
   switch (cnf->notation | (MAU_MASK_NOTATION & MAU_MASK_CASE))
   {
      case (MAU_NOTATION_COLON | MAU_CASE_LOWER):
      snprintf(str, sizeof(maustr_t), "%02x:%02x:%02x:%02x:%02x:%02x", addr[0],addr[1],addr[2],addr[3],addr[4],addr[5]);
      break;

      case (MAU_NOTATION_COLON | MAU_CASE_UPPER):
      snprintf(str, sizeof(maustr_t), "%02X:%02X:%02X:%02X:%02X:%02X", addr[0],addr[1],addr[2],addr[3],addr[4],addr[5]);
      break;

      case (MAU_NOTATION_DASH | MAU_CASE_LOWER):
      snprintf(str, sizeof(maustr_t), "%02x-%02x-%02x-%02x-%02x-%02x", addr[0],addr[1],addr[2],addr[3],addr[4],addr[5]);
      break;

      case (MAU_NOTATION_DASH | MAU_CASE_UPPER):
      snprintf(str, sizeof(maustr_t), "%02X-%02X-%02X-%02X-%02X-%02X", addr[0],addr[1],addr[2],addr[3],addr[4],addr[5]);
      break;

      case (MAU_NOTATION_DOT | MAU_CASE_LOWER):
      snprintf(str, sizeof(maustr_t), "%02x%02x.%02x%02x.%02x%02x", addr[0],addr[1],addr[2],addr[3],addr[4],addr[5]);
      break;

      case (MAU_NOTATION_DOT | MAU_CASE_UPPER):
      snprintf(str, sizeof(maustr_t), "%02X%02X.%02X%02X.%02X%02X", addr[0],addr[1],addr[2],addr[3],addr[4],addr[5]);
      break;

      case (MAU_NOTATION_RAW | MAU_CASE_LOWER):
      snprintf(str, sizeof(maustr_t), "%02x%02x%02x%02x%02x%02x", addr[0],addr[1],addr[2],addr[3],addr[4],addr[5]);
      break;

      case (MAU_NOTATION_RAW | MAU_CASE_UPPER):
      snprintf(str, sizeof(maustr_t), "%02X%02X%02X%02X%02X%02X", addr[0],addr[1],addr[2],addr[3],addr[4],addr[5]);
      break;

      default:
      snprintf(str, sizeof(maustr_t), "%02x:%02x:%02x:%02x:%02x:%02x", addr[0],addr[1],addr[2],addr[3],addr[4],addr[5]);
   };
   return(0);
}


int mau_conv_sin2eui(mau_config * cnf, const struct sockaddr_in6 * sin, maueui64_t eui)
{
   assert(cnf != NULL);
   memcpy(eui, &sin->sin6_addr.s6_addr[8], 8);
   return(0);
}


int mau_conv_sin2str(mau_config * cnf, struct sockaddr_in6  * sin, maustr_t str)
{
   assert(cnf != NULL);
   getnameinfo((struct sockaddr *)sin, sizeof(struct sockaddr_in6), str, sizeof(maustr_t), NULL, 0, NI_NUMERICHOST|NI_NUMERICSERV);
   return(0);
}


int mau_conv_str2eui(mau_config * cnf, const maustr_t str, maueui64_t eui)
{
   int    buff[8];
   size_t slen;
   int    len;
   int    i;
   struct sockaddr_in6 sin;

   assert(cnf  != NULL);
   assert(str  != NULL);
   assert(eui != NULL);

   if (!(mau_conv_str2sin(cnf, str, &sin)))
   {
      mau_conv_sin2eui(cnf, &sin, eui);
      return(0);
   };

   slen = strlen(str);
   len  = 0;

   if (slen == 23)
      len = sscanf(str, "%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x", &buff[0],&buff[1],&buff[2],&buff[3],&buff[4],&buff[5],&buff[6],&buff[7]);
   if ((slen == 23) && (len != 8))
      len = sscanf(str, "%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x", &buff[0],&buff[1],&buff[2],&buff[3],&buff[4],&buff[5],&buff[6],&buff[7]);
   if ((slen == 19) && (len != 8))
      len = sscanf(str, "%02X%02X.%02X%02X.%02X%02X.%02X%02X", &buff[0],&buff[1],&buff[2],&buff[3],&buff[4],&buff[5],&buff[6],&buff[7]);
   if ((slen == 16) && (len != 8))
      len = sscanf(str, "%02X%02X%02X%02X%02X%02X%02X%02X", &buff[0],&buff[1],&buff[2],&buff[3],&buff[4],&buff[5],&buff[6],&buff[7]);
   if (len != 8)
      return(1);

   for(i = 0; i < 8; i++)
      eui[i] = (uint8_t) buff[i];

   return(0);
}


int mau_conv_str2mac(mau_config * cnf, const maustr_t str, mauaddr_t addr)
{
   maueui64_t eui;
   int        buff[6];
   size_t     slen;
   int        len;
   int        i;

   assert(str  != NULL);
   assert(addr != NULL);

   if (!(mau_conv_str2eui(cnf, str, eui)))
   {
      mau_conv_eui2mac(cnf, eui, addr);
      return(0);
   };

   slen = strlen(str);
   len  = 0;

   if (slen == 17)
      len = sscanf(str, "%02x:%02x:%02x:%02x:%02x:%02x", &buff[0],&buff[1],&buff[2],&buff[3],&buff[4],&buff[5]);
   if ((slen == 17) && (len != 6))
      len = sscanf(str, "%02x-%02x-%02x-%02x-%02x-%02x", &buff[0],&buff[1],&buff[2],&buff[3],&buff[4],&buff[5]);
   if ((slen == 14) && (len != 6))
      len = sscanf(str, "%02X%02X.%02X%02X.%02X%02X", &buff[0],&buff[1],&buff[2],&buff[3],&buff[4],&buff[5]);
   if ((slen == 12) && (len != 6))
      len = sscanf(str, "%02X%02X%02X%02X%02X%02X", &buff[0],&buff[1],&buff[2],&buff[3],&buff[4],&buff[5]);
   if (len != 6)
   {
      mau_log_err(cnf, "invalid MAC address\n");
      return(1);
   };

   for(i = 0; i < 6; i++)
      addr[i] = (uint8_t) buff[i];

   return(0);
}


int mau_conv_str2sin(mau_config * cnf, const maustr_t str, struct sockaddr_in6 * sin)
{
   struct addrinfo    hints;
   struct addrinfo  * servinfo;

   assert(cnf  != NULL);
   assert(str  != NULL);
   assert(sin != NULL);

   memset(&hints, 0, sizeof(hints));
   hints.ai_family   = AF_INET6;      // use AF_UNSPEC, AF_INET, or AF_INET6
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_flags    = AI_ADDRCONFIG | AI_PASSIVE | AI_NUMERICSERV | AI_NUMERICHOST;
   if (getaddrinfo(str, NULL, &hints, &servinfo) != 0)
      return(1);

   memset(sin, 0, sizeof(struct sockaddr_in6));
   sin->sin6_family = AF_INET6;
   memcpy(sin, servinfo->ai_addr, sizeof(struct sockaddr_in6));

   return(0);
}


int mau_getopt(mau_config * cnf, int argc, char * const * argv,
   const char * short_opt, const struct option * long_opt, int * opt_index)
{
   int            c;

   c = getopt_long(argc, argv, short_opt, long_opt, opt_index);

   switch(c)
   {
      case 0:
      if (!(strcmp(long_opt[*opt_index].name, "vmware")))
      {
         cnf->use_oui = 1;
         cnf->oui[0] = 0x00;
         cnf->oui[1] = 0x50;
         cnf->oui[2] = 0x56;
      };
      if (!(strcmp(long_opt[*opt_index].name, "xen")))
      {
         cnf->use_oui = 1;
         cnf->oui[0] = 0x00;
         cnf->oui[1] = 0x16;
         cnf->oui[2] = 0x3e;
      };
      if (!(strcmp(long_opt[*opt_index].name, "microsoft")))
      {
         cnf->use_oui = 1;
         cnf->oui[0] = 0x00;
         cnf->oui[1] = 0x15;
         cnf->oui[2] = 0x5d;
      };
      if (!(strcmp(long_opt[*opt_index].name, "qemu")))
      {
         cnf->use_oui = 1;
         cnf->oui[0] = 0x52;
         cnf->oui[1] = 0x54;
         cnf->oui[2] = 0x00;
      };
      if (!(strcmp(long_opt[*opt_index].name, "nsa")))
      {
         cnf->use_oui = 1;
         cnf->oui[0] = 0x00;
         cnf->oui[1] = 0x20;
         cnf->oui[2] = 0x91;
      };
      return(-2);

      case 'c':
      cnf->notation = MAU_SET_NOTATION(cnf->notation, MAU_NOTATION_COLON);
      return(-2);

      case 'D':
      cnf->notation = MAU_SET_NOTATION(cnf->notation, MAU_NOTATION_DOT);
      return(-2);

      case 'd':
      cnf->notation = MAU_SET_NOTATION(cnf->notation, MAU_NOTATION_DASH);
      return(-2);

      case 'h':
      mau_usage(cnf);
      return(1);

      case 'l':
      cnf->notation = MAU_SET_CASE(cnf->notation, MAU_CASE_LOWER);
      return(-2);

      case 'q':
      cnf->quiet++;
      return(-2);

      case 'R':
      cnf->notation = MAU_SET_NOTATION(cnf->notation, MAU_NOTATION_RAW);
      return(-2);

      case 'r':
      cnf->rnd_file = optarg;
      return(-2);

      case 'u':
      cnf->notation = MAU_SET_CASE(cnf->notation, MAU_CASE_UPPER);
      return(-2);

      case 'V':
      mau_version();
      return(1);

      case 'v':
      cnf->verbose++;
      return(-2);

      case 'M':
      cnf->use_oui = 1;
      cnf->oui[0] = 0x00;
      cnf->oui[1] = 0x15;
      cnf->oui[2] = 0x5d;
      return(-2);

      case 'N':
      cnf->use_oui = 1;
      cnf->oui[0] = 0x00;
      cnf->oui[1] = 0x20;
      cnf->oui[2] = 0x91;
      return(-2);

      case 'Q':
      cnf->use_oui = 1;
      cnf->oui[0] = 0x52;
      cnf->oui[1] = 0x54;
      cnf->oui[2] = 0x00;
      return(-2);

      case 'W':
      cnf->use_oui = 1;
      cnf->oui[0] = 0x00;
      cnf->oui[1] = 0x50;
      cnf->oui[2] = 0x56;
      return(-2);

      case 'X':
      cnf->use_oui = 1;
      cnf->oui[0] = 0x00;
      cnf->oui[1] = 0x16;
      cnf->oui[2] = 0x3e;
      return(-2);

      default:
      break;
   };
   return(c);
}


// print error message
void mau_log_err(mau_config * cnf, const char * fmt, ...)
{
   va_list args;
   if (cnf->verbose > 1)
      return;
   va_start(args, fmt);
   mau_logv(cnf, fmt, args);
   va_end(args);
   return;
}


// print verbose information
void mau_log_verbose(mau_config * cnf, const char * fmt, ...)
{
   va_list args;
   if (cnf->verbose < 1)
      return;
   va_start(args, fmt);
   mau_logv(cnf, fmt, args);
   va_end(args);
   return;
}


// print warning message
void mau_log_warn(mau_config * cnf, const char * fmt, ...)
{
   va_list args;
   if ((cnf->verbose))
      return;
   va_start(args, fmt);
   mau_logv(cnf, fmt, args);
   va_end(args);
   return;
}


// logs message
void mau_logv(mau_config * cnf, const char * fmt, va_list args)
{
   if ((cnf->cmd))
      fprintf(stderr, "%s %s: ", PROGRAM_NAME, cnf->cmd->cmd_name);
   else
      fprintf(stderr, "%s: ", PROGRAM_NAME);
   vfprintf(stderr, fmt, args);
   return;
}


/// displays usage information
void mau_usage(mau_config * cnf)
{
   int          i;
   const char * cmd_name = "COMMAND";
   const char * cmd_help = " ...";
   const char * shortopts;

   shortopts = MAU_GETOPT_SHORT;

   if ((cnf->cmd))
   {
      shortopts = ((cnf->cmd->cmd_shortopts)) ? cnf->cmd->cmd_shortopts : MAU_GETOPT_SHORT;
      cmd_name  = cnf->cmd->cmd_name;
      cmd_help  = ((cnf->cmd->cmd_help)) ? cnf->cmd->cmd_help : "";
   };

   printf("Usage: %s %s [OPTIONS]%s\n", PROGRAM_NAME, cmd_name, cmd_help);
   printf("OPTIONS:\n");
   if ((strchr(shortopts, 'c'))) printf("  -c, --colon               print MAC addresses in colon notation\n");
   if ((strchr(shortopts, 'D'))) printf("  -D, --dot                 print MAC addresses in dot notation\n");
   if ((strchr(shortopts, 'd'))) printf("  -d, --dash                print MAC addresses in dash notation\n");
   if ((strchr(shortopts, 'h'))) printf("  -h, --help                print this help and exit\n");
   if ((strchr(shortopts, 'l'))) printf("  -l, --lower               print HEX digits in lower case\n");
   if ((strchr(shortopts, 'q'))) printf("  -q, --quiet, --silent     do not print messages\n");
   if ((strchr(shortopts, 'R'))) printf("  -R, --raw                 print MAC addresses in raw hex notation\n");
   if ((strchr(shortopts, 'r'))) printf("  -r randomdev              a file containing random data\n");
   if ((strchr(shortopts, 'U'))) printf("  -U url                    URL of MA-L Public Listing\n");
   if ((strchr(shortopts, 'u'))) printf("  -u, --upper               print HEX digits in upper case\n");
   if ((strchr(shortopts, 'V'))) printf("  -V, --version             print version number and exit\n");
   if ((strchr(shortopts, 'v'))) printf("  -v, --verbose             print verbose messages\n");
   if ((strchr(shortopts, 'M'))) printf("  --microsoft               generate MAC address for Microsoft\n");
   if ((strchr(shortopts, 'N'))) printf("  --nsa                     generate MAC address for National Security Agency\n");
   if ((strchr(shortopts, 'Q'))) printf("  --qemu                    generate MAC address for QEMU/KVM\n");
   if ((strchr(shortopts, 'W'))) printf("  --vmware                  generate MAC address for VMWare\n");
   if ((strchr(shortopts, 'X'))) printf("  --xen                     generate MAC address for Xen\n");
   if (!(cnf->cmd))
   {
      printf("COMMANDS:\n");
      for(i = 0; mau_cmdmap[i].cmd_name != NULL; i++)
         if ((mau_cmdmap[i].cmd_desc))
            printf("  %-25s %s\n", mau_cmdmap[i].cmd_name, mau_cmdmap[i].cmd_desc);
   };
   printf("\n");
   return;
}


/// displays version information
void mau_version(void)
{
   printf(
      (
         "%s (%s) %s\n"
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
