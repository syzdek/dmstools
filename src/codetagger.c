/*
 *  DMS Tools and Utilities
 *  Copyright (C) 2008 David M. Syzdek <david@syzdek.net>.
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
 *  @file src/codetagger.c Utility for making bulk changes to tqgged text files.
 */
/*
 *  Simple Build:
 *     gcc -W -Wall -O2 -c codetagger.c
 *     gcc -W -Wall -O2 -o codetagger   codetagger.o
 *
 *  GNU Libtool Build:
 *     libtool --mode=compile gcc -W -Wall -g -O2 -c codetagger.c
 *     libtool --mode=link    gcc -W -Wall -g -O2 -o codetagger codetagger.lo
 *
 *  GNU Libtool Install:
 *     libtool --mode=install install -c codetagger /usr/local/bin/codetagger
 *
 *  GNU Libtool Clean:
 *     libtool --mode=clean rm -f codetagger.lo codetagger
 */
#define _DMSTOOLS_SRC_CODINGTAGGER_C 1

/////////////
//         //
//  Notes  //
//         //
/////////////

/*
 *  Debug Levels:
 *      0 - errors only
 *      1 - warnings
 *      2 - file processing notices
 *      3 - tag processing notices
 *      4 - general processing notices
 *      5 - detailed general processing notices
 *      6 - file operations
 *      7 - memory operations
 *      8 - variable dumps
 *      Debug levels are accumulative.
 */

///////////////
//           //
//  Headers  //
//           //
///////////////

#ifdef HAVE_COMMON_H
#include "common.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <getopt.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdarg.h>
#include <regex.h>


///////////////////
//               //
//  Definitions  //
//               //
///////////////////

#ifndef PROGRAM_NAME
#define PROGRAM_NAME "codetagger"
#endif

#ifndef PARAMS
#define PARAMS(protos) protos
#endif

#undef  STR_LEN
#define STR_LEN  512

#undef  BUFF_LEN
#define BUFF_LEN 1024

/////////////////
//             //
//  Datatypes  //
//             //
/////////////////

/* tag data */
typedef struct tag_data TagData;
struct tag_data
{
   char     * name;
   char    ** contents;
   regex_t    regex;
};


/* config data */
typedef struct config_data Config;
struct config_data
{
   unsigned      tagCount;
   TagData    ** tagList;
   const char  * tagFile;
   const char  * leftTagString;
   const char  * rightTagString;
};


/////////////////
//             //
//  Variables  //
//             //
/////////////////

/*
 *  I normally avoid global variables, however the following
 *  variables significantly simplify managing debug levels and
 *  test modes.  I feel the amount of complexity removed justified
 *  the addition of these variables. --David M. Syzdek 2008/06/21
 */
int verbosity = 0;	///< sets verbosity of program
int test_mode = 0;	///< toggles test mode


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////

/* prints debug messages */
void debug_printf PARAMS((int level, const char * fmt, ...));

/* escapes each character of the string and places the result at the end of the buffer */
int escape_string PARAMS((char * buff, const char * str, unsigned len));

/* processes original file by inserting/expanding tags */
int expand_tags PARAMS((const char * filename, Config * cnf));

/* replaces original file with temp file and unlinks temp file name */
int file_link PARAMS((FILE * fdout, const char * ofile, const char * tfile));

/* opens temporary file for writing using mkstemp() */
FILE * file_open PARAMS((const char * file, char * buff, int buff_len));

/* writes formatted data to temp file if the program is not in test mode */
void file_printf PARAMS((FILE * fdout, const char * fmt, ...));

/* reads file into an array */
char ** get_file_contents PARAMS((const char * file));

/* finds a specific tag in the array of tag data */
TagData * find_tag PARAMS((const char * tagName, TagData ** tagList,
                           const char * fileName, int lineNumber));

/* frees memory used to hold file contents */
void free_filedata PARAMS((char ** lines));

