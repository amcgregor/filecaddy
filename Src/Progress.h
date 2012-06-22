#ifndef _Progress_h_
#define _Progress_h_

// $Id: Progress.h,v 1.3 2002/07/16 00:59:43 amcgregor Exp $

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

// $Log: Progress.h,v $
// Revision 1.3  2002/07/16 00:59:43  amcgregor
// Initial public release, bugfixes, an aborted attempt at using the IconCache.
//
// Revision 1.2  2002/07/13 22:16:13  amcgregor
// Offline version to CVS miscelaneous updates.
//
// Revision 1.1  2002/07/11 14:46:55  root
// Initial revision
//

void drawProgressWindow ( WinHandle *progwin, WinHandle *oldwin,
                          WinHandle *savewin, Char *name );
void deleteProgressWindow ( WinHandle progwin, WinHandle oldwin,
                            WinHandle savewin );
void drawProgressBar ( UInt8 percent );

void drawPopupWindow ( WinHandle *progwin, WinHandle *oldwin,
                       WinHandle *savewin, Char *msg );
#define deletePopupWindow(x,y,z) deleteProgressWindow(x,y,z)

void StartTimer ( void );
UInt32 StopTimer ( void );

void WinDrawProgress ( const Char *text );
void WinDeleteProgress ( void );
void WinDrawProgressBar ( UInt16 percent, UInt16 file, Char *text );

#endif // _Progress_h_
