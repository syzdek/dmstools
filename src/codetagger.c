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
#include <dirent.h>
#include <getopt.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdarg.h>
#include <regex.h>
#include <errno.h>


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
#define PROGRAM_NAME "codetagger"
#endif
#ifndef PACKAGE_BUGREPORT
#define PACKAGE_BUGREPORT "david@syzdek.net"
#endif
#ifndef PACKAGE_NAME
#define PACKAGE_NAME ""
#endif
#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION ""
#endif

#define CODETAGGER_OPT_CONTINUE    0x0001
#define CODETAGGER_OPT_DEBUG       0x0002
#define CODETAGGER_OPT_FORCE       0x0004
#define CODETAGGER_OPT_HIDDEN      0x0008
#define CODETAGGER_OPT_LINKS       0x0010
#define CODETAGGER_OPT_QUIET       0x0020
#define CODETAGGER_OPT_RECURSE     0x0040
#define CODETAGGER_OPT_TEST        0x0080
#define CODETAGGER_OPT_VERBOSE     0x0100

#undef  CODETAGGER_STR_LEN
#define CODETAGGER_STR_LEN  512

#ifndef PARAMS
#define PARAMS(protos) protos
#endif


/////////////////
//             //
//  Datatypes  //
//             //
/////////////////

/// tag data
typedef struct codetagger_data CodeTaggerData;
struct codetagger_data
{
   char     * name;
   char    ** contents;
   regex_t    regex;
};


/// config data
typedef struct codetagger_config CodeTagger;
struct codetagger_config
{
   unsigned          opts;
   unsigned          tagCount;
   const char      * tagFile;
   const char      * leftTagString;
   const char      * rightTagString;
   CodeTaggerData ** tagList;
};


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////

// prints debug messages
#define codetagger_debug(cnf)           codetagger_debug_trace(cnf, __func__, NULL)
#define codetagger_debug_ext(cnf, ...)  codetagger_debug_trace(cnf, __func__, __VA_ARGS__)
void codetagger_debug_trace PARAMS((CodeTagger * cnf, const char * func,
   const char * fmt, ...));

// prints error message
void codetagger_error PARAMS((CodeTagger * cnf, const char * fmt, ...));

// creates a regex safe string (escapes chracters meaning full in regex)
int codetagger_escape_string PARAMS((CodeTagger * cnf, char * buff,
   const char * str, unsigned len));

// replaces original file with temp file and unlinks temp file name
int codetagger_file_link PARAMS((CodeTagger * cnf, FILE * fdout,
   const char * ofile, const char * tfile));

// opens temporary file for writing using mkstemp()
FILE * codetagger_file_open PARAMS((CodeTagger * cnf, const char * file,
   char * buff, int buff_len));

// writes formatted data to temp file if the program is not in test mode
void codetagger_file_printf PARAMS((CodeTagger * cnf, FILE * fdout,
   const char * fmt, ...));

// frees memory used to hold file contents
void codetagger_free_filedata PARAMS((CodeTagger * cnf, char ** lines));

// frees memory used store tag
void codetagger_free_tag PARAMS((CodeTagger * cnf, CodeTaggerData * tag));

// frees memory used store an array of tags
void codetagger_free_taglist PARAMS((CodeTagger * cnf, CodeTaggerData ** taglist));

// reads file into an array
char ** codetagger_get_file_contents PARAMS((CodeTagger * cnf, const char * file));

// generate array of tags from file
int codetagger_parse_tag_file PARAMS((CodeTagger * cnf));

// retrieves a specific tag from the tag list
CodeTaggerData * codetagger_retrieve_tag_data PARAMS((CodeTagger * cnf,
   const char * tagName, const char * fileName, int lineNumber));

// recursively scans directory for files to update
int codetagger_scan_directory PARAMS((CodeTagger * cnf, const char * origin));

// scan file to determine wether to attempt and update
int codetagger_scan_file PARAMS((CodeTagger * cnf, const char * file,
   unsigned * countp, char *** queuep, unsigned * sizep));

// updates original file by inserting/expanding tags
int codetagger_update_file PARAMS((CodeTagger * cnf, const char * filename));

// displays usage
void codetagger_usage PARAMS((void));

// prints verbose messages
void codetagger_verbose PARAMS((CodeTagger * cnf, const char * fmt, ...));

// displays version information
void codetagger_version PARAMS((void));

// extracts tag data from tag file
int codetagger_process_tag PARAMS((CodeTagger * cnf, char * name, char ** data,
   int len, int pos));