/* frees memory used store tag */
void free_tag PARAMS((TagData * tag));

/* frees memory used store an array of tags */
void free_taglist PARAMS((TagData ** taglist));

/* generate array of tags from file */
int generate_taglist PARAMS((Config * cnf));

/* main statement */
int main PARAMS((int argc, char * argv[]));

/* displays usage */
void my_usage PARAMS((void));

/* displays version information */
void my_version PARAMS((void));

/* extracts tag data from tag file */
int process_tag PARAMS((char * name, char ** data, int len, int pos,
                        Config * cnf));


/////////////////
//             //
//  Functions  //
//             //
/////////////////

/// prints debug messages
/// @param[in] level the verbose level of the current message
/// @param[in] fmt   the format string of the message
/// @param[in] ...   the format arguments of the message
void debug_printf(int level, const char * fmt, ...)
{
   va_list arg;

   if (level > verbosity)
      return;

   va_start (arg, fmt);
      vprintf(fmt, arg);
   va_end (arg);

   return;
}


/// escapes each character of the string and places the result at the end of the buffer
/// @param[in] buff  buffer into which escape the string
/// @param[in] str   string to escape and concatenate from
/// @param[in] len   size of buffer
int escape_string(char * buff, const char * str, unsigned len)
{
   unsigned strpos;
   unsigned buffpos;
   
   len--;
   buff[len] = '\0';
   
   buffpos = strlen(buff) + 2;
   for(strpos = 0; ( (strpos < strlen(str)) && (buffpos < len)); strpos++)
   {
      buff[buffpos-2] = '\\';
      buff[buffpos-1] = str[strpos];
      buffpos += 2;
   };
   buff[buffpos] = '\0';
   return(0);
}


