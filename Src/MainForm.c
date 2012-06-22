// $Id: MainForm.c,v 1.16 2002/10/01 15:05:58 amcgregor Exp $

// Author:     Alice Bevan-McGregor <alice@gothcandy.com>
// Creator:    David Kessler
// Descripton: The main form handler and support functions.

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

// $Log: MainForm.c,v $
// Revision 1.16  2002/10/01 15:05:58  amcgregor
// Fixed some minor bugs
//
// Revision 1.15  2002/09/24 04:38:19  amcgregor
// Updates to the plugin source, makefiles.
//
// Revision 1.14  2002/08/25 13:09:41  amcgregor
// Bugfixes (closing files,) "Home" button, and Version right column added.
//
// Revision 1.13  2002/07/31 19:10:36  amcgregor
// Requested features added.
//
// Revision 1.12  2002/07/25 15:09:00  amcgregor
// Second thought: tabs bad.
//
// Revision 1.11  2002/07/25 11:51:53  amcgregor
// Code run through: astyle --style=kr --indent=tab=2 -S *.c *.h
//
// Revision 1.10  2002/07/25 08:31:48  amcgregor
// Made typedown on name case-insensitive.
//
// Revision 1.9  2002/07/24 16:20:55  amcgregor
// Prerelease 3, major bugfixes.
//
// Revision 1.8  2002/07/21 19:08:36  amcgregor
// Bugfixes and added features.
//
// Revision 1.7  2002/07/19 18:58:40  amcgregor
// Enhanced Volume Information dialog
//
// Revision 1.6  2002/07/19 03:26:58  amcgregor
// Fixed first major bug, added release notes.
//
// Revision 1.5  2002/07/18 07:55:48  amcgregor
// Added release notes dialog, fixed crash w/ no VFS
//
// Revision 1.4  2002/07/16 00:59:43  amcgregor
// Initial public release, bugfixes, an aborted attempt at using the IconCache.
//
// Revision 1.3  2002/07/13 22:16:13  amcgregor
// Offline version to CVS miscelaneous updates.
//
// Revision 1.1  2002/07/11 14:46:55  root
// Initial revision
//


//#define DEBUG  1
//#define DEBUG1 1

// - ERROR HANDLING DEFINES --------------------------------------------------
#ifdef ERROR_CHECK_LEVEL
#undef ERROR_CHECK_LEVEL
#endif
#define ERROR_CHECK_LEVEL ERROR_CHECK_FULL


// - INCLUDES ----------------------------------------------------------------
#include <PalmOS.h>
#include <VFSMgr.h>
//#include <CallbackFix.h>
#include <DLServer.h>

#ifndef NOHIRES
#include <SonyCLIE.h>
#endif

#include "Resource.h"
#include "BuildNumber.h"
//#include "PlugIn.h"

#include "Globals.h"
#include "Utility.h"
#include "Progress.h"
#include "IconCache.h"
#include "TextForm.h"

// - GLOBAL VARIABLES --------------------------------------------------------
UInt16              mainFormID;

UInt16              volumeCount;         // number of volumes on this device (usually 1)
Char                volumeList[16][32];  // the names of these volumes
UInt32              volumeNums[16];      // the volume numbers for every volume
Char               *volList[16];         // temporary placeholder

Char               *DirectoryParents[2] = { RootDirectoryString,
    ParentDirectoryString };
DirectoryListItem  *Listing,             // the linked list DirectoryListItem
*ListEnd,             // the tail of the linked list DirectoryListItem
**SortedList;          // the sorted version of the list UInt32
UInt32              ListLength;          // the number of entries in the list Char
Char               *CurrentDirectory;    // the current path
UInt32              tTop;                // what item is at the top?
UInt32              tRows;               // how many rows are there?

Char                temp[80];
UInt32              lastTick;
Char               *errStrP_g = NULL;
MemHandle           nameList2Handle_g = 0;
Char              **nameList2_g;
Boolean             startInstall_g = false;
Char                typeDownBuf_g[40];
UInt16              typeDownIndex_g = 0;

Boolean             yesToAll = false;
Boolean             copyCancelled_g = false;
Boolean             readOnly_g = false;

UInt16              currentCheck = 0;
UInt16              numChecks = 0;

#define DEBUGINFO(m) showDebugInfo(__FILE__, __LINE__, m)
void showDebugInfo(const char *file, UInt32 lineNo, char *msg);

//static void doPluginList(FormPtr frmP, UInt16 tappedItem);
static void               dirUp(void);
static void               sortList ( CmpFuncPtr comparF );
static Int16              genericListSort(void *A, void *B, Int32 other );
static DirectoryListItem *growList ( void );
static void               showPage ( FormPtr frmP );
static void               deleteRAMDB ( Char *name );
static void               deleteCardDB ( Char *name );
static void               startViewCard ( FormPtr );
static void               startViewRAM ( FormPtr );

static void createDirectory(void) {
  FormType *newdirForm;
  UInt16 selection;
  Char *userInput;
  Char *path = NULL;
  Char dirName[64];
  Err err = errNone;
  DirectoryListItem *p;
  FileRef fref;

  if ( Preferences.Volume.Number == 0 ) {
    FrmAlert(NOCF_ALERT);
    return;
  }

  newdirForm = FrmInitForm(NEWDIR_FORM);
  FrmSetFocus(newdirForm, FrmGetObjectIndex(newdirForm, NEWDIR_NAME));

  selection = FrmDoDialog(newdirForm);
  if (selection == NEWDIR_OK_BTN) {
    // get the string
    userInput = FldGetTextPtr(getObjectPtr(newdirForm, NEWDIR_NAME));
    StrCopy(dirName, userInput);

    // validate the string
    err = 1; // just to make sure we don't create the item if the string
    // is invalid - that would suck, rats
    if (StrLen(dirName) > 0) {
      // create the directory
      path = MemPtrNew ( StrLen(CurrentDirectory) +
                         StrLen(dirName) + 4 );
      StrCopy ( path, CurrentDirectory );
      if ( StrCompare(CurrentDirectory,"/") )
        StrCat ( path, "/" );
      StrCat ( path, dirName );
      err = VFSDirCreate ( Preferences.Volume.Number, path );
    }

    if (err) {
      StrPrintF(temp, "Unable to create dir: %x", err);
      DEBUGINFO(temp);
    } else {
      FrmDeleteForm ( newdirForm );
      // Add the entry to the array by hand - much faster!
      p = growList();
      p->name = MemPtrNew ( StrLen (dirName) + 1 );
      StrCopy ( p->name, dirName );
      err = VFSFileOpen ( Preferences.Volume.Number, path, vfsModeRead, &fref );
      if ( err ) {
        DEBUGINFO("Unable to get information!");
      } else {
        VFSFileGetAttributes ( fref, &p->attributes );
        VFSFileGetDate ( fref, vfsFileDateCreated, &p->date.created );
        VFSFileGetDate ( fref, vfsFileDateModified, &p->date.modified );
        p->ID.creator = DirectoryCreatorID;
        p->ID.type = DirectoryTypeID;
        VFSFileClose ( fref );
      }

      sortList(genericListSort);
      tTop = 0;
      showPage(FrmGetActiveForm());
      return;
    }
  }

  FrmDeleteForm(newdirForm);
}


void deleteItemList(void) {
  DirectoryListItem *p = Listing,
                         *q = p;

  for ( p = Listing; p; q=p, p=p->next ) {
    if ( MemPtrFree ( q->name ) )
      DEBUGINFO("Error freeing filename.");
    q->next = NULL;
    MemSet ( q, MemPtrSize ( q ), 0 );
    if ( MemPtrFree ( q ) )
      DEBUGINFO("Error freeing pointer.");
  }

  Listing = NULL;
  ListLength = 0;
}

static void sortList ( CmpFuncPtr comparF ) {
  WinHandle progWin;
  WinHandle oldWin;
  WinHandle saveWin;
  DirectoryListItem *p, **q;

  drawPopupWindow ( &progWin, &oldWin, &saveWin, "Sorting list ..." );

  if ( SortedList ) {
    MemSet ( SortedList, MemPtrSize ( SortedList ), 0 );
    if ( MemPtrFree ( SortedList ) )
      DEBUGINFO("Unable to free sorted list.");
  }

  SortedList = MemPtrNew ( sizeof ( DirectoryListItem * ) * ( ListLength + 2 ) );
  if ( !SortedList )
    DEBUGINFO("Unable to allocate sorted list.");
  MemSet ( SortedList, MemPtrSize(SortedList), 0 );

  // Start of main "copy" loop.  Fill the sorted array
  // so it can be sorted.

  for ( q=SortedList, p=Listing; p; p=p->next ) {
    *q = p; // Copy the pointer into the sorted list.
    q++;    // Go to the next entry in the list.
  }

  // Now on with the actual sorting!
  // Tip: This code is MUCH faster then the original.  Reason?  I'm not
  // moving large sections of memory around, just pointers!
  if ( Preferences.Display.Sort != sortNone )
    if ( comparF )
      SysQSort ( SortedList,
                 ListLength + 1,
                 sizeof(DirectoryListItem **),
                 comparF,
                 ( Preferences.Display.DirectoriesFirst && ( Preferences.Volume.Number > 0 ) ? true : false ) );

  deletePopupWindow ( progWin, oldWin, saveWin );
}

