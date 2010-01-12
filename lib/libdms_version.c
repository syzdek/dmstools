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

/// returns version info string for library
const char * dms_version(void)
{
   return("libdms.la (" PACKAGE_NAME ") " PACKAGE_VERSION);
}

/* end of source code */
