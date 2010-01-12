/*
 *  DMS Tools and Utilities
 *  Copyright (C) 2010 David M. Syzdek <david@syzdek.net>.
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