/// processes original file by inserting/expanding tags
/// @param[in]  file  name of file to process
/// @param[in]  cnf   array of tags to insert into new file
int expand_tags(const char * filename, Config * cnf)
{
   /* declares local vars */
   int           data_pos;
   int           data_len;
   int           tag_pos;
   int           err;
   unsigned      tagCount;
   TagData     * tag;
   char       ** data;
   char          tmpfile[STR_LEN];
   char          margin[STR_LEN];
   char          errmsg[STR_LEN];
   char          tagName[STR_LEN];
   char          regstr[STR_LEN];
   char          tagend[STR_LEN];
   FILE        * fdout;
   regex_t       regex;
   regmatch_t    match[5];

   /* initialzes data */
   fdout      = NULL;
   tagCount   = 0;
   tmpfile[0] = '\0';
   memset(regstr, 0, STR_LEN);

   /* reads file and stores contents */
   if (!(data = get_file_contents(filename)))
      return(1);

   /* creates temp file */
   if (!(test_mode))
   {
      if (!(fdout = file_open(filename, tmpfile, STR_LEN)))
      {
         free_filedata(data);
         return(1);
      };
   };

   /* compiles regex used to locate tags */
   debug_printf(5, "   compiling regular expression\n");
   strncat(regstr,       "(.*)",              STR_LEN);
   escape_string(regstr, cnf->leftTagString,  STR_LEN);
   strncat(regstr,       "([_[:blank:][:alnum:]]+)START",   STR_LEN);
   escape_string(regstr, cnf->rightTagString, STR_LEN);
   if ((err = regcomp(&regex, regstr, REG_EXTENDED|REG_ICASE)))
   {
      regerror(err, &regex, errmsg, STR_LEN-1);
      printf("regex error: %s\n", errmsg);
      file_link(fdout, NULL, tmpfile);
      free_filedata(data);
      return(0);
   };
   debug_printf(8, "   compiled regular expression \"%s\"\n", regstr);

   /* counts lines in data */
   for(data_len = 0; data[data_len]; data_len++);

   /* loops through file */
   debug_printf(5, "   expanding tags\n");
   for(data_pos = 0; data_pos < data_len; data_pos++)
   {
      /* copies the old file into the new file */
      file_printf(fdout, "%s\n", data[data_pos]);

      /* checks line for a tag */
      if ((err = regexec(&regex, data[data_pos], 5, match, 0)))
         continue;

      /* copies tag information into buffers */
      memset(margin,  0, STR_LEN);
      memset(tagName, 0, STR_LEN);
      memcpy(margin,  &data[data_pos][(int)match[1].rm_so], match[1].rm_eo - match[1].rm_so);
      memcpy(tagName, &data[data_pos][(int)match[2].rm_so], match[2].rm_eo - match[2].rm_so);

      /* applies the tag to the new file */
      if ((tag = find_tag(tagName, cnf->tagList, filename, data_pos+1)))
      {
         /* writes contents of the tag */
         debug_printf(3, "   inserting tag \"%s\" (line %i)\n", tagName, data_pos);
         for(tag_pos = 0; tag->contents[tag_pos]; tag_pos++)
            file_printf(fdout, "%s%s\n", margin, tag->contents[tag_pos]);

         /* searches for end tag */
         err = 1;
         while((data_pos < (data_len-1)) && (err))
         { 
            data_pos++;
            err = regexec(&tag->regex, data[data_pos], 5, match, 0);
         };

         /* exits with error if end tag is not found */
         if (err)
         {
            fprintf(stderr, "%s: missing end @%sEND@ in %s\n", PROGRAM_NAME, tagName, filename);
            file_link(fdout, NULL, tmpfile);
            free_filedata(data);
            return(-1);
         };
         
         /* copies end tag into buffer */
         memset(tagend, 0, STR_LEN); 
         memcpy(tagend,  &data[data_pos][(int)match[1].rm_so], match[1].rm_eo - match[1].rm_so);
  
         /* prints end tag */
         file_printf(fdout, "%s%s%s%s\n", margin, cnf->leftTagString,
                     tagend, cnf->rightTagString);

         /* counts tags */
         tagCount++;
      };
   };

   /* closes and saves file */
   if (!(tagCount))
   {
      fprintf(stderr, PROGRAM_NAME ": no tags were found in \"%s\"\n", filename);
      file_link(fdout, NULL, tmpfile);
   } else {
      file_link(fdout, filename, tmpfile);
   };

   /* frees memory */
   free_filedata(data);

   /* ends function */
   return(0);
}


/// replaces original file with temp file and unlinks temp file name
/// @param[in] fdout  the file stream of the temp file
/// @param[in] ofile  the original file name
/// @param[in] tfile  the temporary file name    
int file_link(FILE * fdout, const char * ofile, const char * tfile)
{
   /* declares local vars */
   struct stat sb;

   /* closes file */
   if (!(fdout))
      return(0);
   fclose(fdout);

   /* exits if temporary file name is not specified */
   if (!(tfile))
      return(0);

   /* assumes clean-up mode if original file is not specified */
   if ( (tfile) && (!(ofile)) )
   {
      unlink(tfile); // removes temparary file
      return(0);
   };

   /* gathers information about original file to copy to temporary file */
   if (stat(ofile, &sb))
   {
      perror(PROGRAM_NAME ": stat()");
      return(-1);
   };

   /* sets file permissions of temporary file */
   if (chmod(tfile, sb.st_mode) == -1)
   {
      perror(PROGRAM_NAME ": chmod()");
      return(-1);
   };

   /* sets file ownerships of temporary file if run as root */
   if (!(geteuid()))
   {
      if (chown(tfile, sb.st_uid, sb.st_gid) == -1)
      {
         perror(PROGRAM_NAME ": chown()");
         return(-1);
      };
   };

   /* removes original file */
   if (unlink(ofile) == -1)
   {
      perror(PROGRAM_NAME ": unlink()");
      return(-1);
   };

   /* links temporary file to original file name */
   if (link(tfile, ofile) == -1)
   {
      perror(PROGRAM_NAME ": link()");
      return(-1);
   };

   /* removes temporary file */
   if (unlink(tfile) == -1)
   {
      perror(PROGRAM_NAME ": unlink()");
      return(-1);
   };

   /* ends function */
   return(0);
}