static Int16 genericListSort(void *A, void *B, Int32 dirfirst) {
  DirectoryListItem *a = *(DirectoryListItem **)A,
                         *b = *(DirectoryListItem **)B;
  Int16 result = 0;

  switch ( Preferences.Display.Sort ) {
  case sortByName:
    result = StrCaselessCompare ( a->name, b->name );
    break;

  case sortByRightColumn:
    switch (Preferences.Display.Column) {
    case colVersion:
      if ( a->version > b->version ) {
        result = -1;
      } else if ( a->version < b->version ) {
        result = 1;
      } else {
        result = 0;
      }
      break;
      
    case colDateBackup:
      if (a->date.backedUp > b->date.backedUp) {
        result = -1;
      } else if (a->date.backedUp < b->date.backedUp) {
        result = 1;
      } else {
        result = 0;
      }
      break;

    case colDateModified:
      if (a->date.modified > b->date.modified) {
        result = -1;
      } else if (a->date.modified < b->date.modified) {
        result = 1;
      } else {
        result = 0;
      }
      break;

    case colDateCreated:
      if (a->date.created > b->date.created) {
        result = -1;
      } else if (a->date.created < b->date.created) {
        result = 1;
      } else {
        result = 0;
      }
      break;

    case colRecords:
      if (a->records > b->records) {
        result = -1;
      } else if (a->records < b->records) {
        result = 1;
      } else {
        result = 0;
      }
      break;

    case colCreator:
      if (a->ID.creator > b->ID.creator) {
        result = -1;
      } else if (a->ID.creator < b->ID.creator) {
        result = 1;
      } else {
        result = 0;
      }
      break;

    case colType:
      if (a->ID.type > b->ID.type) {
        result = -1;
      } else if (a->ID.type < b->ID.type) {
        result = 1;
      } else {
        result = 0;
      }
      break;

    case colSize:
      if (a->size < b->size) {
        result = -1;
      } else if (a->size > b->size) {
        result = 1;
      } else {
        result = 0;
      }
      break;
    }
    break;

  default:
    DEBUGINFO("Invalid Display.Sort!");
    break;
  }

  // Reverse sort -before- folders.  Folders on Top means folders on top.
  if ( Preferences.Display.ReverseSort )
    result *= -1;

  if ( ( dirfirst ) ) { // && ( Preferences.Volume.Number > 0 ) ) {
    if (  ( a->attributes & vfsFileAttrDirectory ) &&
          !( b->attributes & vfsFileAttrDirectory ) ) {
      result = -1;
    } else if ( !( a->attributes & vfsFileAttrDirectory ) &&
                ( b->attributes & vfsFileAttrDirectory ) ) {
      result = 1;
    }
  }

  return result;
}

static DirectoryListItem *growList ( void ) {
  DirectoryListItem *p;
  p = (DirectoryListItem *)MemPtrNew ( sizeof(DirectoryListItem) );
  if ( !p )
    DEBUGINFO("Dynamic heap exhausted!");
  MemSet ( p, sizeof(DirectoryListItem), 0 );
  p->ID.creator = 'DEAD';
  p->ID.type    = 'babe';

  if ( !Listing ) {
    Listing = p;
    ListEnd = Listing;
    ListLength = 0;
  } else {
    ListEnd->next = p;
    ListEnd = p;
    ListLength++;
  }

  return p;
}


static void updateDirField(FormPtr frmP) {
  FldSetTextPtr(getObjectPtr(frmP, DIRECTORY_FLD), CurrentDirectory);
  FldDrawField(getObjectPtr(frmP, DIRECTORY_FLD));
}

static void drawIconCell ( void *tableP, short row, short column, RectanglePtr bounds ) {
  UInt8              x, y, width;
  UInt16             i;
  RectangleType      r;
  DmOpenRef          dbOR;
  MemHandle          iconH;
  BitmapType        *iconB;
  Boolean            iconDrawn = false;
  DirectoryListItem *p;

  if ( !(row <= ListLength - tTop) )
    return;

  x = bounds->topLeft.x;
  y = bounds->topLeft.y + 1;
  width = bounds->extent.x;

  r.topLeft.y = bounds->topLeft.y;
  r.extent.y = bounds->extent.y;

  i = row + tTop;
  p = SortedList[i];

  if ( Preferences.Display.ShowIcons ) {
    if ( ( Preferences.Volume.Number == 0 ) || ( isPalmDB ( p->name ) ) ) {
      // Search for and open the database that relates to the current file.
      dbOR = DmOpenDatabaseByTypeCreator ( 'appl', p->ID.creator, dmModeReadOnly );
      if ( dbOR ) {                    // Did we actually find an associated program?
        // Find the "small icon" resource.
        iconH = DmGetResourceIndex ( dbOR, DmFindResource ( dbOR, 'tAIB', 1001, NULL ) );
        if ( iconH ) {                 // Did we actctually find it?
          iconB = (BitmapType *)MemHandleLock(iconH); // Lock the memory down, get a Bitmap to draw.
          WinDrawBitmap ( iconB, x, y );              // Draw the bitmap to the screen.
          MemHandleUnlock ( iconH );                  // Unlock the memory so it can be freed.
          DmReleaseResource ( iconH );                // Free the memory, and unlock the resource.
          iconDrawn = true;
        }
        DmCloseDatabase ( dbOR );
      }
    } else if ( p->attributes & vfsFileAttrDirectory ) {
      WinDrawBitmap ( FolderIconB, x, y);
      iconDrawn = true;
    }

    if ( !iconDrawn ) {
      if ( !StrCaselessCompare ( Extension ( p->name ), "txt" ) ||
           !StrCaselessCompare ( Extension ( p->name ), "doc" ) ||
           ( ( p->ID.creator == 'REAd' ) && ( p->ID.type == 'TEXt' ) ) ) {
        WinDrawBitmap ( TextFileIconB, x, y );
      } else
        WinDrawBitmap ( FileIconB, x, y );
    }
  }
}


#if 0
#warning IconCache is broken!
// The IconCache is broken... need some help getting it to work.
static void drawIconCell ( void *tableP, short row, short column, RectanglePtr bounds ) {
  UInt8              x, y, width;
  UInt16             i;
  RectangleType      r;
  DmOpenRef          dbOR;
  IconCacheType     *icon;
  DirectoryListItem *p;

  if ( !(row <= ListLength - tTop) )
    return;

  x = bounds->topLeft.x;
  y = bounds->topLeft.y + 1;
  width = bounds->extent.x;

  r.topLeft.y = bounds->topLeft.y;
  r.extent.y = bounds->extent.y;

  i = row + tTop;
  p = SortedList[i];

  if ( Preferences.Display.ShowIcons ) {
    icon = Cache_Hit ( p->ID.creator, p->ID.type );
    if ( icon && icon->icon ) {
      DEBUGINFO("icon found, drawing");
      WinDrawBitmap ( icon->icon, x, y);
      Cache_FreeBitmap ( icon );
    }
  }
}
#endif

static void drawNameCell ( void *tableP, short row, short column, RectanglePtr bounds ) {
  UInt8              x, y, width;
  UInt16             titleLen, i;
  RectangleType      r;
  DirectoryListItem *p;
  UInt16             oldfont;

  if ( !(row <= ListLength - tTop) )
    return;

  x = bounds->topLeft.x + 1;
  y = bounds->topLeft.y;
  width = bounds->extent.x;

  r.topLeft.y = bounds->topLeft.y;
  r.extent.y = bounds->extent.y;

  oldfont = FntSetFont ( stdFont );

  i = row + tTop;
  p = SortedList[i];

  titleLen = StrLen ( p->name );

  // Make the text for directories bold, if the option is set.
  if ( ( p->attributes == vfsFileAttrDirectory ) &&
       ( Preferences.Volume.Number != 0 ) &&
       Preferences.Display.BoldDirectories )
    FntSetFont ( boldFont );
  if ( !Preferences.Display.ShowIcons ) {
    x -= 16;
    width += 16;
  }
  WinDrawTruncChars ( p->name, titleLen, x, y, width);

}

