#include <PalmOS.h>
#include "Globals.h"

// $Id: Progress.c,v 1.5 2002/07/25 15:09:00 amcgregor Exp $

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

// $Log: Progress.c,v $
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

WinHandle _winProgress = 0;
WinHandle _winOld = 0;
WinHandle _winSaved = 0;
UInt32    _startTicks = 0;

// Function:    drawProgressWindow
// Description: Draw a dialog indicating a busy state, or progress.
// Arguments:   WinHandlePtr progwin - Storage pointer for the progress
//                                     dialog.
//              WinHandlePtr oldwin  - Storage pointer for the previous
//                                     dialog.
//              WinHandlePtr savewin - Storage pointer for the image under
//                                     the progress dialog.
//              CharPtr      tag     - The prefix text (i.e. "Copying: ")
//              CharPtr      name    - The file name (i.e. "hello.txt")
//                                     These get combined to produce:
//                                      "Copying: hello.txt"...
// Returns:     None
void drawProgressWindow ( WinHandle *progwin, WinHandle *oldwin,
                          WinHandle *savewin, Char *name ) {
  Err err;
  RectangleType savebox = { {8, 38}, {144, 38} };
  RectangleType winbox = { {10, 40}, {140, 34} };
  RectangleType barbox = { {10, 19}, {120, 8} };
  FontID oldfont;

  *savewin = WinSaveBits(&savebox, &err);
  if (*savewin) {
    *progwin = WinCreateWindow(&winbox, dialogFrame, true, false, &err);
    if (*progwin) {
      *oldwin = WinGetDrawWindow();
      WinSetDrawWindow(*progwin);
      WinEraseWindow();
      WinDrawWindowFrame();
      oldfont = FntGetFont();
      FntSetFont(stdFont);
      WinDrawTruncChars(name, StrLen(name), 5, 3, 120);
      FntSetFont(oldfont);
      WinDrawRectangleFrame(simpleFrame, &barbox);
    }
  }
}


// Function:    deleteProgressWindow
// Description: Deletes the progress information and restores the screen.
// Arguments:   WinHandle progwin - The progress window information.
//              WinHandle oldwin  - The old screen information.
//              WinHandle savewin - The saved area of screen bitmap.
// Returns:     None
void deleteProgressWindow ( WinHandle progwin, WinHandle oldwin,
                            WinHandle savewin ) {
  if (progwin) {
    WinDeleteWindow(progwin, true);
    WinSetDrawWindow(oldwin);
  }
  if (savewin)
    WinRestoreBits(savewin, 8, 38);
}


// Function:    drawProgressBar
// Description: Draws a progress bar onto the progress window.
// Arguments:   UInt8 percent - % of the bar to display.
// Returns:     None
void drawProgressBar ( UInt8 percent ) {
  RectangleType barbox = { {10, 19}, {0, 8} };
  UInt32 size;

  size = (120 * (UInt32)percent) / 100;
  barbox.extent.x = size;
  WinDrawRectangle(&barbox, 0);
}


// Function:    drawPopupWindow
// Description: Displays a pop-up window for a short length of time.
// Arguments:   WinHandlePtr progwin - Popup window pointer.
//              WinHandlePtr oldwin  - Old window pointer.
//              WinHandlePtr savewin - Old window bitmap.
//              CharPtr      msg     - The message to display.
// Returns:     None
void drawPopupWindow ( WinHandle *progwin, WinHandle *oldwin,
                       WinHandle *savewin, Char *msg ) {
  Err err;
  RectangleType savebox = { {8, 38}, {144, 38} };
  RectangleType winbox = { {10, 40}, {140, 34} };
  FontID oldfont;

  *savewin = WinSaveBits(&savebox, &err);
  if (*savewin) {
    *progwin = WinCreateWindow(&winbox, dialogFrame, true, false, &err);
    if (*progwin) {
      *oldwin = WinGetDrawWindow();
      WinSetDrawWindow(*progwin);
      WinEraseWindow();
      WinDrawWindowFrame();
      oldfont = FntGetFont();
      FntSetFont(stdFont);
      WinDrawTruncChars(msg, StrLen(msg), 5, 3, 120);
      FntSetFont(oldfont);
    }
  }
}

