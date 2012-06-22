#ifndef _Utility_h_
#define _Utility_h_

// $Id: Utility.h,v 1.5 2002/07/19 18:58:41 amcgregor Exp $

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

// $Log: Utility.h,v $
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

#ifndef MIN
#define MIN( a, b ) ( (a) < (b) ? (a) : (b) )
#endif

#ifndef MAX
#define MAX( a, b) ( (a) > (b) ? (a) : (b) )
#endif

#ifndef ABS
#define ABS(a) ( (a) < 0 ? -(a) : (a) )
#endif

#ifndef min
#define min MIN
#endif

#ifndef max
#define max MAX
#endif

#ifndef abs
#define abs ABS
#endif

inline void *getObjectPtr ( FormPtr frmP, Int16 index );

Boolean checkBattery ( void );

Char *printNiceSize ( UInt32 size, Char *dest, UInt8 accuracy );
Char *FourCharInt ( UInt32 value, Char *buffer );
inline Char *StrRChr ( const Char *str, const Char chr );
Char *FileName ( const Char *filename );
Char *Extension ( const Char *filename );

Boolean isROMDB ( UInt16 card, LocalID lid );
Boolean isPalmDB ( Char *fname );
Boolean isDirectory ( UInt16 volume, Char *path );

void showDebugInfo(const char *file, UInt32 lineNo, char *msg);

FieldPtr SetFieldTextFromStrN( FormPtr formP, UInt16 fieldID, char *strP );

#endif // _Utility_h_