/// opens temporary file for writing using mkstemp()
/// @params[in] file      file name of file to open for writing
/// @params[in] buff      buffer to store temporary file name
/// @params[in] buff_len  length of buffer
FILE * file_open(const char * file, char * buff, int buff_len)
{
   int    fd;
   FILE * fdout;

   buff[0] = '\0';

   if (test_mode)
      return(NULL);

   snprintf(buff, buff_len, "%s.XXXXXXXXXX", file);
   if (!(fd = mkstemp(buff)))
   {
      perror(PROGRAM_NAME ": mkstemp()");
      return(NULL);
   };

   if (!(fdout = fdopen(fd, "w")))
   {
      perror(PROGRAM_NAME ": fdpopen()");
      return(NULL);
   };

   return(fdout);
}


/// writes formatted data to temp file if the program is not in test mode
/// @param[in]  fdout  file stream to write data
/// @param[in]  fmt    format of data to write to stream
/// @param[in]  ...    format arguments
void file_printf(FILE * fdout, const char * fmt, ...)
{
   va_list arg;

   if (test_mode)
      return;

   va_start (arg, fmt);
      vfprintf(fdout, fmt, arg);
   va_end (arg);

   return;
}


/// finds a specific tag in the array of tag data
/// @param[in] name name of tag to find
/// @param[in] taglist array of tags to search
/// @param[in] line_number line number of the current file being processed
TagData * find_tag(const char * tagName, TagData ** tagList,
                   const char * fileName, int lineNumber)
{
   int i;

   if (!(tagName))
      return(NULL);
   if (!(tagList))
      return(NULL);
   if (!(fileName))
      return(NULL);

   debug_printf(5, "   retrieving tag \"%s\"\n", tagName);

   for(i = 0; tagList[i]; i++)
      if (!(strcasecmp(tagName, tagList[i]->name)))
         return(tagList[i]);

   debug_printf(1, PROGRAM_NAME ": %s: %i: unknown tag \"%s\"\n", fileName, lineNumber, tagName);

   return(NULL);
}


/// frees memory used to hold file contents
/// @param[in]  lines  array of lines to free
void free_filedata(char ** lines)
{
   if (!(lines))
      return;

   if (lines[0])
      free(lines[0]);
   lines[0] = NULL;

   free(lines);

   return;
}


/// frees memory used store tag
/// @param[in]  tag  tag data structure to free
void free_tag(TagData * tag)
{
   int i;

   if (!(tag))
      return;

   if (tag->name)
   {
      debug_printf(7, "   freeing tag \"%s\"\n", tag->name);
      free(tag->name);
   } else {
      debug_printf(7, "   freeing unknown tag\n");
   };

   if (tag->contents)
   {
      regfree(&tag->regex);
      for(i = 0; tag->contents[i]; i++)
         free(tag->contents[i]);
      free(tag->contents);
   };

   free(tag);

   return;
}


/// frees memory used store an array of tags
/// @param[in]  taglist  array of tags to free
void free_taglist(TagData ** taglist)
{
   int i;
   if (!(taglist))
      return;
   debug_printf(7, "freeing tag index\n");
   for(i = 0; taglist[i]; i++)
      free_tag(taglist[i]);
   free(taglist);
   return;
}