// Function:    deletePopupWindow
// Description: Deletes the popup information and restores the screen.
// Arguments:   WinHandle progwin - The popup window information.
//              WinHandle oldwin  - The old screen information.
//              WinHandle savewin - The saved area of screen bitmap.
// Returns:     None
// Comments:    Not needed - deleteProgressWindow does the exact same.
//void deletePopupWindow ( WinHandle progwin, WinHandle oldwin,
//                         WinHandle savewin ) {
//  if (progwin) {
//    WinDeleteWindow(progwin, true);
//    WinSetDrawWindow(oldwin);
//  }
//  if (savewin) WinRestoreBits(savewin, 8, 38);
//}


// Set the timer start time.
void StartTimer ( void ) {
  _startTicks = TimGetTicks();
}

// Return elapsed time.
UInt32 StopTimer ( void ) {
  UInt32    stopTicks;
  stopTicks = TimGetTicks();
  if ( stopTicks > _startTicks )
    return ( stopTicks - _startTicks );
  else
    return ( (0xffffffff - _startTicks) + stopTicks );
}

// Create and display a progress dialog with a title.
void WinDrawProgress ( const Char *text ) {
#ifndef _DEBUG_
  Err       err;
  RectangleType winbox = { {10,40}, {140,34+28} };
  RectangleType savebox = { {winbox.topLeft.x-2,winbox.topLeft.y-2},
                            {winbox.extent.x+4,winbox.extent.y+4} };
  RectangleType barbox = { {10,19}, {120,8} };
  RectangleType delbox = { {5, 3}, {130,24} };
  FontID    oldfont;
  Boolean   refresh = false;

  if ( _winProgress )
    refresh = true;

  if ( !refresh )
    _winSaved = WinSaveBits (&savebox, &err );
  if ( !refresh )
    _winProgress = WinCreateWindow ( &winbox, dialogFrame, true, false, &err );

  if ( _winProgress ) {
    if ( !refresh )
      _winOld = WinGetDrawWindow();
    if ( !refresh )
      WinSetDrawWindow(_winProgress);
    if ( !refresh )
      WinEraseWindow();
    WinDrawWindowFrame();
    oldfont = FntGetFont();
    FntSetFont(stdFont);
    if ( refresh )
      WinEraseRectangle ( &delbox, 0 );
    WinDrawTruncChars(text, StrLen(text), 5, 3, 130);
    FntSetFont(oldfont);
    WinDrawRectangleFrame(simpleFrame, &barbox);
    if ( !refresh )
      barbox.topLeft.y += 15;
    if ( !refresh )
      WinDrawRectangleFrame(simpleFrame, &barbox);
  }
#endif
}

void WinDeleteProgress ( void ) {
#ifndef _DEBUG_
  if ( _winProgress ) {
    WinDeleteWindow (_winProgress, true );
    WinSetDrawWindow ( _winOld );
    _winProgress = 0;
  }
  if ( _winSaved ) {
    WinRestoreBits ( _winSaved, 8, 38 );
    _winSaved = 0;
  }
#endif
}

void WinDrawProgressBar ( UInt16 percent, UInt16 file, Char *text ) {
#ifndef _DEBUG_
  static RectangleType  barbox1  = { {  10, 19 }, { 0, 8 } };
  static RectangleType  barbox2  = { {  10, 34 }, { 0, 8 } };
  static RectangleType  erasebox = { {   5, 20+14+6+6 },
                                     { 130, 20+6+6 } };
  static UInt32         size1, size2;

  size1 = ( 120 * (UInt32)percent ) / 100;
  barbox1.extent.x = size1;
  WinDrawRectangle ( &barbox1, 0 );
  size2 = ( 120 * (UInt32)file ) / 100;
  barbox2.extent.x = size2;
  WinDrawRectangle ( &barbox2, 0 );

  WinEraseRectangle ( &erasebox, 0 );
  WinDrawTruncChars ( text, StrLen(text), 5, barbox2.topLeft.y+barbox2.extent.y+5, 130 );
#endif
}

