#ifndef _PLUGIN_H_
#define _PLUGIN_H_

// $Id: PlugIn.h,v 1.8 2002/08/01 16:47:06 amcgregor Exp $

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

// $Log: PlugIn.h,v $
// Revision 1.8  2002/08/01 16:47:06  amcgregor
// Updated release notes, plugin system worked upon, and makefile no longer builds iSilo manual.
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
// Revision 1.4  2002/07/16 00:59:43  amcgregor
// Initial public release, bugfixes, an aborted attempt at using the IconCache.
//
// Revision 1.3  2002/07/13 22:16:13  amcgregor
// Offline version to CVS miscelaneous updates.
//
// Revision 1.1  2002/07/11 14:46:55  root
// Initial revision
//

#define PLUG_TYPE   'Plug'

// You can register to handle any of the following:
typedef UInt32 Plug_Shutdown ( void *ep, void *gp, UInt16 stage );
typedef UInt32 Plug_Restart  ( void *ep, void *gp, UInt16 stage );

typedef UInt32 Plug_onGetInfo         ( void *gp );
typedef UInt32 Plug_onSetup           ( void *gp );
typedef UInt32 Plug_onChangeVolume    ( void *gp, UInt16 oldVolume, UInt16 newVolume );
typedef UInt32 Plug_onChangeDirectory ( void *gp, UInt16 volume, Char *oldDirectory, Char *newDirectory );
typedef UInt32 Plug_onMenuTap         ( void *gp, UInt16 volume, Char *directory, Char *file );
typedef UInt32 Plug_onFileCopy        ( void *gp, UInt16 volume[2], Char *directory[2], Char *file[2] );
typedef UInt32 Plug_onFileMove        ( void *gp, UInt16 volume[2], Char *directory[2], Char *file[2] );
typedef UInt32 Plug_onDelete          ( void *gp, UInt16 volume, Char* path, Boolean directory );

typedef UInt32 PlugInVFSEntry ( void *gp, UInt32, ... );
typedef UInt32 PlugInEntryPointEx ( UInt32, ... );

typedef struct {
  Plug_Shutdown          *PlugInShutdown;     // called when plugin is removed or app is quit
  Plug_Restart           *PlugInRestart;      // currently never called

  Plug_onGetInfo         *onGetInfo;          // called to display an "about" screen
  Plug_onSetup           *onSetup;            // called to display an "options" screen
  Plug_onChangevolume    *onChangeVolume;     // 0 = internal, 512 = FTP server
  Plug_onChangeDirectory *onChangeDirectory;  // also called after changing volume
  Plug_onMenuTap         *onMenuTap;          // if !null, plugin name will appear in list
  Plug_onFileCopy        *onFileCopy;         // called before and after the file is copied
  Plug_onFileMove        *onFileMove;         // called before and after the file is moved
  Plug_onDelete          *onDelete;           // called before and after the file is deleted

  PlugInVFSEntry         *vfsEntry;           // used to simulate VFS for some other device
  PlugInEntryPointEx     *extended;           // a cheap way out in case we miss something
} EntryPoints;

// The basic function.  You _must_ have at least this.
// Standard arguments: ep - entry point list, set used entry points
//                     gp - global pointer, use to store plugin globals
//                     stage - startup/shutdown stage
typedef UInt32 PlugInStartup  ( EntryPoints *ep, void *gp, UInt16 stage );


typedef struct PlugIn_t {
  Char     name[32];    // loaded from 'tAIN'
  Char     version[16]; // loaded from 'tver' (major/minor i.e. 1.2)
  void    *globals;     // allocated by PlugInStartup

  UInt16   cardNo;
  LocalID  dbID;

  Boolean  active;      // has the plugin been started?
  Boolean  enabled;     // is the plugin enabled (if not, entrylist may be invalid)

  PlugInStartup  *startup;    // plugin startup function - called on load
  PlugInShutdown *shutdown;   // plugin shutdown function - called before quit
  PlugInRestart  *restart;    // plugin restart function - should shutdown the startup
  EntryPoints     entrylist;  // standard entry points

  //struct PlugIn_t *next;
  //struct PlugIn_t *prev;
} PlugIn;

//extern PlugIn  *PlugInList;

typedef enum {
} PlugCmd;

typedef enum {
  cmdVFSDirCreate,                        // create directory
  cmdVFSDirEntryEnumerate,                // scan directory
  cmdVFSExportDatabaseToFile,             // copy DB to file
  cmdVFSExportDatabaseToFileCustom,       // copy DB to file w/ progress
  cmdVFSExportFileToFile,                 // copy file to file
  cmdVFSExportFileToFileCustom,           // copy file to file w/ progress
  cmdVFSFileClose,                        // close open file
  cmdVFSFileCreate,                       // create empty file
  cmdVFSFileDBInfo,                       // get palmOS database information
  cmdVFSFileDelete,                       // delete file
  cmdVFSFileEOF,                          // at end of open file
  cmdVFSFileGetAttributes,                // get file attributes (chmod)
  cmdVFSFileGetDate,                      // get created/modified/accessed times
  cmdVFSFileOpen,                         // open file
  cmdVFSFileRead,                         // read data from file
  cmdVFSFileRename,                       // rename file
  cmdVFSFileResize,                       // resize file
  cmdVFSFileSeek,                         // seek in open file
  cmdVFSFileSetAttributes,                // set attributes (chmod)
  cmdVFSFileSetDate,                      // set file created/modified/accessed times
  cmdVFSFileSize,                         // get file size
  cmdVFSFileTell,                         // get current location in open file
  cmdVFSFileWrite,                        // write data to file
  cmdVFSImportDatabaseFromFile,           // copy file->internal database
  cmdVFSImportDatabaseFromFileCustom,     // copy file->internal database w/ progress
  cmdVFSImportFileFromFile,               // copy file->file
  cmdVFSImportFileFromFileCustom,         // copy file->file w/ progress
  cmdVFSVolumeEnumerate,                  // find available volumes (drives)
  cmdVFSVolumeGetLabel,                   // get volume label
  cmdVFSVolumeInfo,                       // get volume extended information
  cmdVFSVolumeMount,                      // mount volume
  cmdVFSVolumeSetLabel,                   // set volume label
  cmdVFSVolumeSize,                       // get free/used space on volume (may not apply)
  cmdVFSVolumeUnmount                     // unmount mounted volume
} PlugVFSCmd;

#endif