static void drawAttribCell ( void *tableP, short row, short column, RectanglePtr bounds ) {
  UInt8              x, y, width;
  UInt16             textWidth, i;
  RectangleType      r;
  DateTimeType       tempDate;
  DirectoryListItem *p;
  Char               sizetext[16];

  if ( !(row <= ListLength - tTop) )
    return;

  x = bounds->topLeft.x + 1;
  y = bounds->topLeft.y;
  width = bounds->extent.x;

  r.topLeft.y = bounds->topLeft.y;
  r.extent.y = bounds->extent.y;

  FntSetFont ( stdFont );

  i = row + tTop;
  p = SortedList[i];

  // Directories generally do not have attributes ;)
  if ( ( p->attributes == vfsFileAttrDirectory ) &&
       ( Preferences.Volume.Number != 0 ) &&
       ( Preferences.Display.Column != colAttributes ) ) {
    if ( Preferences.Display.BoldDirectories )
      FntSetFont ( boldFont );
    textWidth = FntCharsWidth ("<dir>", 5);
    WinDrawChars ( "<dir>", 5, x+width-textWidth-2, y);

  } else {
    switch ( Preferences.Display.Column ) {
      case colSize:       // File Size (# of files in a dir?)
        StrCopy ( sizetext, printNiceSize ( p->size, sizetext, 1 ) );
        break;

      case colCreator:    // Creator ID
        if ( Preferences.Volume.Number == 0 ) {
          FourCharInt ( p->ID.creator, sizetext );
        } else {
          if ( !isPalmDB(p->name) )
            StrCopy ( sizetext, "N/A" );
          else
            FourCharInt ( p->ID.creator, sizetext );
        }
        break;

      case colType:      // Type ID
        if ( Preferences.Volume.Number == 0 )
          FourCharInt ( p->ID.type, sizetext );
        else if ( !isPalmDB(p->name) )
          StrCopy ( sizetext, "N/A" );
        else
          FourCharInt ( SortedList[row+tTop]->ID.type, sizetext );
        break;

      case colRecords: // # Records
        if ( Preferences.Volume.Number == 0 )
          StrPrintF ( sizetext, "%ld", p->records );
        else if ( !isPalmDB(p->name) )
          StrCopy ( sizetext, "N/A" );
        else
          StrPrintF ( sizetext, "%d", p->records );
        break;

      case colDateCreated: // Date Created
        TimSecondsToDateTime ( p->date.created, &tempDate );
        StrPrintF ( sizetext, "%d/%d", tempDate.day, tempDate.month ); //, (tempDate.year>2000?tempDate.year-2000:tempDate.year-1900));
        break;

      case colDateModified: // Date Last Modified
        TimSecondsToDateTime ( p->date.modified, &tempDate );
        StrPrintF ( sizetext, "%d/%d", tempDate.day, tempDate.month ); //, (tempDate.year>2000?tempDate.year-2000:tempDate.year-1900));
        break;

      case colDateBackup: // Date Last Backed Up
        TimSecondsToDateTime ( p->date.backedUp, &tempDate );
        StrPrintF ( sizetext, "%d/%d", tempDate.day, tempDate.month ); //, (tempDate.year>2000?tempDate.year-2000:tempDate.year-1900));
        break;

      case colVersion: // version number
        if ( ( Preferences.Volume.Number == 0 ) || ( isPalmDB(p->name) ) )
          StrPrintF ( sizetext, "%d", p->version );
        break;
  
      case colAttributes: // special attributes
        if ( Preferences.Volume.Number == 0 ) {
          StrCopy ( sizetext, "" );
          if ( p->attributes & dmHdrAttrBackup )
            StrCat(sizetext,"b");
          if ( p->attributes & dmHdrAttrHidden )
            StrCat(sizetext,"h");
          if ( p->attributes & dmHdrAttrCopyPrevention )
            StrCat(sizetext,"p");
          if ( p->attributes & dmHdrAttrLaunchableData )
            StrCat(sizetext,"l");
          if ( p->attributes & dmHdrAttrOpen )
            StrCat(sizetext,"o");
          if ( p->attributes & dmHdrAttrReadOnly )
            StrCat(sizetext,"r");
          if ( p->attributes & dmHdrAttrStream )
            StrCat(sizetext,"s");
          if ( p->attributes & dmHdrAttrAppInfoDirty )
            StrCat(sizetext,"d"); // new
          if ( p->attributes & dmHdrAttrBundle )
            StrCat(sizetext,"u"); // new
          if ( p->attributes & dmHdrAttrRecyclable )
          StrCat(sizetext,"e"); // new
          if ( p->attributes & dmHdrAttrResDB )
            StrCat(sizetext,"R"); // new
          if ( p->attributes & dmHdrAttrResetAfterInstall )
            StrCat(sizetext,"i"); // new
        } else {
          StrCopy(sizetext,"");
          if ( p->attributes & vfsFileAttrDirectory )
            StrCat(sizetext,"d");
          if ( p->attributes & vfsFileAttrReadOnly )
            StrCat(sizetext,"r");
          if ( p->attributes & vfsFileAttrHidden )
            StrCat(sizetext,"h");
          if ( p->attributes & vfsFileAttrSystem )
            StrCat(sizetext,"s");
          if ( p->attributes & vfsFileAttrArchive )
            StrCat(sizetext,"a");
          if ( p->attributes & vfsFileAttrLink )
            StrCat(sizetext,"l");
        }
        break;
      default: // future expantion?
        StrCopy ( sizetext, printNiceSize ( p->size, sizetext, 1 ) );
        break;
    }
    textWidth = FntCharsWidth ( sizetext, StrLen ( sizetext ) );
    WinDrawChars ( sizetext, StrLen ( sizetext ), x + width - textWidth - 2, y );
  } // else
}

static void showPage ( FormPtr formP ) {
  TablePtr      tableP;
  ScrollBarPtr  scrollP;
  UInt16        i;

  tableP = (TablePtr)FrmGetObjectPtr ( formP, FrmGetObjectIndex ( formP, MAIN_TABLE ) );
  scrollP = (ScrollBarPtr)FrmGetObjectPtr ( formP, FrmGetObjectIndex ( formP,  MAIN_SCROLLBAR) );

  if ( !( ListLength < tRows ) ) {
    SclSetScrollBar ( scrollP, tTop, 0, ListLength-tRows+1, tRows );
  } else {
    SclSetScrollBar ( scrollP, 0, 0, 0, tRows );
  }
  SclDrawScrollBar ( scrollP );

  for (i = 0; i < tRows; i++) {
    if ( i <= ListLength-tTop ) {
      TblSetRowUsable ( tableP, i, true );
      if ( !StrCompare(SortedList[i+tTop]->name, DirectoryParents[0]) ||
           !StrCompare(SortedList[i+tTop]->name, DirectoryParents[1]) ) {
        TblSetItemStyle ( tableP, i, 0, customTableItem );
      } else {
        TblSetItemStyle ( tableP, i, 0, checkboxTableItem );
        TblSetItemInt ( tableP, i, 0, SortedList[i+tTop]->selected );
      }
    } else {
      TblSetRowUsable ( tableP, i, false );
    }
    TblMarkRowInvalid ( tableP, i ); // mark invalid so it redraws
  }
  
  if ( PalmOSVersion >= PalmOS35 ) WinScreenLock ( winLockCopy );
  TblRedrawTable ( tableP );
  if ( PalmOSVersion >= PalmOS35 ) WinScreenUnlock ( );
}

static void startViewRAM ( FormPtr frmP ) {
  WinHandle  progWin;
  WinHandle  oldWin;
  WinHandle  saveWin;
  UInt16     oldAutoOffTime;
  Boolean    ns;
  DirectoryListItem   *p;
  UInt16     card, attributes;
  LocalID    id;
  DmSearchStateType s;
  Char       name[96];
  UInt32     numDB, currentDB = 0;

  oldAutoOffTime = SysSetAutoOffTime(0);
  drawProgressWindow(&progWin, &oldWin, &saveWin, "Scanning databases...");
  numDB = DmNumDatabases(0);

  deleteItemList();

  ns = true;
  while ( DmGetNextDatabaseByTypeCreator ( ns, &s, NULL, NULL, false, &card, &id ) == 0 ) {
    ns = false;
    currentDB++;
    if ( !isROMDB(card, id) || Preferences.Display.ShowROM ) {
      p = growList();
      DmDatabaseInfo ( card, id, name, &attributes, &p->version,
                       &p->date.created, &p->date.modified, &p->date.backedUp,
                       &p->modnum, NULL, NULL, &p->ID.type, &p->ID.creator );
      DmDatabaseSize ( card, id, &p->records, &p->size, NULL );
      p->name = MemPtrNew ( StrLen(name)+1 );
      if ( !p )
        DEBUGINFO("Unable to allocate memory for filename.");
      StrCopy ( p->name, name );
      p->attributes = attributes;

      //StrPrintF( tempStr, "%ld%% (%s/%s) - %sB/s",
      //           (currentDB*100)/numDB, currentDB, numDB, name );
      drawProgressBar ( (currentDB*100)/numDB );
    }
  }

  deletePopupWindow(progWin, oldWin, saveWin);

  sortList(genericListSort);

  if ( frmP )
    showPage(frmP);
  else
    showPage(FrmGetActiveForm());

  SysSetAutoOffTime(oldAutoOffTime);
}


