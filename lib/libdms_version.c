/*
 *  DMS Tools and Utilities
 *  Copyright (C) 2010 David M. Syzdek <david@syzdek.net>.
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
 *  @file lib/dms_version.c functions for querying information about the library 
 */
#define _DMS_LIB_LIBDMS_VERSION_C 1
#include "libdms_version.h"

///////////////
//           //
//  Headers  //
//           //
///////////////

#include "libdms.h"


/////////////////
//             //
//  Functions  //
//             //
/////////////////

/// returns the binary GNU Libtool LIB_VERSION_INFO value
int dms_lib_version(void)
{
   return(DMS_LIB_VERSION);
}


/// returns the GNU Libtool LIB_VERSION_AGE value
int dms_lib_version_age(void)
{
   return(DMS_LIB_VERSION_AGE);
}


/// checks for library binary compatability
/// @param[in]  info    version info to compare
int dms_lib_version_check(int version)
{
   int lib_cur = ((DMS_LIB_VERSION_CURRENT  >> 16) & 0xFFFF);
   int lib_age = ((DMS_LIB_VERSION_AGE      >>  0) & 0xFF);
   int ver_cur = ((version >> 16) & 0xFFFF);
   int ver_age = ((version >>  0) & 0xFF);
   if (ver_cur < (lib_cur-lib_age))
      return(1);
   if (lib_cur < (ver_cur-ver_age))
      return(1);
   return(0);
}


/// returns the GNU Libtool LIB_VERSION_CURRENT value
int dms_lib_version_current(void)
{
   return(DMS_LIB_VERSION_CURRENT);
}


/// returns the GNU Libtool LIB_VERSION_INFO string
const char * dms_lib_version_info(void)
{
   return(DMS_LIB_VERSION_INFO);
}


/// returns the GNU Libtool LIB_VERSION_REVISION value
int dms_lib_version_revision(void)
{
   return(DMS_LIB_VERSION_REVISION);
}


/// returns version info string for library
const char * dms_version(void)
{
   return("libdms.la (" PACKAGE_NAME ") " DMS_LIB_VERSION_INFO);
}

/* end of source code */
