#include <PalmOS.h>
#include "Resource.h"
#include "Globals.h"
#include "IconCache.h"

// $Id: IconCache.c,v 1.5 2002/07/25 15:09:00 amcgregor Exp $

// Copyright (c) 2002, Alice Bevan-McGregor

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

// $Log: IconCache.c,v $
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


// - GLOBAL VARIABLES --------------------------------------------------------
IconCacheType *IconCache;
IconCacheType *CacheLast;
UInt32         CacheSize;

IconCacheType *FolderIcon;
MemHandle      FolderIconH, TextFileIconH, FileIconH;
BitmapPtr      FolderIconB, TextFileIconB, FileIconB;
//IconCacheType *FileIcon;      // not needed - there is a simpler way
//IconCacheType *TextFileIcon;

Err            CacheError;

// - INTERNAL FUNCTIONS ------------------------------------------------------

#define WinDebugAlert(m) showDebugInfo(__FILE__, __LINE__, m)
void showDebugInfo(const char *file, UInt32 lineNo, char *msg);

// - EXPORTED FUNCTIONS ------------------------------------------------------

// Function:    Cache_Init
// Description: Init the icon cache, and load the folder icon.
// Arguments:   None.
// Returns:     errnone if there was no error, the error code if otherwise.
Err Cache_Init ( void ) {
  //  IconCacheType *p;

  //  WinDebugAlert("Init, Top");

  //  p = (IconCacheType *)MemPtrNew ( sizeof(IconCacheType) );
  //  if ( !p ) return memErrNotEnoughSpace;
  //  MemSet ( p, sizeof(IconCacheType), 0 );

  //  p->creator = DIRECTORY_CREATOR;
  //  p->iconH = DmGetResource( 'Tbmp', FOLDER_BITMAP );
  //  if ( p->iconH ) p->icon = MemHandleLock ( p->iconH );
  //  p->prev    = NULL;
  //  p->next    = NULL;

  //  FolderIcon = p;
  //  IconCache  = p;
  //  CacheLast  = p;
  //  CacheSize  = 1;

  // It's easier to handle it this way, then to create more default entries in the cache.
  // This also makes it easier for an application to override the default icon.
  FolderIconH   = DmGetResource ( bitmapRsc, FOLDER_BITMAP );
  FolderIconB   = (BitmapType *)MemHandleLock ( FolderIconH );
  FileIconH     = DmGetResource ( bitmapRsc, FILE_BITMAP );
  FileIconB     = (BitmapType *)MemHandleLock ( FileIconH );
  TextFileIconH = DmGetResource ( bitmapRsc, TEXTFILE_BITMAP );
  TextFileIconB = (BitmapType *)MemHandleLock ( TextFileIconH );

  //  WinDebugAlert("Init, Bottom");

  return errNone;
}

// Function:    Cache_Delete
// Description: Remove the icon cache from memory.
// Arguments:   None.
// Returns:     errNone if there was no error, the error code if otherwise.
Err Cache_Delete ( void ) {
  //  IconCacheType *p, *q;
  //  Err            err;

  //  WinDebugAlert("Delete, Top");

  //  for ( p = IconCache, q = p; p; q = p, p = p->next ) {
  //    DmReleaseResource(q->iconH);
  //  q->next = NULL;
  //  if ( ( err = MemSet ( q, MemPtrSize ( q ), 0 ) ) )
  //    return err;
  //  if ( ( err = MemPtrFree ( q ) ) )
  //    return err;
  //}

  //IconCache = NULL;
  //CacheSize = 0;

  return errNone;
}