static void startViewCard ( FormPtr frmP ) {
  WinHandle  progWin;
  WinHandle  oldWin;
  WinHandle  saveWin;
  UInt32     j = 0;
  DirectoryListItem  *p;
  UInt32     it;
  FileRef    dref, fref;
  FileInfoType finfo;
  Char       fname[512];
  Err        err;
  UInt16     oldAutoOffTime;
  Char       *path;

  oldAutoOffTime = SysSetAutoOffTime(0);

  drawPopupWindow(&progWin, &oldWin, &saveWin, "Scanning files...");

  deleteItemList();

  finfo.nameBufLen = 512;
  finfo.nameP = fname;

  // Open the directory to get contents.
  err = VFSFileOpen ( Preferences.Volume.Number, CurrentDirectory, vfsModeRead, &dref );
  if ( err ) {
    DEBUGINFO("Unable to open CurrentDirectory! Trying root directory...");
    StrCopy(CurrentDirectory, "/");
    err = VFSFileOpen ( Preferences.Volume.Number, CurrentDirectory, vfsModeRead, &dref );
    if ( err ) {
      DEBUGINFO("Unable to open root directory! Hilarity ensues...");
      return;
    }
  }

  // Add directory surfing items if not in the root directory.
  if ( StrCompare ( CurrentDirectory, "/" ) ) {
    for ( j = 0; j < 2; j++ ) {
      p = growList();
      p->name = MemPtrNew ( StrLen(DirectoryParents[j]) + 1 );
      StrCopy ( p->name, DirectoryParents[j] );
      p->attributes = vfsFileAttrDirectory;
      p->size = 0;
      p->ID.creator = DIRECTORY_CREATOR;
      p->ID.type = DIRECTORY_TYPE;
    }
  }

  it = vfsIteratorStart;
  while (it != vfsIteratorStop) {
    if ( VFSDirEntryEnumerate ( dref, &it, &finfo ) )
      break;

    path = (Char *)MemPtrNew ( StrLen(CurrentDirectory) +
                               StrLen(fname) + 4 );
    StrCopy ( path, CurrentDirectory );
    if ( StrCompare(CurrentDirectory,"/") )
      StrCat(path,"/");
    StrCat(path, fname);

    err = VFSFileOpen ( Preferences.Volume.Number, path, vfsModeRead, &fref );
    if ( err ) {
      DEBUGINFO("Unable to open file to get information!");
      DEBUGINFO(path);

    } else {

      if ( !isPalmDB(fname) &&
           !(finfo.attributes & vfsFileAttrDirectory ) &&
           Preferences.Display.PalmOnly ) {
        // Need to close the file, THEN continue.
        VFSFileClose ( fref );
        continue;
      }

      p = growList();
      p->name = MemPtrNew ( StrLen (fname) + 1 );
      StrCopy ( p->name, fname );

      p->attributes = finfo.attributes;
      if ( !(p->attributes & vfsFileAttrDirectory) )
        VFSFileSize ( fref, &p->size );
      //VFSFileGetDate ( fref, vfsFileDateCreated, &p->date.created );
      //VFSFileGetDate ( fref, vfsFileDateModified, &p->date.modified );

      if ( (isPalmDB ( p->name )) && !(p->attributes & vfsFileAttrDirectory) ) {
        VFSFileDBInfo ( fref, NULL,
                        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                        &p->ID.type,
                        &p->ID.creator,
                        (UInt16 *)&p->records );
      } else {
        if ( p->attributes & vfsFileAttrDirectory ) {
          p->ID.creator = DIRECTORY_CREATOR;
          p->ID.type = DIRECTORY_TYPE;
        } else {
          p->ID.creator = FILE_CREATOR;
          p->ID.type = FILE_TYPE;
        }
      }
      MemPtrFree ( path );
      VFSFileClose ( fref );
    }

    j++;
  }

  VFSFileClose ( dref );

  deletePopupWindow(progWin, oldWin, saveWin);

  sortList(genericListSort);

  showPage ( frmP );

  SysSetAutoOffTime(oldAutoOffTime);
}


static Err dirDown(char *path, FormPtr formP) {
  char *newpath;

  if ( path[0] == '/' ) { // absoulute path given
    if ( isDirectory(Preferences.Volume.Number, path) ) {
      if ( CurrentDirectory )
        MemPtrFree ( CurrentDirectory );
      CurrentDirectory = (Char *)MemPtrNew ( StrLen(path)+1 );
      StrCopy ( CurrentDirectory, path );
      startViewCard(formP);
    } else { // there was an error (not a directory?)
      // TODO: display error, or at least a warning
      return 1;
    }
  } else { // not an absolute path, relative?
    newpath = (Char *)MemPtrNew ( StrLen(path)+StrLen(CurrentDirectory)+2 );
    StrCopy ( newpath, CurrentDirectory );
    if ( StrCompare ( newpath, "/" ) )
      StrCat ( newpath, "/" );
    StrCat ( newpath, path );
    if ( isDirectory(Preferences.Volume.Number, newpath) ) {
      if ( CurrentDirectory )
        MemPtrFree ( CurrentDirectory );
      CurrentDirectory = (Char *)MemPtrNew ( StrLen(newpath)+1 );
      StrCopy ( CurrentDirectory, newpath );
      startViewCard(formP);
    } else {
      // TODO: display error, or at least a warning
      MemPtrFree(newpath);
      return 1;
    }
    MemPtrFree(newpath);
  }

  tTop = 0;
  StrCopy(Preferences.Volume.Path, CurrentDirectory);
  PrefSetAppPreferences(AppCreator, 1000, 1, &Preferences,
                        sizeof(TPreferences), false);

  updateDirField(formP);

  return 0;
}


static void dirUp(void) {
  int i;

  // Can't go "up" from root!
  if ( !StrCompare(CurrentDirectory, "/") ) {
    return;
  }

  // Strip the last directory name from cwd
  for (i = StrLen(CurrentDirectory); i > 0; i--) {
    if ( CurrentDirectory[i] == '/') {
      CurrentDirectory[i] = '\0';
      i = -1;
    }
  }

  if (i == 0)
    StrCopy (CurrentDirectory, "/");

  StrCopy(Preferences.Volume.Path, CurrentDirectory);
  PrefSetAppPreferences(AppCreator, 1000, 1, &Preferences,
                        sizeof(TPreferences), false);

  updateDirField(FrmGetActiveForm());

  // TODO: This should be done after deleting the current directory,
  //       not every time the function is called (this would double-
  //       call when the user taps the "Parent Directory" entry...)
  //if ( Preferences.Volume.Number > 0 ) startViewCard(FrmGetActiveForm());
  //else startViewRAM(FrmGetActiveForm());

  return;
}

static void dirRoot(void) {
  if ( CurrentDirectory )
    MemPtrFree ( CurrentDirectory );
  CurrentDirectory = (Char *)MemPtrNew(2);
  StrCopy ( CurrentDirectory, "/" );

  updateDirField(FrmGetActiveForm());
}

static void doSelectAll ( void ) {
  UInt16 i;

  numChecks = 0;

  for (i = 0; i < ListLength+1; i++) {
    if ( (Preferences.Volume.Number == 0) ||
         !(SortedList[i]->attributes & vfsFileAttrDirectory)) {
      SortedList[i]->selected = true;
      numChecks++;
    }
  }
}

static void doSelectNone ( void ) {
  UInt16 i;

  for (i = 0; i < ListLength+1; i++) {
    SortedList[i]->selected = false;
  }

  numChecks = 0;
}

static void doSelectInvert ( void ) {
  UInt16 i;

  for (i = 0; i < ListLength+1; i++) {
    if ( ( Preferences.Volume.Number == 0 ) ||
         !( SortedList[i]->attributes & vfsFileAttrDirectory ) )
      SortedList[i]->selected = !SortedList[i]->selected;
  }
}

#if 0
static void doFormatCard ( void ) {
  VFSSlotMountParamType slotParam;
  UInt32 slotIterator = expIteratorStart;

  slotParam.vfsMountParamP.mountClass = VFSMountClass_SlotDriver;
  err = ExpSlotEnumerate ( &slotParam.slotRefNum, &slotIterator );
  err = ExpSlotLibFind ( slotParam.slotRefNum, &slotParam.slotLibRefNum );

  err = VFSVolumeFormat ( NULL, NULL, (VFSAnyMountParamPtr)&slotParam );
}
#endif

typedef struct {
  UInt32 current;
  UInt32 total;
  Boolean first;
}
CallbackStruct;

Err ProgressCallback ( UInt32 total, UInt32 current, void *data ) {
  CallbackStruct *d = (CallbackStruct *)data;
  UInt32          tempInt = StopTimer(),
                            tempInt2 = (current*100)/total,
                                       tempInt3 = ((d->current+current)*100)/d->total;
  Char            sizeStr[3][16];
  Char            tempStr[64];

  // Draw on first call, or if 20ms have passed since the last call and the size is > 20k.
  // Copies go -much- faster if the delay is longer, or the minimum size is greater.
  if ( (d->first) || ( ( total > 25*1024 ) && ( tempInt-lastTick > 20 ) ) ) {
    d->first = false;
    lastTick=tempInt;
    StrPrintF( tempStr, "%ld%% (%s/%s) - %sB/s",
               tempInt2,
               printNiceSize(current, sizeStr[0], 1),
               printNiceSize(total, sizeStr[1], 1),
               printNiceSize((current+1)/((tempInt/100)+1), sizeStr[2], 2) );
    // StrPrintF ( tempStr, "%ld%% (%d/%d)", tempInt3, d->current, d->total );
    WinDrawProgressBar ( tempInt2, tempInt3, tempStr );
  }

  return errNone;
}