// main statement
int main PARAMS((int argc, char * argv[]));


/////////////////
//             //
//  Functions  //
//             //
/////////////////

/// prints debug messages
/// @param[in] cnf   pointer to config data structure
/// @param[in] func  name of calling function
/// @param[in] fmt   the format string of the message
/// @param[in] ...   the format arguments of the message
void codetagger_debug_trace(CodeTagger * cnf, const char * func, const char * fmt, ...)
{
   int     len;
   char    buff[1024];
   va_list ap;

   if (!(cnf))
      return;
   if (!(cnf->opts & CODETAGGER_OPT_DEBUG))
      return;
   if (!(fmt))
   {
      fprintf(stderr, "codetagger: %s\n", func);
      return;
   };
   va_start(ap, fmt);
      len = vsnprintf(buff, 1023, fmt, ap);
   va_end(ap);
   buff[1023] = '\0';
   if (!(func))
      fprintf(stderr, "codetagger: %s\n", buff);
   else
      fprintf(stderr, "codetagger: %s %s\n", func, buff);
   return;
}


/// prints error message
/// @param[in] cnf    pointer to config data structure
/// @param[in] fatal  name of calling function
/// @param[in] fmt    the format string of the message
/// @param[in] ...    the format arguments of the message
void codetagger_error(CodeTagger * cnf, const char * fmt, ...)
{
   int     len;
   char    buff[1024];
   va_list ap;

   codetagger_debug(cnf);

   if (!(fmt))
      return;
   if (cnf)
      if ((cnf->opts & CODETAGGER_OPT_CONTINUE) && (cnf->opts & CODETAGGER_OPT_QUIET))
         return;

   va_start(ap, fmt);
      len = vsnprintf(buff, 1023, fmt, ap);
   va_end(ap);
   buff[1023] = '\0';

   fprintf(stderr, PROGRAM_NAME ": %s", buff);

   return;
}