// Function:    Cache_Grow
// Description: Add a new icon to the icon cache.
// Arguments:   UInt32 type - the type ID of the file ('appl')
//              UInt32 creator - the creator ID of the file ('kILA')
// Returns:     A pointer to the new icon in cache.
IconCacheType *Cache_Grow ( UInt32 type, UInt32 creator ) {
  IconCacheType *p = NULL;
  Err            err = errNone;
  DmOpenRef      dbOR;
  MemHandle      iconH = 0;

  WinDebugAlert("Grow, Top");

  p = (IconCacheType *)MemPtrNew ( sizeof(IconCacheType) );
  if ( !p ) {
    WinDebugAlert("memErrNotEnoughSpace");
    CacheError = memErrNotEnoughSpace;
    return NULL;
  }
  if ( ( err = MemSet ( p, sizeof(IconCacheType), 0 ) ) ) {
    WinDebugAlert("Error in MemSet");
    CacheError = err;
    return NULL;
  }

  // Find the icon resource and add it to the cache entry.
  dbOR = DmOpenDatabaseByTypeCreator ( 'appl', creator, dmModeReadOnly );
  if ( dbOR ) {
    WinDebugAlert("Found application...");

    // Allow applications to have "file" icons.
    if ( type != 'appl' )
      p->iconH = DmGetResourceIndex ( dbOR, DmFindResource ( dbOR, 'tAIB', 9000, NULL ) );

    // If the file -is- an application, or has no "file" icon...
    if ( !p->iconH )
      p->iconH = DmGetResourceIndex ( dbOR, DmFindResource ( dbOR, 'tAIB', 1001, NULL ) );

    if ( iconH ) {
      WinDebugAlert("Icon found.");

      p->creator = creator;
      p->iconH = iconH;
      p->prev  = NULL;
      p->next  = IconCache;
      IconCache = p;
      CacheSize++;
      return p;
    }
  }

  if ( type == 'appl' ) {
    // TODO: Display a generic application icon...
    WinDebugAlert("Generic application icon...");

  } else if ( ( creator == 'REAd' ) && ( type == 'TEXt' ) ) {
    WinDebugAlert("TextFileIcon");
    p->iconH = TextFileIconH;
    p->creator = 'REAd';
    p->prev  = NULL;
    p->next  = IconCache;
    IconCache = p;
    CacheSize++;
    return p;

  } else {
    WinDebugAlert("FileIcon");
    p->iconH = FileIconH;
    p->creator = 'FILE';
    p->prev  = NULL;
    p->next  = IconCache;
    IconCache = p;
    CacheSize++;
    return p;

  }

  return NULL;
}

// Function:    Cache_Hit
// Description: Find the icon in the cache, if not found, create it.
// Arguments:   The creator and type ID of the file.
// Returns:     A pointer into the cache.
IconCacheType *Cache_Hit ( UInt32 creator, UInt32 type ) {
  IconCacheType *p = IconCache, *q = IconCache;

  WinDebugAlert("Hit, Top");

  ErrFatalDisplayIf(creator==0, "Nil creator ID");
  ErrFatalDisplayIf(type==0, "Nil type ID");
  //  ErrFatalDisplayIf(!IconCache, "Nil cache");                 // not needed, algorithm takes care of it.
  //  ErrNonFatalDisplayIf(!CacheLast, "Nil previous cache hit"); // Not needed, vestigal variable.

  for ( p = IconCache; p; p = p->next ) {
    if ( p->creator == creator ) {
      WinDebugAlert("Hit found!");

      // If p's not already the head, move it to the head.
      if ( p->prev ) {
        // Reconnect around p first.  If p's not last, then set p->next to
        // p->prev, currently known as q
        q = p->prev;
        q->next = p->next;
        if (q->next)
          q->next->prev = q;

        // Since p's disconnected, we can safely alter its next/prev.  NULL
        // the prev, and set the next to the current IconCache.  Then point
        // the present IconCache's prev at p and make p the new IconCache.
        p->prev = NULL;
        p->next = IconCache;
        IconCache->prev = p;
        IconCache = p;
      }

      if ( !q->icon )
        q->icon = (BitmapPtr)MemHandleLock(p->iconH);
      return p;
    }
  }

  WinDebugAlert("Miss!");

  // Cache hit not found, create new item.
  p = Cache_Grow(creator, type);
  if ( p ) {
    WinDebugAlert("Cache Grew");
    if ( !q->icon )
      q->icon = (BitmapPtr)MemHandleLock(p->iconH);
    return p;
  }

  return NULL;
}

#if 0

// Function:    Cache_LockBitmap
// Description: Get the pointer to a bitmap for the icon.
// Arguments:   A pointer to a cache entry.
// Returns:     A bitmap pointer.
BitmapPtr Cache_LockBitmap ( IconCacheType *ch ) {
  ErrFatalDisplayIf(!ch, "Nil cache pointer");
  ErrFatalDisplayIf(!ch->iconH, "Nil icon handle");

  // If the icon has already been locked, return gracefully.
  if ( ch->icon ) {
    ErrNonFatalDisplay ( "Icon already locked" );
    return ch->icon;
  }

  return ( ch->icon = (BitmapPtr)MemHandleLock(ch->iconH) );
}

#endif

// Function:    Cache_FreeBitmap
// Description: Free the pointer and unlock the handle for the icon bitmap.
// Arguments:   A pointer into the cache.
// Returns:     errNone if there was no error, the error code if otherwise.
Err Cache_FreeBitmap ( IconCacheType *ch ) {
  Err err;

  ErrFatalDisplayIf(!ch, "Nil cache poiter");
  ErrFatalDisplayIf(!ch->iconH, "Nil icon handle");
  ErrFatalDisplayIf(!ch->icon, "Nil icon ptr");

  WinDebugAlert("FreeBitmap, Top");

  err = MemHandleUnlock ( ch->iconH );
  ch->icon = NULL;

  return err;
}