static Err copyDBToCard ( UInt16 volume, DirectoryListItem *file, Char *modeStr, UInt32 item, UInt32 total ) {
  Err             err;
  Char            message[128];
  Char           *path;
  CallbackStruct  cbs;
  UInt8           selection;
  WinHandle       _winOld;
  Char           *p, *q;

  StrPrintF ( message, "%s%s to card...", modeStr, file->name );
  WinDrawProgress ( message );

  path = (Char *)MemPtrNew ( StrLen(CurrentDirectory) + StrLen(file->name) + 6 );
  StrCopy ( path, CurrentDirectory );
  if ( StrCompare ( CurrentDirectory, "/" ) )
    StrCat ( path, "/" );
  StrCat ( path, file->name );
  StrCat ( path, ( file->attributes & dmHdrAttrResDB ? ".prc" : ".pdb" ) );
  p = path + StrLen(path) - StrLen(file->name);
  q = p + StrLen(path);
  while ( p < q ) {
    if ( *p == '/' ) *p = '_';
    if ( *p < ' ' ) *p = '_';
    if ( *p > 165 ) *p = '_';
  }
  
  cbs.current = item;
  cbs.total = total;
  cbs.first = true;
  StartTimer();
  err = VFSExportDatabaseToFileCustom ( volume, path, 0,
                                        DmFindDatabase(0, file->name),
                                        &ProgressCallback, &cbs );
  //WinDeleteProgress();

  if ( err == vfsErrFileAlreadyExists ) {
    _winOld = WinGetDrawWindow();
    if ( !yesToAll )
      selection = FrmCustomAlert ( FILE_DELETE_ALERT, file->name, "", "" );
    else
      selection = 0;
    WinSetDrawWindow ( _winOld );
    if ( (selection == 0) || (selection==2) ) {
      if ( selection == 2 )
        yesToAll = true;
      VFSFileDelete ( volume, path );
      err = VFSExportDatabaseToFileCustom ( volume, path, 0,
                                            DmFindDatabase(0, file->name),
                                            &ProgressCallback, &cbs );
    } else {
      err = 0;
    }
  }

  MemPtrFree(path);
  return err;
}

static Err copyCardToDB ( UInt16 volume, DirectoryListItem *file, Char *modeStr, UInt32 item, UInt32 total ) {
  Err err;
  Char message[128];
  Char *path;
  CallbackStruct cbs;
  //WinHandle       _winOld;

  StrPrintF ( message, "%s%s to card...", modeStr, file->name );
  WinDrawProgress ( message );

  path = (Char *)MemPtrNew ( StrLen(CurrentDirectory) + StrLen(file->name) + 6 );
  StrCopy ( path, CurrentDirectory );
  if ( StrCompare ( CurrentDirectory, "/" ) )
    StrCat ( path, "/" );
  StrCat ( path, file->name );
  cbs.current = item;
  cbs.total = total;
  cbs.first = true;
  StartTimer();
  err = VFSImportDatabaseFromFileCustom ( volume, path, NULL, NULL, &ProgressCallback, &cbs );
  MemPtrFree(path);

  return err;
}

static void doCopyMove ( FormPtr formP, Boolean del  ) {
  UInt16 i;
  Err err = 0;
  UInt32 totalBytes;
  UInt32 bytesFree;
  UInt32 usedBytes;
  UInt32 currentbyte=0, totalbytes=0; // used for overall progress...
  //WinHandle       _winOld;

  numChecks = 0;
  currentCheck = 0;
  for ( i = 0; i < ListLength+1; i++ )
    if ( SortedList[i]->selected ) {
      numChecks++;
      totalbytes += SortedList[i]->size;
    }

  // loop through all the databases looking for those that are selected
  yesToAll = false;
  for ( i = 0; i < ListLength+1; i++ ) {
    if ( SortedList[i]->selected ) {
      // copy this one
      if ( checkBattery() ) {
        if ( Preferences.Volume.Number == 0 ) {
          // check for space on the CF card
          VFSVolumeSize ( volumeNums[1], &usedBytes, &totalBytes );
          bytesFree = totalBytes - usedBytes;

          if ( SortedList[i]->size > (bytesFree - 16384) ) {
            // no space
            FrmAlert(ALERT_FULL_CARD);
            err = 1;
          } else {
            // extract to the CF card
            err = copyDBToCard ( volumeNums[1],
                                 SortedList[i],
                                 ( del ? "Moving " : "Copying " ),
                                 currentbyte, totalbytes );
            // currentCheck, numChecks ); - display num files complete, instead of total bytes
            currentbyte += SortedList[i]->size;
            if ( del )
              deleteRAMDB ( SortedList[i]->name );
          }
        } else {
          err = copyCardToDB ( volumeNums[1],
                               SortedList[i],
                               ( del ? "Moving " : "Copying " ),
                               currentbyte, totalbytes );
          // currentCheck, numChecks );
          currentbyte += SortedList[i]->size;
          if ( del )
            deleteCardDB ( SortedList[i]->name );
        }

        currentCheck++;
      } else {
        err = -1;
      }
    }

    if (err)
      break;
  }

  WinDeleteProgress();

  if ( del ) {
    if ( Preferences.Volume.Number > 0 ) {
      startViewCard(formP);
    } else {
      startViewRAM(formP);
    }
  }

  //  FrmAlert(DONE_ALERT);
}

static void deleteRAMDB ( Char *name ) {
  Err err;
  WinHandle       _winOld;

  // Delete the database, by name... FIXME: Can there be multiple files with
  // the same name?  Probably not, but need to check.
  if ( ( err = DmDeleteDatabase ( 0, DmFindDatabase ( 0, name ) ) ) ) {
    // deletion failed
    _winOld = WinGetDrawWindow();
    StrPrintF(temp, "DB delete (%s) failed: %x", name, err);
    DEBUGINFO(temp);
    WinSetDrawWindow ( _winOld );
  }
}


static void deleteCardDB ( Char *name ) {
  Err err;
  Char *path;
  WinHandle       _winOld;

  // Construct the full file path...
  path = (Char *)MemPtrNew ( StrLen(CurrentDirectory) + StrLen(name) + 4 );
  StrCopy ( path, CurrentDirectory );
  if ( StrCompare(path, "/") )
    StrCat ( path, "/" );
  StrCat ( path, name );

  // Delete the file...
  if ( ( err = VFSFileDelete(Preferences.Volume.Number, path) ) ) {
    _winOld = WinGetDrawWindow();
    StrPrintF(temp, "VFS delete (%s+%s=%s) failed: %x", CurrentDirectory, name, path, err);
    DEBUGINFO(temp);
    WinSetDrawWindow ( _winOld );
  }
}


static void doDelete(FormPtr frm) {
  UInt16 i, numSelected = 0, j = 0;
  UInt32 perc;
  UInt16 choice;
  Char   message[64];
  //DirectoryListItem *p, *q;
  WinHandle       _winOld;

  if ( (Preferences.Volume.Number > 0) && readOnly_g ) {
    FrmAlert(READONLY_ALERT);
    return;
  }

  // loop through all the databases looking for those that are selected
  yesToAll = false;

  choice = FrmAlert(DELETE_ALERT);
  if (choice != 0)
    return;
  if (!checkBattery())
    return;

  WinDrawProgress ( "Deleting..." );

  for ( i = 0; i < ListLength+1; i++ )
    if ( SortedList[i]->selected )
      numSelected++;

  for ( i=0, j=0; i < ListLength+1; i++ ) {
    if ( SortedList[i]->selected ) {
      perc = ( (UInt32)(j) * 100 ) / numSelected;
      StrPrintF ( message, "%ld%% (%d/%d) - %s",
                  perc, j, numSelected, SortedList[i]->name );
      WinDrawProgressBar ( 0, perc, message );
      if ( Preferences.Volume.Number == 0 ) {
        deleteRAMDB( SortedList[i]->name );
      } else {
        deleteCardDB ( SortedList[i]->name );
      }
      j++;
    }
  }

  WinDeleteProgress();
  // FrmAlert(DONE_ALERT); I find this -really- annoying... need to make it
  // optional for the user.  Default on, but easily turned off.  Idea:
  // turn it into a function that checks for a second button - never remind
  // me again, and sets the option in the preferences and only displays the
  // message if the option is turned off...

  // Now, instead of refreshing the entire blinking list, why not
  // just remove the entries we tried to delete, eh?  Need to
  // handle undeletable files though... uncheck them? ^_^

  if ( Preferences.Volume.Number > 0 )
    startViewCard(frm);
  else
    startViewRAM(frm);

  // Real quick pointer-walking loop.  Linked lists are great. ;)
  // If we run through the array (p) we can not unlink p, as there
  // is no way to set it's parent's next to skip p, we need q to
  // check the -next- item, thus giving us warning.
  //for ( p=Listing; p; p=p->next ) {
  //  q = p->next;
  //  if ( p->selected ) {
  //    // If this item is not the head, there is nothing we can do to remove it
  //    // from the list... sad, but true.  This is not a doubly-linked list.
  //    if ( Listing == p ) Listing = p->next;
  //  } else if ( q->selected ) { // not the head, check the -next- item...
  //   if ( q == ListEnd ) {
  //      p->next = NULL;
  //      ListEnd = p;
  //    } else {
  //      p->next = p->next->next;
  //    }
  //    // Now it's time to actually free the memory...
  //    MemPtrFree ( q->name );
  //    MemPtrFree ( q );
  //  }
  //}
  // The above code does very bad things...

  // Re-sort the display list, and update the screen.
  //sortList(genericListSort);
  //showPage(frm);
}

