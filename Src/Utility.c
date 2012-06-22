#include <PalmOS.h>
#include <VFSMgr.h>
#include "Resource.h"
#include "Globals.h"

// $Id: Utility.c,v 1.8 2002/08/25 13:09:41 amcgregor Exp $

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

// $Log: Utility.c,v $
// Revision 1.8  2002/08/25 13:09:41  amcgregor
// Bugfixes (closing files,) "Home" button, and Version right column added.
//
// Revision 1.7  2002/07/25 15:09:00  amcgregor
// Second thought: tabs bad.
//
// Revision 1.6  2002/07/25 11:51:53  amcgregor
// Code run through: astyle --style=kr --indent=tab=2 -S *.c *.h
//
// Revision 1.5  2002/07/19 18:58:41  amcgregor
// Enhanced Volume Information dialog
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


// Function:    getObjectPtr
// Description: A helper function to easily get the pointer to an on-screen
//              object.  Could really be a macro...
// Arguments:   FormPtr frmP - Form to get object pointer from.
//              Int16  index - The object index to get the pointer to.
// Returns:     VoidPtr      - The object pointer.
inline void *getObjectPtr ( FormPtr frmP, Int16 index ) {
  UInt16 fldIndex = FrmGetObjectIndex ( frmP, index );
  return FrmGetObjectPtr ( frmP, fldIndex );
}


// Function:    printNiceSize
// Description: Return a pretty-printed version of a number. (1024 -> 1K)
// Arguments:   UInt32  size - The size, in bytes.
//              CharPtr dest - The destination buffer.
//              UInt8   accuracy - the number of decimals
// Returns:     CharPtr      - The destination buffer.
Char *printNiceSize ( UInt32 size, Char *dest, UInt8 accuracy ) {
  UInt8   magnitude = 0;
  Char    magnitudeChar[4][3] = { "B", "KB", "MB", "GB" };
  UInt32  accuracyMod = 1;

  if ( size == 0 ) {
    StrCopy ( dest, "0" );
    return dest;
  }

  if ( accuracy > 0 ) {
    while ( accuracy ) {
      accuracyMod *= 10;
      accuracy--;
    }
  }

  size *= accuracyMod;  // two decimal places

  while ( size > (1024 * accuracyMod) ) {
    size = size >> 10;
    magnitude++;
  }

  if ( ( size % accuracyMod ) == 0 )
    StrPrintF ( dest, "%ld%s", ( size / accuracyMod ), magnitudeChar[magnitude] );
  else
    StrPrintF( dest, "%ld.%ld%s",
               ( size / accuracyMod ),
               ( size % accuracyMod ),
               magnitudeChar[magnitude] );

  return dest;
}


// Function:    checkBattery
// Description: Check to see if the battery is within the warning limit,
//              and if so, warn the user.
// Arguments:   None
// Returns:     Boolean - True  - User wishes to cancel.
//                        False - User wishes to continue.
// Comments:    Instead of using a hard-coded value, allow the user to
//              choose within the preferences?
Boolean checkBattery ( void ) {
  UInt8 percent;
  Char percentStr[5];
  UInt16 selection;

  SysBatteryInfo(false, NULL, NULL, NULL, NULL, NULL, &percent);
  if (percent < 15) {
    StrPrintF(percentStr, "%d%%", percent);
    selection = FrmCustomAlert(ALERT_BATTERY, percentStr, "", "");
    if (selection == 0) {
      return false;
    } else {
      return true;
    }
  }

  return true;
}


// Function:    isROMDB
// Description: Check to see if the database is in ROM, instead of RAM.
// Arguments:   UInt16 card - Which memory card is the database on?
//              LocalID lid - What is the local ID for the database?
// Returns:     Boolean - True  - The database -is- in ROM.
//                        False - The database is -not- in ROM.
//                                The database may not even be in RAM...
Boolean isROMDB ( UInt16 card, LocalID lid ) {
  void *phan;
  unsigned short heapid;

  phan = MemLocalIDToGlobal(lid, card);
  if (MemLocalIDKind(lid) == memIDHandle)
    heapid = MemHandleHeapID(phan);
  else
    heapid = MemPtrHeapID(phan);
  if (MemHeapFlags(heapid) & 0x0001)  /* memHeapFlagReadOnly */
    return true;
  else
    return false;
}


// Function:    StrRChr
// Description: System function to return the last matching character.
// Arguments:   CharPtr str - The string to search.
//              Char    chr - The character to search for.
// Returns:     CharPtr - The location in the source string the caharacter
//                        was found.  A pointer into the string.
inline Char *StrRChr ( const Char *str, const Char chr ) {
  Char *p = (Char *)str+StrLen(str)-1;
  while ( ( p >= str ) && ( *p != chr ) )
    p--;
  return (Char *)p;
}


// Function:    FileName & Extension
// Description: Utility functions to return the filename or extension of a
//              fully qualified path, in UNIX/PalmVFS format.
// Arguments:   CharPtr filename - The full path to the file.
// Returns:     CharPtr - A pointer into the source where the name or
//              extension begin, respectively.  After the '/' or '.'.
Char *FileName ( const Char *filename ) {
  Char *p = StrRChr (filename, '/');
  return ( p ? (Char *)(p + 1) : (Char *)filename );
}
Char *Extension ( const Char *filename ) {
  Char *p = StrRChr (filename, '.');
  return ( p ? (Char *)(p + 1) : (Char *)filename );
}


