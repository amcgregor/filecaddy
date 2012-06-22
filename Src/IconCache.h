#ifndef _ICONCACHE_H_
#define _ICONCACHE_H_

// $Id: IconCache.h,v 1.5 2002/07/25 15:09:00 amcgregor Exp $

// Copyright (c) 2002, Kopsis, Inc.

// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the
// Free Software Foundation; either version 2 of the License, or (at your
// option) any later version.

// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.

// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

// $Log: IconCache.h,v $
// Revision 1.5  2002/07/25 15:09:00  amcgregor
// Second thought: tabs bad.
//
// Revision 1.4  2002/07/25 11:51:53  amcgregor
// Code run through: astyle --style=kr --indent=tab=2 -S *.c *.h
//
// Revision 1.3  2002/07/16 00:59:43  amcgregor
// Initial public release, bugfixes, an aborted attempt at using the IconCache.
//
// Revision 1.2  2002/07/13 22:16:13  amcgregor
// Offline version to CVS miscelaneous updates.
//
// Revision 1.1  2002/07/11 14:46:55  root
// Initial revision
//


// - DEFINES -----------------------------------------------------------------
#define DIRECTORY_CREATOR ' DIR'
#define DIRECTORY_TYPE    ' DIR'
#define FILE_CREATOR      ' N/A'
#define FILE_TYPE         ' N/A'


// - STRUCTURES --------------------------------------------------------------
typedef struct ICONCACHE_T {
  UInt32 creator;
  MemHandle iconH;
  BitmapPtr icon;
  struct ICONCACHE_T *prev;
  struct ICONCACHE_T *next;
}
IconCacheType;


// - EXPORTED GLOBAL VARIABLES -----------------------------------------------
//extern IconCacheType *IconCache;
//extern IconCacheType *CacheLast;
//extern IconCacheType *CacheEnd;


// - EXPORTED FUNCTIONS ------------------------------------------------------
Err Cache_Init ( void );
Err Cache_Delete ( void );
IconCacheType *Cache_Hit ( UInt32 creator, UInt32 type );
Err Cache_FreeBitmap ( IconCacheType *ch );


#endif // _ICONCACHE_H_