static void listFillVolumes ( FormPtr formP, ControlPtr triggerP,
                              ListPtr listP, UInt32 selected ) {
  UInt32 it, i=0;
  UInt16 vn;

  // Make a pointer array to the strings... need it later.
  for ( i = 0; i < 16; i++ )
    volList[i] = volumeList[i];

  // Default the first item to "Internal Memory"
  StrCopy(volumeList[0], "Internal"); // Or RAM, whatever you want...
  volumeNums[0] = 0;

  i = 0;

  if ( CardEnabled ) {
    it = vfsIteratorStart;
    while (it != vfsIteratorStop) {
      i++;
      if ( VFSVolumeEnumerate ( &vn, &it ) )
        break;
      if ( VFSVolumeGetLabel ( vn, volumeList[i], 32 ) == vfsErrFileNotFound )
        StrCopy ( volumeList[i], "- Unnamed -" );
      volumeNums[i] = vn;
    }
  }
  //  i++;
  //  StrCopy(volumeList[i], "FTP Server...");
  //  volumeNums[i] = 0;
  volumeCount = i+1;

  LstSetListChoices ( listP, volList, volumeCount );
  LstSetHeight ( listP, volumeCount );

  if ( selected > 0 ) {
    for ( i=0; i < volumeCount; i++ ) {
      if ( volumeNums[i] == selected ) {
        LstSetSelection ( listP, i );
        CtlSetLabel ( triggerP, volumeList[i] );
        break;
      }
    }
  } else {
    LstSetSelection ( listP, 0 );
    CtlSetLabel ( triggerP, volumeList[0] );
    Preferences.Volume.Number = 0;
    startViewRAM(formP);
  }

  if ( CardEnabled ) {
//    if ( ( Preferences.Volume.Path ) &&
//       ( isDirectory(volumeNums[1], Preferences.Volume.Path) ) ) {
//      CurrentDirectory = (Char *)MemPtrNew(StrLen(Preferences.Volume.Path)+1);
//      StrCopy(CurrentDirectory,Preferences.Volume.Path);
//    } else {
//      CurrentDirectory = (Char *)MemPtrNew(2);
//      dirRoot();
//    }
  }

  if ( selected > 0 )
    startViewCard(formP);
}

static void mainFormInit ( FormPtr frmP ) {
  UInt16 i;
  TablePtr tableP = NULL;

  if ( HiResEnabled ) {
//    FldSetFont ( FrmGetObjectPtr ( frmP, FrmGetObjectIndex ( frmP, MAIN_NAMESORT_BTN, 1 );
  }

  tableP = (TablePtr)FrmGetObjectPtr ( frmP, FrmGetObjectIndex ( frmP, MAIN_TABLE ) );
  TblHasScrollBar ( tableP, true );
  tRows = TblGetNumberOfRows ( tableP );

  for ( i = 0; i < tRows; i++ ) {
    TblSetItemStyle ( tableP, i, 0, checkboxTableItem );
    TblSetItemStyle ( tableP, i, 1, customTableItem );
    TblSetItemStyle ( tableP, i, 2, customTableItem );
    TblSetItemStyle ( tableP, i, 3, customTableItem );
  }

  TblSetColumnUsable ( tableP, 0, true );
  TblSetColumnUsable ( tableP, 1, true );
  TblSetColumnUsable ( tableP, 2, true );
  TblSetColumnUsable ( tableP, 3, true );

  TblSetCustomDrawProcedure ( tableP, 1, &drawIconCell );
  TblSetCustomDrawProcedure ( tableP, 2, &drawNameCell );
  TblSetCustomDrawProcedure ( tableP, 3, &drawAttribCell );

  //if ( i < ListLength-tTop )
  //  TblSetRowUsable ( tableP, i, true );
  //TblSetRowID ( tableP, i, i );
  //TblMarkRowInvalid ( tableP, i ); // mark invalid so it redraws

  switch (Preferences.Display.Column) {
  case colSize:
    CtlSetLabel(getObjectPtr(frmP, MAIN_INFO_POPUP), "Size");
    break;
  case colType:
    CtlSetLabel(getObjectPtr(frmP, MAIN_INFO_POPUP), "Type");
    break;
  case colCreator:
    CtlSetLabel(getObjectPtr(frmP, MAIN_INFO_POPUP), "Creator");
    break;
  case colRecords:
    CtlSetLabel(getObjectPtr(frmP, MAIN_INFO_POPUP), "Records");
    break;
  case colDateCreated:
    CtlSetLabel(getObjectPtr(frmP, MAIN_INFO_POPUP), "CDate");
    break;
  case colDateModified:
    CtlSetLabel(getObjectPtr(frmP, MAIN_INFO_POPUP), "MDate");
    break;
  case colDateBackup:
    CtlSetLabel(getObjectPtr(frmP, MAIN_INFO_POPUP), "BDate");
    break;
  case colVersion:
    CtlSetLabel(getObjectPtr(frmP, MAIN_INFO_POPUP), "Version");
    break;
  case colAttributes:
    CtlSetLabel(getObjectPtr(frmP, MAIN_INFO_POPUP), "Attrib");
    break;
  }

  listFillVolumes ( frmP,
                    (ControlPtr)getObjectPtr(frmP, MAIN_CARD_POPUP),
                    (ListPtr)getObjectPtr(frmP, MAIN_CARD_LIST),
                    Preferences.Volume.Number );

  updateDirField(frmP);
}


static void doCFInfo(FormPtr frmP) {
  UInt32                usedBytes, totalBytes, bytesFree;     // VFS
  UInt32                romTotalBytes, totalSpace, freeSpace; // RAM
  Char                  totalStr[16];
  Char                  freeStr[16];
  Char                  tempStr[64];
  FormType             *popP;
  Err                   err;
  VolumeInfoType        VolumeInformation;
  Char                  VolumeLabel[32];
  Char                  RAMLabel[32];
  Char                  manufLabel[32];

  popP = FrmInitForm(INFO_FORM);

  // Get the main volume information.  Extended info dialog?
  err = VFSVolumeInfo ( volumeNums[1], &VolumeInformation );
  if ( err ) {
    StrPrintF(temp,"%ld",err);
    DEBUGINFO(temp);
  }

  // Get the volume label.  Extended info dialog?
  err = VFSVolumeGetLabel ( volumeNums[1], VolumeLabel, 32 );
  if ( err ) {
    StrPrintF(temp,"%ld",err);
    DEBUGINFO(temp);
  }
  SetFieldTextFromStrN ( popP, INFO_LABEL, VolumeLabel );

  err = VFSVolumeSize ( volumeNums[1], &usedBytes, &totalBytes );
  bytesFree = totalBytes - usedBytes;
  printNiceSize ( totalBytes, totalStr, 1 );
  printNiceSize ( bytesFree, freeStr, 1 );
  StrPrintF(tempStr, "%s of %s free", freeStr, totalStr );
  SetFieldTextFromStrN ( popP, INFO_NUM, tempStr );

  MemCardInfo (0, (Char *)&RAMLabel, (Char *)&manufLabel, NULL, NULL, &romTotalBytes, &totalSpace, &freeSpace );
  printNiceSize ( totalSpace, totalStr, 2 );
  printNiceSize ( freeSpace, freeStr, 2 );
  StrPrintF(tempStr, "%s of %s free", freeStr, totalStr );
  SetFieldTextFromStrN ( popP, INFO_RAMNUM, tempStr );

  SetFieldTextFromStrN ( popP, INFO_RAMLABEL, RAMLabel );
  SetFieldTextFromStrN ( popP, INFO_MANUFAC, manufLabel );

  FrmSetFocus ( popP, FrmGetObjectIndex ( popP, INFO_LABEL ) );

  // Why did the PalmOS types add all these types, if they were never going to use them?
  switch ( VolumeInformation.fsType ) {
  case vfsFilesystemType_AFS:
    SetFieldTextFromStrN ( popP, INFO_FS, "Unix Andrew (AFS)" );
    break;
  case vfsFilesystemType_EXT2:
    SetFieldTextFromStrN ( popP, INFO_FS, "Extended 2 (Linux)" );
    break;
  case vfsFilesystemType_FAT:
    SetFieldTextFromStrN ( popP, INFO_FS, "FAT 12/16" );
    break;
  case vfsFilesystemType_FFS:
    SetFieldTextFromStrN ( popP, INFO_FS, "Unix Berkly (Block)" );
    break;
  case vfsFilesystemType_HFS:
    SetFieldTextFromStrN ( popP, INFO_FS, "Macintosh (HFS)" );
    break;
  case vfsFilesystemType_HFSPlus:
    SetFieldTextFromStrN ( popP, INFO_FS, "Macintosh (HFS+)" );
    break;
  case vfsFilesystemType_HPFS:
    SetFieldTextFromStrN ( popP, INFO_FS, "OS2 HPFS" );
    break;
  case vfsFilesystemType_MFS:
    SetFieldTextFromStrN ( popP, INFO_FS, "Macintosh Original (MFS)" );
    break;
  case vfsFilesystemType_NFS:
    SetFieldTextFromStrN ( popP, INFO_FS, "Networked (NFS)" );
    break;
  case vfsFilesystemType_Novell:
    SetFieldTextFromStrN ( popP, INFO_FS, "Novell" );
    break;
  case vfsFilesystemType_NTFS:
    SetFieldTextFromStrN ( popP, INFO_FS, "Windows NT (NTFS)" );
    break;
  case vfsFilesystemType_VFAT:
    SetFieldTextFromStrN ( popP, INFO_FS, "Extended FAT 12/16" );
    break;
  default:
    SetFieldTextFromStrN ( popP, INFO_FS, "Unknown" );
    break;
  }

  switch ( VolumeInformation.mediaType ) {
  case expMediaType_Any:
    SetFieldTextFromStrN ( popP, INFO_MEDIA, "??? Wildcard?" );
    break;
  case expMediaType_MemoryStick:
    SetFieldTextFromStrN ( popP, INFO_MEDIA, "Memory Stick (MS)" );
    break;
  case expMediaType_CompactFlash:
    SetFieldTextFromStrN ( popP, INFO_MEDIA, "Compact Flash (CF)" );
    break;
  case expMediaType_SecureDigital:
    SetFieldTextFromStrN ( popP, INFO_MEDIA, "Secure Digital (SD)" );
    break;
  case expMediaType_MultiMediaCard:
    SetFieldTextFromStrN ( popP, INFO_MEDIA, "Multimedia Card (MMC)" );
    break;
  case expMediaType_SmartMedia:
    SetFieldTextFromStrN ( popP, INFO_MEDIA, "Smart Media (SM)" );
    break;
  case expMediaType_RAMDisk:
    SetFieldTextFromStrN ( popP, INFO_MEDIA, "RAM-Disk based" );
    break;
  case expMediaType_PoserHost:
    SetFieldTextFromStrN ( popP, INFO_MEDIA, "POSE Host" );
    break;
  case expMediaType_MacSim:
    SetFieldTextFromStrN ( popP, INFO_MEDIA, "RAM/ROM Card" );
    break;
  default:
    SetFieldTextFromStrN ( popP, INFO_MEDIA, "Unknown" );
    break;
  }

  if ( FrmDoDialog ( popP ) == INFO_OK )
    if ( StrCompare ( VolumeLabel, (Char *)FldGetTextPtr ( (FieldPtr)getObjectPtr (popP, INFO_LABEL ) ) ) )
      VFSVolumeSetLabel ( volumeNums[1], (Char *)FldGetTextPtr ( (FieldPtr)getObjectPtr ( popP, INFO_LABEL ) ) );

  FrmDeleteForm (popP);
}