/// generate array of tags from file
/// @param[in] file  file name of file which contains tag definitions
int generate_taglist(Config * cnf)
{
   /* declares local vars */
   int           i;
   int           err;
   int           lineCount;
   char          regstr[STR_LEN];
   char          errmsg[STR_LEN];
   char          tagName[STR_LEN];
   char       ** data;
   regex_t       regex;
   regmatch_t    match[5];

   /* initialize data */
   memset(regstr,   0, STR_LEN);
   memset(errmsg,   0, STR_LEN);
   memset(tagName,  0, STR_LEN);

   /* reads tag file */
   if (!(data = get_file_contents(cnf->tagFile)))
      return(-1);

   /* counts number of lines in tag file */
   for(lineCount = 0; data[lineCount]; lineCount++);

   /* compiles regex used to locate tags */
   debug_printf(5, "   compiling regular expression for tags\n");
   strncat(regstr,       "^",              STR_LEN);
   escape_string(regstr, cnf->leftTagString,  STR_LEN);
   strncat(regstr,       "([_[:blank:][:alnum:]]+)START",   STR_LEN);
   escape_string(regstr, cnf->rightTagString, STR_LEN);
   if ((err = regcomp(&regex, regstr, REG_EXTENDED|REG_ICASE)))
   {
      regerror(err, &regex, errmsg, STR_LEN-1);
      printf("regex error: %s\n", errmsg);
      return(-1);
   };
   debug_printf(8, "   compiled regular expression \"%s\"\n", regstr);

   /* loops through looking for tags */
   debug_printf(3, "creating tag index from \"%s\"\n", cnf->tagFile);
   for(i = 0; i < lineCount; i++)
   {
      if (!(err = regexec(&regex, data[i], 5, match, 0)))
      {
         data[i][(int)match[1].rm_eo] = '\0';
         strncpy(tagName, &data[i][(int)match[1].rm_so], STR_LEN-1);
         i++;
         if ((i = process_tag(tagName, data, lineCount, i, cnf)) == -1)
         {
            free_filedata(data);
            regfree(&regex);
            return(-1);
         };
         cnf->tagCount++;
      };
   };

   /* frees resources */
   debug_printf(7, "   freeing file buffer\n");
   free_filedata(data);
   debug_printf(7, "   freeing regular expressions\n");
   regfree(&regex);

   /* ends function */
   return(0);
}


/// reads file into an array
/// @param[in]  file  file name of file to process
char ** get_file_contents(const char * file)
{
   /* declares local vars */
   int            i;
   int            fd;
   int            len;
   int            count;
   char         * buff;
   char        ** lines;
   struct stat    sb;

   /* prints debug information */
   debug_printf(2, "processing file \"%s\"\n", file);

   /* stat file */
   debug_printf(6, "   inspecting file\n"); 
   if ((stat(file, &sb)) == -1)
   {
      perror(PROGRAM_NAME ": stat()");
      return(NULL);
   };

   /* opens file for reading */
   debug_printf(6, "   opening file\n"); 
   if ((fd = open(file, O_RDONLY)) == -1)
   {
      perror(PROGRAM_NAME ": open()");
      return(NULL);
   };

   /* allocates memory for buffer */
   debug_printf(7, "   allocating memory for file buffer\n"); 
   if (!(buff = (char *) malloc(sb.st_size + 1)))
   {
      fprintf(stderr, PROGRAM_NAME ": out of virtual memory\n");
      close(fd);
      return(NULL);
   };

   /* reads file into buffer */
   debug_printf(6, "   reading file\n"); 
   if ((len = read(fd, buff, sb.st_size)) == -1)
   {
      perror(PROGRAM_NAME ": read()");
      close(fd);
      return(NULL);
   };
   buff[len] = '\0';

   /* closes file */
   debug_printf(6, "   closing file\n"); 
   close(fd);

   /* counts and terminates lines */
   debug_printf(5, "   analyzing file contents\n"); 
   count = 0;
   for(i = 0; i < len; i++)
   {
      if (buff[i] == '\n')
      {
         count++;
         buff[i] = '\0';
      };
   };

   /* allocates memory for line array */
   debug_printf(7, "   allocating memory for file index\n"); 
   count++;
   if (!(lines = (char **) malloc(sizeof(char *) * count)))
   {
      perror(PROGRAM_NAME ": read()");
      free(buff);
      return(NULL);
   };

   /* places lines in array */
   debug_printf(5, "   indexing file's contents\n"); 
   lines[0] = buff;
   count = 0;
   for(i = 0; i < len; i++)
   {
      if (buff[i] == '\0')
      {
         count++;
         lines[count] = &buff[i+1];
      };
   };
   lines[count] = NULL;

   /* ends function */
   return(lines);
}