// Function:    isPalmDB
// Descriptoin: Is the file a Palm database?
// Arguments:   CharPtr fname - Full path, or relative path to the file.
// Returns:     Boolean -  True - The file should be a Palm database.
//                        False - The file is not a Palm database.
// Comments:    This should be updated to do more than just check the file
//              extension.  Read in the database header and check for sane
//              values?
UInt8 isPalmDB ( Char *fname ) {
  Char *ext = MemPtrNew(StrLen(fname)+1);
  ext = Extension(fname);
  if ( !StrCaselessCompare ( ext, "pdb" ) )
    return PalmDatabase;
  else if ( !StrCaselessCompare ( ext, "prc" ) )
    return PalmResourceDatabase;
  else if ( !StrCaselessCompare ( ext, "pqa" ) )
    return PalmQueryDatabase;
  return false;
}


void showDebugInfo(const char *file, UInt32 lineNo, char *msg) {
  Char lineStr[11];
  EventType event;
  UInt16 sel;

  StrIToA(lineStr, lineNo);
  sel = FrmCustomAlert (DEBUG_ALERT, file, lineStr, msg);
  switch (sel) {
  case 0:
    return;
  case 1:
    // clean things up and exit
    //    deleteItemList();
    event.eType = appStopEvent;
    EvtAddEventToQueue(&event);
    return;
  case 2:
    // soft reset
    SysReset();
  }
}


// Function:    isDirectory
// Description: Check to see if the given path is a directory.
// Arguments:   CharPtr - The path to the supposed directory.
// Returns:     Boolean -  True - The path is a directory.
//                        False - The path is not a directory.
Boolean isDirectory ( UInt16 volume, Char *path ) {
  FileRef fref;
  UInt32  attribs;

  if ( !StrCompare(path,"/") )
    return true;

  if ( VFSFileOpen ( volume, path, vfsModeRead, &fref ) )
    return false;
  VFSFileGetAttributes ( fref, &attribs );
  VFSFileClose ( fref );   // Givan path is a directory if bit set...

  if ( attribs == vfsFileAttrDirectory )
    return true;   // Attribute not set, file is not directory.

  return false;
}

// Name:        FourCharInt
// Description: Take a standard Palm/MacOS UInt32 and turn it into a string.
// Arguments:   value - the UInt32 to convert
//              buffer - a generic buffer string (length >= 5)
// Returns:     A pointer to the buffer passed to it.
// Example:     WinDrawChars ( FourCharInt(dbID, buffy), 5, 20, 20 );
// Revision:    13/11/2001 - First Created
Char *FourCharInt ( UInt32 value, Char *buffer ) {
  Char *ptr;

  ptr = (Char *)&value;
  StrPrintF ( buffer, "%c%c%c%c\0", ptr[0], ptr[1], ptr[2], ptr[3] );

  return buffer;
}


// Name:  SetFieldTextFromStrN
// Description: Set the contents of an off screen field.  The PalmOS way.
// Arguments: formP - the form the field is on
//    fieldID - the Resource ID of the field
//    strP - the new string for the field
// Returns:  A pointer to the field
// Comments: Modified from the PalmOS original to handle off-screen fields, as
//    well as fix many POSE complaints (direct access of Form memory..)
// Revision: 5/11/2001 - First Created
FieldPtr SetFieldTextFromStrN( FormPtr formP, UInt16 fieldID, char *strP ) {
  MemHandle  oldTxtH;  // handle to old text
  MemHandle  txtH;   // temporary text handle
  MemPtr   txtP;   // temporary text pointer

  FieldPtr  fldP;   // field pointer
  FieldAttrType fieldAttributes; // added to stop POSE from bitching
  Boolean   fieldWasEditable; // same as original, just beaten around

  // get some space in which to stash the string.
  txtH = MemHandleNew (  StrLen( (Char *)strP ) + 1 );
  txtP = MemHandleLock ( txtH );

  // copy the string.
  StrCopy ( (char *)txtP, (char *)strP );

  // unlock the string handle.
  MemHandleUnlock ( txtH );
  txtP = 0;

  // get the field and the field's current text handle.
  fldP = (FieldPtr)getObjectPtr ( formP, fieldID );
  oldTxtH = FldGetTextHandle ( fldP );

  // set the field's text to the new text.
  FldGetAttributes ( fldP, &fieldAttributes );
  fieldWasEditable = fieldAttributes.editable;
  fieldAttributes.editable = true;
  FldSetAttributes ( fldP, &fieldAttributes );

  FldSetTextHandle( fldP, txtH );

  fieldAttributes.editable = fieldWasEditable;
  FldSetAttributes ( fldP, &fieldAttributes );

  // free the handle AFTER we call FldSetTextHandle().
  if ( oldTxtH != NULL )
    MemHandleFree( oldTxtH );

  return fldP;
} // SetFieldTextFromStr
