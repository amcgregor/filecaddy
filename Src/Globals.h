#ifndef _GLOBALS_H_
#define _GLOBALS_H_

// $Id: Globals.h,v 1.8 2002/08/25 13:09:41 amcgregor Exp $

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

// $Log: Globals.h,v $
// Revision 1.8  2002/08/25 13:09:41  amcgregor
// Bugfixes (closing files,) "Home" button, and Version right column added.
//
// Revision 1.7  2002/07/25 15:09:00  amcgregor
// Second thought: tabs bad.
//
// Revision 1.6  2002/07/25 11:51:53  amcgregor
// Code run through: astyle --style=kr --indent=tab=2 -S *.c *.h
//
// Revision 1.5  2002/07/21 19:08:36  amcgregor
// Bugfixes and added features.
//
// Revision 1.4  2002/07/18 07:55:48  amcgregor
// Added release notes dialog, fixed crash w/ no VFS
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


// - GLOBAL DEFINES ----------------------------------------------------------
#define AppCreator  'FCad'

#define MaxDirectoryDepth 50
#define MaxFileNameLength 36

// The following are used to substitute values for odd entries.  Direcoties
// and files that are not Palm databases are included, as well as the root/
// parent directory entries.
#define RootDirectoryString "[Root Directory]"
#define ParentDirectoryString "[Parent Directory]"
#define DirectoryCreatorID 'DIR '
#define DirectoryTypeID  'DIR '
#define DirectorySizeString "<DIR>"

#define MaxPlugins  20

#ifndef BUILD_NUMBER
#define BUILD_NUMBER  1
#endif
#define FM_VERSION BUILD_NUMBER

#define PalmOS3  0x03003000
#define PalmOS31 0x03103000
#define PalmOS35 0x03503000
#define PalmOS4  0x04003000

// - STRUCTURES --------------------------------------------------------------
enum {              // the different types of display on the right column
  colSize = 0,      // display file size (in whatever format is applicable)
  colCreator,       // display the creator ID ('MSvb')
  colType,          // display the type ID ('appl')
  colRecords,       // display the number of records in palm data files
  colDateCreated,   // display the date the file was created
  colDateModified,  // display the date the file was last modified
  colDateBackup,    // display the date the file was last backed up (internal)
  colVersion,       // display file's version number (N/A, if not an app ;)
  colAttributes,    // display the file attributes ('bRt', 'boR', 'bpt', &c.)
  colEndList        // a useful end marker
} columnViewL;

enum {
  sortNone = 0,
  sortByName,
  sortByRightColumn,
  sortEndList
} sortModeL;

enum {
  NotPalmDB = false,
  PalmDatabase = 1,
  PalmResourceDatabase,
  PalmQueryDatabase
} databaseType;

typedef struct {
  UInt32 Version;

  struct {
    Boolean LeftHanded;
    UInt32 InstallTS;         // When was FileCaddy installed on the device?
    UInt32 UpdateReminder;    // Countdown timer until reminder is shown.
  } General;

  struct {
    UInt16 Number;
    Char Path[MaxDirectoryDepth * MaxFileNameLength + 1];
  } Volume;

  struct {
    Boolean ShowIcons;
    Boolean BoldDirectories;
    Boolean ShowROM;
    Boolean UseColor;
    Boolean ShowHidden;
    Boolean DirectoriesFirst;
    Boolean PalmOnly;
    Boolean StartSorted;
    Boolean ReverseSort;
    UInt8 Column;
    UInt8 Sort;
  } Display;
  
  Boolean HomeIsRoot;
  Char HomePath[MaxDirectoryDepth * MaxFileNameLength + 1];
} TPreferences;

typedef struct TDirectoryListItem {
  struct {
    UInt32 creator;
    UInt32 type;
  } ID;
  struct {
    UInt32 created;
    UInt32 modified;
    UInt32 backedUp;
  } date;
  UInt32 size;
  UInt32 modnum;
  UInt32 records;
  UInt16 version;
  UInt32 attributes;
  Boolean selected;
  Char *name;

  struct TDirectoryListItem *next;
} DirectoryListItem;

typedef struct {
  UInt16 cardNo;
  LocalID dbID;
  Char name[MaxFileNameLength];
  Boolean active;
} PlugIn_t;

// - GLOBAL VARIABLES --------------------------------------------------------

// From: AppMain.c
extern TPreferences            Preferences;
extern Boolean                 CardEnabled, HiResEnabled;
extern UInt32                  ScreenWidth, ScreenHeight;
extern UInt16                  HRLibRef;
extern UInt32                  PalmOSVersion;
extern BitmapType             *FolderIconB, *FileIconB, *TextFileIconB;
extern Char                    DebugMessage[128];
extern UInt16                  mainFormID;

// From: MainForm.c
extern DirectoryListItem      *Listing;
extern DirectoryListItem      *ListEnd;
extern DirectoryListItem     **SortedList;
extern UInt32                  ListLength;
extern Char                   *CurrentDirectory;

#endif // _GLOBALS_H_