/// main statement
/// @param[in]  argc  number of arguments passed to program
/// @param[in]  argv  array of arguments passed to program
int main(int argc, char * argv[])
{
   /* declares local vars */
   int           c;
   int           i;
   int           opt_index;
   Config        cnf;

   /* getopt options */
   static char   short_opt[] = "f:hl:r:tvV";
   static struct option long_opt[] =
   {
      {"help",          no_argument, 0, 'h'},
      {"test",          no_argument, 0, 't'},
      {"verbose",       no_argument, 0, 'v'},
      {"version",       no_argument, 0, 'V'},
      {NULL,            0,           0, 0  }
   };

   /* initialize variables */
   memset(&cnf, 0, sizeof(Config));
   cnf.leftTagString  = "@";
   cnf.rightTagString = "@";
   cnf.tagFile        = NULL;
   verbosity          = 0;
   opt_index          = 0;

   /* loops through arguments */
   while((c = getopt_long(argc, argv, short_opt, long_opt, &opt_index)) != -1)
   {
      switch(c)
      {
         case -1:	/* no more arguments */
         case 0:	/* long options toggles */
            break;

         case 'f':
            cnf.tagFile = optarg;
            break;
            
         case 'h':
            my_usage();
            return(0);
            
         case 'l':
            cnf.leftTagString = optarg;
            break;
            
         case 'r':
            cnf.rightTagString = optarg;
            break;
            
         case 't':
            test_mode = 1;
            break;
            
         case 'V':
            my_version();
            return(0);
            
         case 'v':
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

   /* verifies arguments were passed on the command line */
   if ((optind == argc) || (optind == 1))
   {
      my_usage();
      return(1);
   };
   if (!(cnf.tagFile ))
   {
      fprintf(stderr, "%s: missing required option `-f'\n", PROGRAM_NAME);
      fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
      return(1);
   };

   /* parses tag file */
   if ((generate_taglist(&cnf)))
      return(0);
   if (!(cnf.tagCount))
   {
      fprintf(stderr, PROGRAM_NAME ": no tag definitions were found in \"%s\"\n", cnf.tagFile);
      return(1);
   };

   /* loops through files to be tagged */
   for(i = optind; i < argc; i++)
      expand_tags(argv[i], &cnf);

   /* frees memory */
   free_taglist(cnf.tagList);

   /* ends function */
   return(0);
}


/// displays usage
void my_usage(void)
{
   printf("Usage: %s [OPTIONS] files\n", PROGRAM_NAME);
   printf("  -f file                   file containing tags\n");
   printf("  -h, --help                print this help and exit\n");
   printf("  -l str                    left enclosing string for tags\n");
   printf("  -r str                    right enclosing string for tags\n");
   printf("  -t, --test                show what would be done\n");
   printf("  -v, --verbose             print verbose messages\n");
   printf("  -V, --version             print version number and exit\n");
   printf("\n");
#ifdef PACKAGE_BUGREPORT
   printf("Report bugs to <%s>.\n", PACKAGE_BUGREPORT);
#endif
   return;
}


/// displays version information
void my_version(void)
{
#if defined(PACKAGE_NAME) && defined(PACKAGE_VERSION)
   printf("%s (%s) %s\n", PROGRAM_NAME, PACKAGE_NAME, PACKAGE_VERSION);
#else
   printf("%s\n", PROGRAM_NAME);
#endif
   printf("Written by David M. Syzdek.\n");
   printf("\n");
#if defined(PACKAGE_COPYRIGHT)
   printf("%s\n", PACKAGE_COPYRIGHT);
#endif
   printf("This is free software; see the source for copying conditions.  There is NO\n");
   printf("warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n");
   return;
}


/// processes tag from tag file
/// @param[in]  tagname   name of tag to process
/// @param[in]  data      data from which to extract the tag
/// @param[in]  len       length of data array
/// @param[in]  pos       current position within the data
/// @param[in]  taglistp  pointer to array of tags
int process_tag(char * tagName, char ** data, int len, int pos, Config * cnf)
{
   /* declares local vars */
   int           i;
   int           err;
   int           count;
   int           tagCount;
   int           line_count;
   TagData     * tag;
   void        * ptr;
   char          regstr[STR_LEN];
   char          errmsg[STR_LEN];
   regmatch_t    match[5];

   /* checks arguments */
   if (!(tagName))
      return(-1);
   if (!(data))
      return(-1);
   if (!(cnf))
      return(-1);
      
   memset(regstr, 0, STR_LEN);
   memset(errmsg, 0, STR_LEN);

   /* prints debugging information */
   debug_printf(5, "   indexing tag \"%s\"\n", tagName);

   /* counts tags in taglist */
   tagCount = 0;
   if (cnf->tagList)
      for(tagCount = 0; cnf->tagList[tagCount]; tagCount++);

   /* allocates memory for tag */
   tagCount++;
   if (!(tag = (TagData *) malloc(sizeof(TagData))))
   {
      fprintf(stderr, PROGRAM_NAME ": out of virtual memory\n");
      return(-1);
   };
   memset(tag, 0, sizeof(TagData));

   /* allocates memory for array */
   if (!(ptr = realloc(cnf->tagList, sizeof(TagData) * (tagCount + 1))))
   {
      fprintf(stderr, PROGRAM_NAME ": out of virtual memory\n");
      free_tag(tag);
      return(-1);
   };
   cnf->tagList             = ptr;
   cnf->tagList[tagCount-1] = NULL;
   cnf->tagList[tagCount]   = NULL;

   /* saves tag name */
   if (!(tag->name = strdup(tagName)))
   {
      fprintf(stderr, PROGRAM_NAME ": out of virtual memory\n");
      free_tag(tag);
      return(-1);
   };

   /* compiles regular expression */
   debug_printf(5, "   compiling regular expression\n");
   escape_string(regstr, cnf->leftTagString,  STR_LEN);
   strncat(regstr,       "(",                 STR_LEN);
   strncat(regstr,       tagName,             STR_LEN);
   strncat(regstr,       "END",               STR_LEN);
   strncat(regstr,       ")",                 STR_LEN);
   escape_string(regstr, cnf->rightTagString, STR_LEN);
   if ((err = regcomp(&tag->regex, regstr, REG_EXTENDED|REG_ICASE)))
   {
      regerror(err, &tag->regex, errmsg, STR_LEN-1);
      printf("regex error: %s\n", errmsg);
      free_tag(tag);
      return(-1);
   };

   /* loops through file */
   err = 1;
   for(count = pos; ((count < len) && (err)); count++)
      err = regexec(&tag->regex, data[count], 5, match, 0);

   /* checks for error */
   if (err)
   {
      free_tag(tag);
      return(-1);
   };

   /* calculate number of lines in tag */
   line_count = count - pos - 1;

   /* allocate memory for tag contents */
   if (!(tag->contents = (char **) malloc(sizeof(char *) * (line_count + 1))))
   {
      free_tag(tag);
      return(-1);
   };
   memset(tag->contents, 0, (sizeof(char *) * (line_count + 1)));

   /* copies lines into tag */
   for(i = pos; i < (pos + line_count); i++)
   {
      if (!(tag->contents[i-pos] = strdup(data[i])))
      {
         free_tag(tag);
         return(-1);
      };
   };

   /* saves tag */
   cnf->tagList[tagCount-1] = tag;

   /* ends function */
   return(i);
}


/* end of source file */