static void doAbout(FormPtr frmP) {
  Char fafmVerStr[16];
  FormType *aboutForm;
  UInt16 selection;

  // initialize the form
  aboutForm = FrmInitForm(ABOUT_FORM);

  // figure out version string
  StrIToA(fafmVerStr, BUILD_NUMBER);
  FldSetTextPtr(getObjectPtr(aboutForm, ABOUT_VERSION), fafmVerStr);

  selection = FrmDoDialog(aboutForm);

  FrmDeleteForm(aboutForm);
}



static void doPreferences ( FormPtr frmP ) {
  FormPtr popP = FrmInitForm ( SETTINGS_FORM );

  if (Preferences.General.LeftHanded) {
    CtlSetLabel(getObjectPtr(popP, APPEARANCE_POPUP), "left-handed");
  } else {
    CtlSetLabel(getObjectPtr(popP, APPEARANCE_POPUP), "right-handed");
  }

  CtlSetValue(getObjectPtr(popP, SET_SHOW_ICONS_CB), Preferences.Display.ShowIcons);
  CtlSetValue(getObjectPtr(popP, SET_BOLD_DIRECTORIES_CB), Preferences.Display.BoldDirectories);
  CtlSetValue(getObjectPtr(popP, SET_SHOW_ROM_CB), Preferences.Display.ShowROM);
  CtlSetValue(getObjectPtr(popP, SET_USE_COLOR_CB), Preferences.Display.UseColor);
  CtlSetValue(getObjectPtr(popP, SET_SHOW_HIDDEN_CB), Preferences.Display.ShowHidden);
  CtlSetValue(getObjectPtr(popP, SET_DIRECTORIES_FIRST_CB), Preferences.Display.DirectoriesFirst);
  CtlSetValue(getObjectPtr(popP, SET_PALM_ONLY_CB), Preferences.Display.PalmOnly);
  CtlSetValue(getObjectPtr(popP, SET_REVERSE_SORT_CB), Preferences.Display.ReverseSort);
  CtlSetValue(getObjectPtr(popP, SET_START_SORT_CB), Preferences.Display.StartSorted);

  if ( FrmDoDialog ( popP ) == SETTINGS_SAVE_BTN ) {
    Preferences.Display.ShowIcons = CtlGetValue(getObjectPtr(popP, SET_SHOW_ICONS_CB));
    Preferences.Display.BoldDirectories = CtlGetValue(getObjectPtr(popP, SET_BOLD_DIRECTORIES_CB));
    Preferences.Display.ShowROM = CtlGetValue(getObjectPtr(popP, SET_SHOW_ROM_CB));
    Preferences.Display.UseColor = CtlGetValue(getObjectPtr(popP, SET_USE_COLOR_CB));
    Preferences.Display.ShowHidden = CtlGetValue(getObjectPtr(popP, SET_SHOW_HIDDEN_CB));
    Preferences.Display.DirectoriesFirst = CtlGetValue(getObjectPtr(popP, SET_DIRECTORIES_FIRST_CB));
    Preferences.Display.PalmOnly = CtlGetValue(getObjectPtr(popP, SET_PALM_ONLY_CB));
    Preferences.Display.ReverseSort = CtlGetValue(getObjectPtr(popP, SET_REVERSE_SORT_CB));
    Preferences.Display.StartSorted = CtlGetValue(getObjectPtr(popP, SET_START_SORT_CB));
    if ( Preferences.Volume.Number == 0 )
      startViewRAM(frmP);
    else
      startViewCard(frmP);
    showPage(frmP);
  }

  FrmDeleteForm(popP);
}


static Boolean doMenu(FormPtr frmP, UInt16 command) {
  Boolean handled = false;

  switch ( command ) {
  case HELP_ABOUT:
    doAbout ( frmP );
    handled = true;
    break;

  case HELP_RELEASE:
    FrmInformation ( 0, 9000, "Release Notes" );
    handled = true;
    break;

  case OPTIONS_PREFS:
    doPreferences ( frmP );
    handled = true;
    break;

  case OPTIONS_SORT:
    handled = true;
    break;

  case VOLUME_CREATE_DIR:
    createDirectory();
    FrmUpdateForm(mainFormID, 1);
    handled = true;
    break;

  case VOLUME_REFRESH:
    if ( Preferences.Volume.Number == 0 )
      startViewRAM(frmP);
    else
      startViewCard(frmP);
    showPage(frmP);
    handled = true;
    break;

  case VOLUME_FIND_FILES:
    handled = true;
    break;

  case VOLUME_INFO:
    doCFInfo(frmP);
    handled = true;
    break;

  }

  return handled;
}


static Boolean doScroll ( FormPtr frmP, ScrollBarType *sb,
                          UInt16 value, UInt16 newvalue,
                          Boolean clearTypeDown ) {
  tTop = newvalue;
  if (clearTypeDown) {
    typeDownIndex_g = 0;
  }

  showPage(frmP);
  return true;
}


static void doTypeDown(FormPtr frmP) {
  UInt16 tdIndex;
  UInt16 i;
  UInt16 matchCount = 0;
  UInt16 dbIndex = 0;
  UInt16 bestMatch = 0;
  Char   *blorg, a, b;

  for (i = dbIndex; i < ListLength; i++ ) {
    matchCount = 0;
    for (tdIndex = 0; tdIndex < typeDownIndex_g; tdIndex++) {
      if ( Preferences.Display.Sort <= sortByName ) {
        a = SortedList[i]->name[tdIndex];
        b = typeDownBuf_g[tdIndex];
        if ( a > 'z' )
          a -= 26;
        if ( b > 'z' )
          a -= 26;
        if ( a == b ) {
          // matched on that char - count match
          matchCount++;
        } else {
          break;
        }
      } else if ( Preferences.Display.Sort == sortByRightColumn ) {
        if ( Preferences.Display.Column  == colCreator) {
          blorg = (Char *)&SortedList[i]->ID.creator;
          if ( blorg[tdIndex] == typeDownBuf_g[tdIndex] ) {
            matchCount++;
          } else {
            break;
          }

        } else if ( Preferences.Display.Column == colType ) {
          blorg = (Char *)&SortedList[i]->ID.type;
          if ( blorg[tdIndex] == typeDownBuf_g[tdIndex] ) {
            matchCount++;
          } else {
            break;
          }

        } else {
          if ( SortedList[i]->name[tdIndex] == typeDownBuf_g[tdIndex] ) {
            // matched on that char - count match
            matchCount++;
          } else {
            break;
          }
        }
      }
    }

    if (matchCount > bestMatch) {
      bestMatch = matchCount;
      dbIndex = i;
    }
  }

  if (bestMatch < typeDownIndex_g) {
    // didn't find a complete match
    // sound an error
    SndPlaySystemSound(sndError);
    // throw out the mismatching character
    typeDownIndex_g--;
  }

  doScroll(frmP, NULL, dbIndex, dbIndex, false);
}