// creates a regex safe string (escapes chracters meaning full in regex)
/// @param[in] buff  buffer into which escape the string
/// @param[in] str   string to escape and concatenate from
/// @param[in] len   size of buffer
int codetagger_escape_string(CodeTagger * cnf, char * buff, const char * str, unsigned len)
{
   unsigned strpos;
   unsigned buffpos;

   codetagger_debug(cnf);

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


/// replaces original file with temp file and unlinks temp file name
/// @param[in] fdout  the file stream of the temp file
/// @param[in] ofile  the original file name
/// @param[in] tfile  the temporary file name    
int codetagger_file_link(CodeTagger * cnf, FILE * fdout, const char * ofile, const char * tfile)
{
   struct stat sb;

   codetagger_debug(cnf);

   if (!(fdout))
      return(0);
   fclose(fdout);

   if (!(tfile))
      return(0);

   if ( (tfile) && (!(ofile)) )
   {
      unlink(tfile); // removes temparary file
      return(0);
   };

   if (stat(ofile, &sb))
   {
      perror(PROGRAM_NAME ": stat()");
      return(-1);
   };

   if (chmod(tfile, sb.st_mode) == -1)
   {
      perror(PROGRAM_NAME ": chmod()");
      return(-1);
   };

   if (!(geteuid()))
   {
      if (chown(tfile, sb.st_uid, sb.st_gid) == -1)
      {
         perror(PROGRAM_NAME ": chown()");
         return(-1);
      };
   };

   if (unlink(ofile) == -1)
   {
      perror(PROGRAM_NAME ": unlink()");
      return(-1);
   };

   if (link(tfile, ofile) == -1)
   {
      perror(PROGRAM_NAME ": link()");
      return(-1);
   };

   if (unlink(tfile) == -1)
   {
      perror(PROGRAM_NAME ": unlink()");
      return(-1);
   };

   return(0);
}


/// opens temporary file for writing using mkstemp()
/// @params[in] file      file name of file to open for writing
/// @params[in] buff      buffer to store temporary file name
/// @params[in] buff_len  length of buffer
FILE * codetagger_file_open(CodeTagger * cnf, const char * file, char * buff, int buff_len)
{
   int    fd;
   FILE * fdout;

   codetagger_debug(cnf);

   buff[0] = '\0';

   if (cnf->opts & CODETAGGER_OPT_TEST)
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
void codetagger_file_printf(CodeTagger * cnf, FILE * fdout, const char * fmt, ...)
{
   va_list arg;

   codetagger_debug(cnf);
   if (cnf->opts & CODETAGGER_OPT_TEST)
      return;

   va_start (arg, fmt);
      vfprintf(fdout, fmt, arg);
   va_end (arg);

   return;
}


/// frees memory used to hold file contents
/// @param[in]  lines  array of lines to free
void codetagger_free_filedata(CodeTagger * cnf, char ** lines)
{
   if (!(lines))
      return;

   codetagger_debug(cnf);

   if (lines[0])
      free(lines[0]);
   lines[0] = NULL;

   free(lines);

   return;
}


/// frees memory used store tag
/// @param[in]  tag  tag data structure to free
void codetagger_free_tag(CodeTagger * cnf, CodeTaggerData * tag)
{
   int i;

   codetagger_debug(cnf);

   if (!(tag))
      return;

   if (tag->name)
   {
      codetagger_verbose(cnf, _("   freeing tag \"%s\"\n"), tag->name);
      free(tag->name);
   } else {
      codetagger_verbose(cnf, _("   freeing unknown tag\n"));
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
void codetagger_free_taglist(CodeTagger * cnf, CodeTaggerData ** taglist)
{
   int i;
   codetagger_debug(cnf);
   if (!(taglist))
      return;
   codetagger_verbose(cnf, _("freeing tag index\n"));
   for(i = 0; taglist[i]; i++)
      codetagger_free_tag(cnf, taglist[i]);
   free(taglist);
   return;
}


/// reads file into an array
/// @param[in]  file  file name of file to process
char ** codetagger_get_file_contents(CodeTagger * cnf, const char * file)
{
   /* declares local vars */
   int            i;
   int            fd;
   int            len;
   int            count;
   char         * buff;
   char        ** lines;
   struct stat    sb;

   codetagger_debug(cnf);

   /* prints debug information */
   codetagger_verbose(cnf, _("processing file \"%s\"\n"), file);

   /* stat file */
   codetagger_verbose(cnf, _("   inspecting file\n"));
   if ((stat(file, &sb)) == -1)
   {
      perror(PROGRAM_NAME ": stat()");
      return(NULL);
   };

   /* opens file for reading */
   codetagger_verbose(cnf, _("   opening file\n"));
   if ((fd = open(file, O_RDONLY)) == -1)
   {
      perror(PROGRAM_NAME ": open()");
      return(NULL);
   };

   /* allocates memory for buffer */
   codetagger_verbose(cnf, _("   allocating memory for file buffer\n"));
   if (!(buff = (char *) malloc(sb.st_size + 1)))
   {
      fprintf(stderr, PROGRAM_NAME ": out of virtual memory\n");
      close(fd);
      return(NULL);
   };

   /* reads file into buffer */
   codetagger_verbose(cnf, _("   reading file\n"));
   if ((len = read(fd, buff, sb.st_size)) == -1)
   {
      perror(PROGRAM_NAME ": read()");
      close(fd);
      return(NULL);
   };
   buff[len] = '\0';

   /* closes file */
   codetagger_verbose(cnf, _("   closing file\n"));
   close(fd);

   /* counts and terminates lines */
   codetagger_verbose(cnf, _("   analyzing file contents\n"));
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
   codetagger_verbose(cnf, _("   allocating memory for file index\n"));
   count++;
   if (!(lines = (char **) malloc(sizeof(char *) * count)))
   {
      perror(PROGRAM_NAME ": read()");
      free(buff);
      return(NULL);
   };

   /* places lines in array */
   codetagger_verbose(cnf, _("   indexing file's contents\n"));
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


/// generate array of tags from file
/// @param[in] file  file name of file which contains tag definitions
int codetagger_parse_tag_file(CodeTagger * cnf)
{
   int           i;
   int           err;
   int           lineCount;
   char          regstr[CODETAGGER_STR_LEN];
   char          errmsg[CODETAGGER_STR_LEN];
   char          tagName[CODETAGGER_STR_LEN];
   char       ** data;
   regex_t       regex;
   regmatch_t    match[5];

   codetagger_debug(cnf);

   memset(regstr,   0, CODETAGGER_STR_LEN);
   memset(errmsg,   0, CODETAGGER_STR_LEN);
   memset(tagName,  0, CODETAGGER_STR_LEN);

   if (!(data = codetagger_get_file_contents(cnf, cnf->tagFile)))
      return(-1);

   for(lineCount = 0; data[lineCount]; lineCount++);

   // compiles regex used to locate tags
   codetagger_verbose(cnf, _("   compiling regular expression for tags\n"));
   strncat(regstr,       "^",              CODETAGGER_STR_LEN);
   codetagger_escape_string(cnf, regstr, cnf->leftTagString,  CODETAGGER_STR_LEN);
   strncat(regstr,       "([_[:blank:][:alnum:]]+)START",   CODETAGGER_STR_LEN);
   codetagger_escape_string(cnf, regstr, cnf->rightTagString, CODETAGGER_STR_LEN);
   if ((err = regcomp(&regex, regstr, REG_EXTENDED|REG_ICASE)))
   {
      regerror(err, &regex, errmsg, CODETAGGER_STR_LEN-1);
      printf("regex error: %s\n", errmsg);
      return(-1);
   };
   codetagger_verbose(cnf, _("   compiled regular expression \"%s\"\n"), regstr);

   // loops through looking for tags
   codetagger_verbose(cnf, _("creating tag index from \"%s\"\n"), cnf->tagFile);
   for(i = 0; i < lineCount; i++)
   {
      if (!(err = regexec(&regex, data[i], 5, match, 0)))
      {
         data[i][(int)match[1].rm_eo] = '\0';
         strncpy(tagName, &data[i][(int)match[1].rm_so], CODETAGGER_STR_LEN-1);
         i++;
         if ((i = codetagger_process_tag(cnf, tagName, data, lineCount, i)) == -1)
         {
            codetagger_free_filedata(cnf, data);
            regfree(&regex);
            return(-1);
         };
         cnf->tagCount++;
      };
   };

   codetagger_verbose(cnf, _("   freeing file buffer\n"));
   codetagger_free_filedata(cnf, data);
   codetagger_verbose(cnf, _("   freeing regular expressions\n"));
   regfree(&regex);

   return(0);
}


/// retrieves a specific tag from the tag list
/// @param[in] name name of tag to find
/// @param[in] taglist array of tags to search
/// @param[in] line_number line number of the current file being processed
CodeTaggerData * codetagger_retrieve_tag_data(CodeTagger * cnf, const char * tagName,
   const char * fileName, int lineNumber)
{
   int i;

   codetagger_debug(cnf);

   if (!(cnf))
      return(NULL);
   if (!(cnf->tagList))
      return(NULL);
   if (!(tagName))
      return(NULL);
   if (!(fileName))
      return(NULL);

   codetagger_verbose(cnf, _("   retrieving tag \"%s\"\n"), tagName);

   for(i = 0; cnf->tagList[i]; i++)
      if (!(strcasecmp(tagName, cnf->tagList[i]->name)))
         return(cnf->tagList[i]);

   codetagger_verbose(cnf, _(PROGRAM_NAME ": %s: %i: unknown tag \"%s\"\n"), fileName, lineNumber, tagName);

   return(NULL);
}


/// recursively scans directory for files to update
/// @param[in] cnf   pointer to config data structure
int codetagger_scan_directory(CodeTagger * cnf, const char * origin)
{
   int             err;
   DIR           * d;
   char          * file;
   char         ** queue;
   void          * ptr;
   size_t          len_dir;
   size_t          len_file;
   unsigned        size;
   unsigned        count;
   struct dirent * dp;

   size  = 0;
   count = 0;
   ptr   = NULL;
   queue = NULL;

   // seeds queue with first file/directory
   if ((err = codetagger_scan_file(cnf, origin, &count, &queue, &size)))
      return(err);

   if (!(cnf->opts & CODETAGGER_OPT_RECURSE))
   {
      if (queue)
      {
         free(queue[0]);
         free(queue);
      };
      return(0);
   };

   while(count)
   {
      if (cnf->opts & CODETAGGER_OPT_VERBOSE)
         printf("%s\n", queue[count-1]);

      // opens directory for processing
      if (!(d = opendir(queue[count-1])))
      {
         codetagger_error(cnf, "%s: %s\n", queue[count-1], strerror(errno));
         if (!(cnf->opts & CODETAGGER_OPT_CONTINUE))
            return(1);
      };

      len_dir = strlen(queue[count-1]);

      for(dp = readdir(d); dp; dp = readdir(d))
      {
         len_file = strlen(dp->d_name);
         if (!(file = malloc(sizeof(char) * (len_dir + len_file + 2))))
         {
            fprintf(stderr, "%s: out of virtual memory\n", PROGRAM_NAME);
            return(-1);
         };
         sprintf(file, "%s/%s", queue[count-1], dp->d_name);

         if ( (dp->d_name[0] == '.') && (cnf->opts & CODETAGGER_OPT_HIDDEN) )
         {
            if ( (dp->d_name[1] != '/') &&
                 (dp->d_name[1] != '\0') &&
                 (dp->d_name[1] != '.') )
            {
               switch (err = codetagger_scan_file(cnf, file, &count, &queue, &size))
               {
                  case -1:
                     free(file);
                     closedir(d);
                     return(1);
                  case 0:
                     free(file);
                     break;
                  default:
                     free(file);
                     if (!(cnf->opts & CODETAGGER_OPT_CONTINUE))
                     {
                        closedir(d);
                        return(1);
                     };
                     break;
               };
            };
         }
         else if (dp->d_name[0] != '.')
         {
            switch (err = codetagger_scan_file(cnf, file, &count, &queue, &size))
            {
               case -1:
                  free(file);
                  closedir(d);
                  return(1);
               case 0:
                  free(file);
                  break;
               default:
                  free(file);
                  if (!(cnf->opts & CODETAGGER_OPT_CONTINUE))
                  {
                     closedir(d);
                     return(1);
                  };
                  break;
            };
         };
      };

      // closes directory and removes from queue
      closedir(d);
      count--;
      free(queue[count]);
   };

   free(queue);

   return(0);
}


/// scan file to determine wether to attempt and update
/// @param[in] cnf   pointer to config data structure
int codetagger_scan_file(CodeTagger * cnf, const char * file,
   unsigned * countp, char *** queuep, unsigned * sizep)
{
   int             err;
   void          * ptr;
   unsigned        u;
   struct stat     sb;

   if (cnf->opts & CODETAGGER_OPT_LINKS)
      err = stat(file, &sb);
   else
      err = lstat(file, &sb);
   if (err == -1)
   {
      codetagger_error(cnf, "%s: %s\n", file, strerror(errno));
      return(1);
   };
   switch(sb.st_mode & (S_IFDIR|S_IFREG|S_IFLNK))
   {
      case S_IFDIR:
         // adjust size of array
         if (*countp >= *sizep)
         {
            *sizep += 20;
            if (!(ptr = realloc(*queuep, sizeof(char *) * (*sizep))))
            {
               fprintf(stderr, "%s: out of virtual memory\n", PROGRAM_NAME);
               return(-1);
            };
            *queuep = ptr;
         };

         // copy path name
         if (!(ptr = strdup(file)))
         {
            fprintf(stderr, "%s: out of virtual memory\n", PROGRAM_NAME);
            return(-1);
         };

         // insert path name at head of array
         for(u = (*countp); u > 0; u--)
            (*queuep)[u] = (*queuep)[u-1];
         (*queuep)[0] = ptr;
         (*countp)++;
         break;

      case S_IFLNK:  // ignore symbolic links
         break;

      case S_IFREG:
         if (cnf->opts & CODETAGGER_OPT_VERBOSE)
            printf("%s\n", file);
         return(codetagger_update_file(cnf, file));
         break;

      default:
         codetagger_error(cnf, "%s: unknown file type\n", file);
         return(1);
         break;
   };

   return(0);
}


/// updates original file by inserting/expanding tags
/// @param[in]  file  name of file to process
/// @param[in]  cnf   array of tags to insert into new file
int codetagger_update_file(CodeTagger * cnf, const char * filename)
{
   /* declares local vars */
   int           data_pos;
   int           data_len;
   int           tag_pos;
   int           err;
   unsigned      tagCount;
   CodeTaggerData     * tag;
   char       ** data;
   char          tmpfile[CODETAGGER_STR_LEN];
   char          margin[CODETAGGER_STR_LEN];
   char          errmsg[CODETAGGER_STR_LEN];
   char          tagName[CODETAGGER_STR_LEN];
   char          regstr[CODETAGGER_STR_LEN];
   char          tagend[CODETAGGER_STR_LEN];
   FILE        * fdout;
   regex_t       regex;
   regmatch_t    match[5];

   codetagger_debug(cnf);

   /* initialzes data */
   fdout      = NULL;
   tagCount   = 0;
   tmpfile[0] = '\0';
   memset(regstr, 0, CODETAGGER_STR_LEN);

   /* reads file and stores contents */
   if (!(data = codetagger_get_file_contents(cnf, filename)))
      return(1);

   /* creates temp file */
   if (!(cnf->opts & CODETAGGER_OPT_TEST))
   {
      if (!(fdout = codetagger_file_open(cnf, filename, tmpfile, CODETAGGER_STR_LEN)))
      {
         codetagger_free_filedata(cnf, data);
         return(1);
      };
   };

   /* compiles regex used to locate tags */
   codetagger_verbose(cnf, _("   compiling regular expression\n"));
   strncat(regstr,       "(.*)",              CODETAGGER_STR_LEN);
   codetagger_escape_string(cnf, regstr, cnf->leftTagString,  CODETAGGER_STR_LEN);
   strncat(regstr,       "([_[:blank:][:alnum:]]+)START",   CODETAGGER_STR_LEN);
   codetagger_escape_string(cnf, regstr, cnf->rightTagString, CODETAGGER_STR_LEN);
   if ((err = regcomp(&regex, regstr, REG_EXTENDED|REG_ICASE)))
   {
      regerror(err, &regex, errmsg, CODETAGGER_STR_LEN-1);
      printf("regex error: %s\n", errmsg);
      codetagger_file_link(cnf, fdout, NULL, tmpfile);
      codetagger_free_filedata(cnf, data);
      return(0);
   };
   codetagger_verbose(cnf, _("   compiled regular expression \"%s\"\n"), regstr);

   /* counts lines in data */
   for(data_len = 0; data[data_len]; data_len++);

   /* loops through file */
   codetagger_verbose(cnf, _("   expanding tags\n"));
   for(data_pos = 0; data_pos < data_len; data_pos++)
   {
      /* copies the old file into the new file */
      codetagger_file_printf(cnf, fdout, "%s\n", data[data_pos]);

      /* checks line for a tag */
      if ((err = regexec(&regex, data[data_pos], 5, match, 0)))
         continue;

      /* copies tag information into buffers */
      memset(margin,  0, CODETAGGER_STR_LEN);
      memset(tagName, 0, CODETAGGER_STR_LEN);
      memcpy(margin,  &data[data_pos][(int)match[1].rm_so], match[1].rm_eo - match[1].rm_so);
      memcpy(tagName, &data[data_pos][(int)match[2].rm_so], match[2].rm_eo - match[2].rm_so);

      /* applies the tag to the new file */
      if ((tag = codetagger_retrieve_tag_data(cnf, tagName, filename, data_pos+1)))
      {
         /* writes contents of the tag */
         codetagger_verbose(cnf, _("   inserting tag \"%s\" (line %i)\n"), tagName, data_pos);
         for(tag_pos = 0; tag->contents[tag_pos]; tag_pos++)
            codetagger_file_printf(cnf, fdout, "%s%s\n", margin, tag->contents[tag_pos]);

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
            fprintf(stderr, _("%s: missing end @%sEND@ in %s\n"), PROGRAM_NAME, tagName, filename);
            codetagger_file_link(cnf, fdout, NULL, tmpfile);
            codetagger_free_filedata(cnf, data);
            return(-1);
         };

         /* copies end tag into buffer */
         memset(tagend, 0, CODETAGGER_STR_LEN);
         memcpy(tagend,  &data[data_pos][(int)match[1].rm_so], match[1].rm_eo - match[1].rm_so);

         /* prints end tag */
         codetagger_file_printf(cnf, fdout, "%s%s%s%s\n", margin, cnf->leftTagString,
                     tagend, cnf->rightTagString);

         /* counts tags */
         tagCount++;
      };
   };

   /* closes and saves file */
   if (!(tagCount))
   {
      fprintf(stderr, PROGRAM_NAME ": no tags were found in \"%s\"\n", filename);
      codetagger_file_link(cnf, fdout, NULL, tmpfile);
   } else {
      codetagger_file_link(cnf, fdout, filename, tmpfile);
   };

   /* frees memory */
   codetagger_free_filedata(cnf, data);

   /* ends function */
   return(0);
}


/// displays usage
void codetagger_usage(void)
{
   // TRANSLATORS: The following strings provide usage for command. These
   // strings are displayed if the program is passed `--help' on the command
   // line. The two strings referenced are: PROGRAM_NAME, and
   // PACKAGE_BUGREPORT
   printf(_("Usage: %s [OPTIONS] files\n"
         "  -a                        include hidden files\n"
         "  -c                        continue on error\n"
         "  -d                        enter debug mode\n"
         "  -f                        force writes\n"
         "  -h, --help                print this help and exit\n"
         "  -i file                   file containing tags\n"
         "  -L                        follow symbolic links\n"
         "  -l str                    left enclosing string for tags\n"
         "  -q, --quiet, --silent     do not print messages\n"
         "  -r str                    right enclosing string for tags\n"
         "  -R                        recursively follow directories\n"
         "  -t, --test                show what would be done\n"
         "  -v, --verbose             print verbose messages\n"
         "  -V, --version             print version number and exit\n"
         "\n"
         "Report bugs to <%s>.\n"
      ), PROGRAM_NAME, PACKAGE_BUGREPORT
   );
   return;
}


/// prints verbose messages
/// @param[in] cnf   pointer to config data structure
/// @param[in] fmt   the format string of the message
/// @param[in] ...   the format arguments of the message
void codetagger_verbose(CodeTagger * cnf, const char * fmt, ...)
{
   va_list arg;

   if (!(cnf))
      return;
   if (!(cnf->opts & CODETAGGER_OPT_VERBOSE))
      return;

   va_start (arg, fmt);
      vprintf(fmt, arg);
   va_end (arg);

   return;
}


/// displays version information
void codetagger_version(void)
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


/// processes tag from tag file
/// @param[in]  tagname   name of tag to process
/// @param[in]  data      data from which to extract the tag
/// @param[in]  len       length of data array
/// @param[in]  pos       current position within the data
/// @param[in]  taglistp  pointer to array of tags
int codetagger_process_tag(CodeTagger * cnf, char * tagName, char ** data, int len, int pos)
{
   /* declares local vars */
   int           i;
   int           err;
   int           count;
   int           tagCount;
   int           line_count;
   CodeTaggerData     * tag;
   void        * ptr;
   char          regstr[CODETAGGER_STR_LEN];
   char          errmsg[CODETAGGER_STR_LEN];
   regmatch_t    match[5];

   codetagger_debug(cnf);
   codetagger_verbose(cnf, "   indexing tag \"%s\"\n", tagName);

   if (!(tagName))
      return(-1);
   if (!(data))
      return(-1);
   if (!(cnf))
      return(-1);
      
   memset(regstr, 0, CODETAGGER_STR_LEN);
   memset(errmsg, 0, CODETAGGER_STR_LEN);

   // counts tags in taglist
   tagCount = 0;
   if (cnf->tagList)
      for(tagCount = 0; cnf->tagList[tagCount]; tagCount++);

   // allocates memory for tag
   tagCount++;
   if (!(tag = (CodeTaggerData *) malloc(sizeof(CodeTaggerData))))
   {
      fprintf(stderr, _(PROGRAM_NAME ": out of virtual memory\n"));
      return(-1);
   };
   memset(tag, 0, sizeof(CodeTaggerData));

   // allocates memory for array
   if (!(ptr = realloc(cnf->tagList, sizeof(CodeTaggerData) * (tagCount + 1))))
   {
      fprintf(stderr, _(PROGRAM_NAME ": out of virtual memory\n"));
      codetagger_free_tag(cnf, tag);
      return(-1);
   };
   cnf->tagList             = ptr;
   cnf->tagList[tagCount-1] = NULL;
   cnf->tagList[tagCount]   = NULL;

   // saves tag name
   if (!(tag->name = strdup(tagName)))
   {
      fprintf(stderr, _(PROGRAM_NAME ": out of virtual memory\n"));
      codetagger_free_tag(cnf, tag);
      return(-1);
   };

   // compiles regular expression
   codetagger_verbose(cnf, "   compiling regular expression\n");
   codetagger_escape_string(cnf, regstr, cnf->leftTagString,  CODETAGGER_STR_LEN);
   strncat(regstr,       "(",                 CODETAGGER_STR_LEN);
   strncat(regstr,       tagName,             CODETAGGER_STR_LEN);
   strncat(regstr,       "END",               CODETAGGER_STR_LEN);
   strncat(regstr,       ")",                 CODETAGGER_STR_LEN);
   codetagger_escape_string(cnf, regstr, cnf->rightTagString, CODETAGGER_STR_LEN);
   if ((err = regcomp(&tag->regex, regstr, REG_EXTENDED|REG_ICASE)))
   {
      regerror(err, &tag->regex, errmsg, CODETAGGER_STR_LEN-1);
      printf("regex error: %s\n", errmsg);
      codetagger_free_tag(cnf, tag);
      return(-1);
   };

   // loops through file
   err = 1;
   for(count = pos; ((count < len) && (err)); count++)
      err = regexec(&tag->regex, data[count], 5, match, 0);

   // checks for error
   if (err)
   {
      codetagger_free_tag(cnf, tag);
      return(-1);
   };

   // calculate number of lines in tag
   line_count = count - pos - 1;

   // allocate memory for tag contents
   if (!(tag->contents = (char **) malloc(sizeof(char *) * (line_count + 1))))
   {
      codetagger_free_tag(cnf, tag);
      return(-1);
   };
   memset(tag->contents, 0, (sizeof(char *) * (line_count + 1)));

   // copies lines into tag
   for(i = pos; i < (pos + line_count); i++)
   {
      if (!(tag->contents[i-pos] = strdup(data[i])))
      {
         codetagger_free_tag(cnf, tag);
         return(-1);
      };
   };

   cnf->tagList[tagCount-1] = tag;

   return(i);
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
   CodeTagger        cnf;

   /* getopt options */
   static char   short_opt[] = "acdfhi:Ll:qRr:tvV";
   static struct option long_opt[] =
   {
      {"continue",      no_argument, 0, 'c'},
      {"help",          no_argument, 0, 'h'},
      {"silent",        no_argument, 0, 'q'},
      {"quiet",         no_argument, 0, 'q'},
      {"test",          no_argument, 0, 't'},
      {"verbose",       no_argument, 0, 'v'},
      {"version",       no_argument, 0, 'V'},
      {NULL,            0,           0, 0  }
   };

#ifdef HAVE_GETTEXT
   setlocale (LC_ALL, "");
   bindtextdomain (PACKAGE, LOCALEDIR);
   textdomain (PACKAGE);
#endif

   /* initialize variables */
   memset(&cnf, 0, sizeof(CodeTagger));
   cnf.leftTagString  = "@";
   cnf.rightTagString = "@";
   cnf.tagFile        = NULL;
   opt_index          = 0;

   /* loops through arguments */
   while((c = getopt_long(argc, argv, short_opt, long_opt, &opt_index)) != -1)
   {
      switch(c)
      {
         case -1:	/* no more arguments */
         case 0:	/* long options toggles */
            break;
         case 'a':
            cnf.opts |= CODETAGGER_OPT_HIDDEN;
            break;
         case 'c':
            cnf.opts |= CODETAGGER_OPT_CONTINUE;
            break;
         case 'd':
            cnf.opts |= CODETAGGER_OPT_DEBUG;
            break;
         case 'f':
            cnf.opts |= CODETAGGER_OPT_FORCE;
            break;
         case 'h':
            codetagger_usage();
            return(0);
         case 'i':
            cnf.tagFile = optarg;
            break;
         case 'L':
            cnf.opts |= CODETAGGER_OPT_LINKS;
            break;
         case 'l':
            cnf.leftTagString = optarg;
            break;
         case 'q':
            cnf.opts |= CODETAGGER_OPT_QUIET;
            cnf.opts &= cnf.opts & (~CODETAGGER_OPT_VERBOSE);
            break;
         case 'R':
            cnf.opts |= CODETAGGER_OPT_RECURSE;
            break;
         case 'r':
            cnf.rightTagString = optarg;
            break;
         case 't':
            cnf.opts |= CODETAGGER_OPT_TEST;
            break;
         case 'V':
            codetagger_version();
            return(0);
         case 'v':
            cnf.opts |= CODETAGGER_OPT_VERBOSE;
            cnf.opts &= cnf.opts & (~CODETAGGER_OPT_QUIET);
            break;
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

   /* verifies arguments were passed on the command line */
   if ((optind == argc) || (optind == 1))
   {
      codetagger_usage();
      return(1);
   };
   if (!(cnf.tagFile ))
   {
      fprintf(stderr, _("%s: missing required option `-f'\n"
            "Try `%s --help' for more information.\n"
         ), PROGRAM_NAME, PROGRAM_NAME
      );
      return(1);
   };

   codetagger_debug(&cnf);

   /* parses tag file */
   if ((codetagger_parse_tag_file(&cnf)))
      return(0);
   if (!(cnf.tagCount))
   {
      fprintf(stderr, _(PROGRAM_NAME ": no tag definitions were found in \"%s\"\n"), cnf.tagFile);
      return(1);
   };

   // loops through files to be tagged
   for(i = optind; i < argc; i++)
      switch (codetagger_scan_directory(&cnf, argv[i]))
      {
         case -1: return(1);
         case 0:  break;
         default:
            if (!(cnf.opts & CODETAGGER_OPT_CONTINUE))
               return(1);
            break;
      };

   /* frees memory */
   codetagger_free_taglist(&cnf, cnf.tagList);

   /* ends function */
   return(0);
}


/* end of source file */
