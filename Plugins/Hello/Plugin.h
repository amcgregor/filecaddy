#ifndef _PLUGIN_H_
#define _PLUGIN_H_

// $Id: Plugin.h,v 1.2 2002/09/24 04:38:19 amcgregor Exp $

// Copyright (c) 2002, Alice Bevan-McGregor

// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
                        
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
                                        
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

// $Log: Plugin.h,v $
// Revision 1.2  2002/09/24 04:38:19  amcgregor
// Updates to the plugin source, makefiles.
//
// Revision 1.1  2002/09/12 13:04:58  amcgregor
// Initial import.
//
//

#define kPlugInFileType     'PLUG'
#define kPlugInFileCreator  'FCad'
#define kPlugInResourceType 'code'
#define kFirstPlugInResID   0

#define plugVersion                       0x0100

typedef enum {
  cmdNoOp = 0,                            // plugin called to do nothing
  cmdIdle,                                // plugin called when app idle
  
  cmdStartup = 5,                         // plugin is loaded and first run
  cmdShutdown,                            // plugin is run for the last time
  cmdRestart,                             // plugin restarted by user
  cmdGetInfo,                             // hack style about dialog
  cmdSetup,                               // hack style configuration dialog
  
  cmdChangeVolume = 1000,                 // user requested new volume
  cmdChangeDirectory,                     // user tapped a directory entry
  cmdMenuTap,                             // user chose plugin from the popup
  cmdFileCopy,                            // user requesting to copy file(s)
  cmdFileMove,                            // user requesting to move file(s)
  cmdDelete,                              // user requesting to delete file

  // the following are used to emulate a new VFS drive...
  cmdVFSDirCreate = 2000,                 // create directory
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
  cmdVFSVolumeUnmount,                    // unmount mounted volume
  
  cmdPlugCustomBase                       // base point for custom functions
                                          // big number = lots of standard functions ;)
} PlugCmd;

#define plugNoErr                   0
#define plugErrIncompatible         1
#define plugErrGlobalSpace          2
#define plugErrNotImplemented       3

typedef struct {
  Boolean menuItem;                       // does this plugin request a menu item?
  Boolean vfsHandler;                     // is this plugin a VFS handler?
  Boolean userInitiated;                  // did the user activate the plugin?
  
  UInt32  creatorType;
  UInt32  creatorID;
  Char   *extension;
} _cmdStartup;

typedef struct {
  Boolean userInitiated;                  // user or system shutdown?
} _cmdShutdown;

typedef struct {
  UInt16  oldVolume;                      // old volume number
  UInt16  newVolume;                      // new volume number
  Boolean allowed;                        // allow this volume change?
} _cmdChangeVolume;

typedef struct {
  UInt16  volume;                         // volume number
  Char   *oldDir;                         // previous directory
  Char   *newDir;                         // new directory
  Boolean allowed;                        // allow this directory change?
} _cmdChangeDirectory;

typedef struct {
  UInt16  volume;                         // volume number
  Char   *path;                           // current directory
  Char  **files;                          // selected file list
} _cmdMenuTap;

typedef struct {
  UInt16  srcVolume;
  UInt16  destVolume;
  Char   *srcPath;
  Char   *destPath;
  Char   *srcFile;
  Char   *destFile;
  Boolean allowed;
} _cmdFileCopy;

typedef struct {
  UInt16  srcVolume;
  UInt16  destVolume;
  Char   *srcPath;
  Char   *destPath;
  Char   *srcFile;
  Char   *destFile;
  Boolean allowed;
} _cmdFileMove;

typedef struct {
  UInt16  volume;
  Char   *file;
  Boolean allowed;
} _cmdFileDelete;

typedef struct {
  void   *globals;      // pointer to plugin-allocated space
  UInt32  errno;        // error number set by plugin
  PlugCmd eType;        // type of event being passed
  UInt16  hostVersion;  // version number of the plugin system...
  
  union {
    _cmdStartup         startup;
    _cmdShutdown        shutdown;
    _cmdChangeVolume    changeVolume;
    _cmdChangeDirectory changeDirectory;
    _cmdMenuTap         menuTap;
    _cmdFileCopy        fileCopy;
    _cmdFileMove        fileMove;
    _cmdFileDelete      fileDelete;
    
    // ...
  } data;
} Plug_ParamBlock;
typedef Plug_ParamBlock* Plug_ParamBlockPtr;

typedef UInt32 Plug_Main  ( Plug_ParamBlockPtr eventP );

typedef struct {
  Char      *longname;
  Char      *shortname;
  DmOpenRef  gPlugInDbRef;
  Plug_Main *gPlugInResP;
  
} Plug_InternalRef;

#endif