static void doNameTap ( FormPtr frmP, TablePtr tableP, UInt32 item, UInt8 column ) {
  UInt8 row = item - tTop;

  // TblSetItemInt

  if ( ( SortedList[item]->attributes & vfsFileAttrDirectory ) &&
       ( column > 0 ) ) {
    // if root dir go there
    if (!StrCompare(SortedList[item]->name, RootDirectoryString)) {
      tTop = 0;
      dirRoot();
      startViewCard(frmP);
    }

    // if parent dir go up one
    else if (!StrCompare(SortedList[item]->name, ParentDirectoryString)) {
      tTop = 0;
      dirUp();
      startViewCard(frmP);
    }

    // else change to the directory
    else {
      tTop = 0;
      dirDown ( SortedList[item]->name, frmP );
    }

    // update the form
    FrmUpdateForm(mainFormID, 1);
  }

  // else handle file taps, or check the check box
  else {
    //doPluginList(frmP, tTop + item);

    TblSetItemInt ( tableP, row, 0, !TblGetItemInt(tableP, row, 0) );
    SortedList[item]->selected = TblGetItemInt(tableP, row, 0 );
    TblMarkRowInvalid ( tableP, row );
    if ( PalmOSVersion >= PalmOS35 ) WinScreenLock ( winLockCopy );
    TblRedrawTable ( tableP );
    if ( PalmOSVersion >= PalmOS35 ) WinScreenUnlock ( );
  }
}


Boolean mainFormEventHandler ( EventPtr eventP ) {
  Boolean handled = false, inverted = false, penDown = false;
  FormPtr frmP = FrmGetActiveForm();
  TablePtr tableP;
  UInt16 row, column, x, y;
  RectangleType r = { { 0, 0 }, { 0, 0 } };

  switch (eventP->eType) {
  case frmOpenEvent:
/*  if (Preferences.General.LeftHanded) {
      mainFormID = MAIN_FORM_LEFT;
    } else {
      mainFormID = MAIN_FORM;
    } */
    if ( PalmOSVersion >= PalmOS35 ) WinScreenLock ( winLockErase );
    FrmDrawForm(frmP);
    WinEraseLine ( 0, 130, 160, 130 );
    WinDrawLine ( 0, 131, 160, 131 );
    WinDrawLine ( 0, 16+12, 160, 16+12 );
    if ( PalmOSVersion >= PalmOS35 ) WinScreenUnlock ( );
    mainFormInit(frmP);
    handled = true;
    break;

  case frmUpdateEvent:
    if ( PalmOSVersion >= PalmOS35 ) WinScreenLock ( winLockCopy );
    FrmDrawForm ( frmP );
    WinEraseLine ( 0, 130, 160, 130 );
    WinDrawLine ( 0, 131, 160, 131 );
    WinDrawLine ( 0, 16+12, 160, 16+12 );
    if ( PalmOSVersion >= PalmOS35 ) WinScreenUnlock ( );

    handled = true;
    break;

  case menuEvent:
    handled = doMenu(frmP, eventP->data.menu.itemID);
    break;

  case keyDownEvent:
    if ( eventP->data.keyDown.chr == pageUpChr ) {
      if ( tTop > tRows ) {
        tTop -= tRows;
      } else if ( tTop != 0 ) {
        tTop = 0;
      }
      handled = true;

      showPage(frmP);
    } else if ( eventP->data.keyDown.chr == pageDownChr ) {
      if ( ListLength-tTop > tRows+1 ) {
        tTop += tRows;
      }
      if ( ( tTop+tRows >= ListLength-tRows+1 ) &&
           ( tTop != ListLength-tRows+1 ) ) {
        tTop = ListLength-tRows+1;
      }
      handled = true;
      showPage(frmP);

    } else if (eventP->data.keyDown.chr == chrBackspace) {
      if (typeDownIndex_g > 0)
        typeDownIndex_g--;
    } else if (!(TxtCharIsCntrl(eventP->data.keyDown.chr))) {
      // add to the type-down buffer
      typeDownBuf_g[typeDownIndex_g++] = eventP->data.keyDown.chr;
      doTypeDown(frmP);
    }
    break;

  case sclExitEvent:
    if (eventP->data.sclExit.scrollBarID == MAIN_SCROLLBAR) {
      handled = doScroll(frmP, eventP->data.sclExit.pScrollBar,
                         eventP->data.sclExit.value,
                         eventP->data.sclExit.newValue, true);
    } else {
      handled = false;
    }
    break;

  case sclRepeatEvent:
    if (eventP->data.sclRepeat.scrollBarID == MAIN_SCROLLBAR) {
      if (eventP->data.sclRepeat.newValue != tTop) {
        handled = doScroll(frmP, eventP->data.sclRepeat.pScrollBar,
                           eventP->data.sclRepeat.value,
                           eventP->data.sclRepeat.newValue, true);
      }
    } else {
      handled = false;
    }

    SclHandleEvent(eventP->data.sclRepeat.pScrollBar, eventP);
    SclDrawScrollBar(eventP->data.sclRepeat.pScrollBar);
    break;

  case tblEnterEvent:
    row = eventP->data.tblEnter.row;
    column = eventP->data.tblEnter.column;
    tableP = eventP->data.tblEnter.pTable;

    if ( row+tTop < ListLength+1 ) {
      TblGetItemBounds ( tableP, row, column, &r );
      r.topLeft.x = 0; // 12 to not invert check box...
      r.extent.x = 160-8;
      r.extent.y++;
      WinInvertRectangle ( &r, 0 );
      inverted = true;
      penDown = true;
      while ( penDown ) {
        PenGetPoint (&x, &y, &penDown);
        if ( RctPtInRectangle( x, y, &r ) ) {
          if ( !inverted ) {
            WinInvertRectangle ( &r, 0 );
            inverted = true;
          }
        } else {
          if ( inverted ) {
            WinInvertRectangle ( &r, 0 );
            inverted = false;
          }
        }
      }

      if ( RctPtInRectangle ( x, y, &r ) ) {
        //i = PopListAtCursor ( (ListPtr)FrmGetObjectPtr ( formP, FrmGetObjectIndex ( formP, MainOperationList ) ) );
        WinInvertRectangle ( &r, 0 );

        if ( ( column > 0 ) ||
             StrCompare(SortedList[tTop+row]->name,RootDirectoryString) ||
             StrCompare(SortedList[tTop+row]->name,ParentDirectoryString) )
          doNameTap ( frmP, tableP, tTop + row, column );
      }
    }

    handled = true;
    break;

  case popSelectEvent:
    switch (eventP->data.popSelect.controlID) {
    case MAIN_INFO_POPUP:
      Preferences.Display.Column = eventP->data.popSelect.selection;

      if ( Preferences.Display.Sort == sortByRightColumn )
        Preferences.Display.ReverseSort = !Preferences.Display.ReverseSort;
      else {
        Preferences.Display.ReverseSort = false;
        Preferences.Display.Sort = sortByRightColumn;
      }
      tTop = 0;
      sortList ( genericListSort );
      showPage ( frmP );

      //handled = true;
      break;

    case MAIN_CARD_POPUP:
      if ( volumeNums[eventP->data.popSelect.selection] != Preferences.Volume.Number ) {
        LstSetSelection ( eventP->data.popSelect.listP,
                          eventP->data.popSelect.selection );
        if ( eventP->data.popSelect.selection == 0 ) {
          Preferences.Volume.Number = 0;
          tTop = 0;
          startViewRAM(frmP);
        } else if ( eventP->data.popSelect.selection == LstGetNumberOfItems(eventP->data.popSelect.listP ) ) {
          // handle FTP servers...
        } else {
          Preferences.Volume.Number = volumeNums[eventP->data.popSelect.selection];
          tTop = 0;
          startViewCard(frmP);
        }
        CtlSetLabel ( eventP->data.popSelect.controlP,
                      volumeList[eventP->data.popSelect.selection] );
      }
      break;

    }
    break;

  case ctlSelectEvent:
    switch (eventP->data.ctlSelect.controlID) {
      case MAIN_GOHOME:
        if ( ( Preferences.Volume.Number > 0 ) && ( isDirectory ( Preferences.Volume.Number, Preferences.HomePath ) ) ) {
          dirDown(Preferences.HomePath, frmP);
//          startViewCard(frmP);
        } else if ( Preferences.Volume.Number > 0 ) {
        } else DEBUGINFO("NOTPATH!");
        break;
        
      case MAIN_COPY_BTN :
        doCopyMove(frmP, false);
        handled = true;
        break;

    case MAIN_MOVE_BTN :
      doCopyMove(frmP, true);
      handled = true;
      break;

    case MAIN_DEL_BTN :
      doDelete(frmP);
      handled = true;
      break;

    case MAIN_SELECT_BTN :
      row = LstPopupList(FrmGetObjectPtr(frmP, FrmGetObjectIndex(frmP, MAIN_SELECT_LIST)));
      switch ( row ) {
      case 0: // Select All
        doSelectAll();
        break;
      case 1: // Select None
        doSelectNone();
        break;
      case 2: // Select Invert
        doSelectInvert();
        break;
      default: // Do nothing...
        break;
      }
      showPage(frmP);
      break;

      //      case MAIN_INFO_BTN:
      //        doCFInfo(frmP);
      //        break;

    case MAIN_NAMESORT_BTN:
      if ( Preferences.Display.Sort == sortByName )
        Preferences.Display.ReverseSort = !Preferences.Display.ReverseSort;
      else {
        Preferences.Display.Sort = sortByName;
        Preferences.Display.ReverseSort = false;
      }
      tTop = 0;
      sortList ( genericListSort );
      showPage ( frmP );
      break;

    case MAIN_DIR_BTN:
      FrmPopupForm ( DIRECTORY_FORM );
      break;
    }
    break;

  default:
    break;
  }

  return handled;
}
