// $Id: TextForm.h,v 1.2 2002/07/25 11:51:53 amcgregor Exp $

// Author:     Alice Bevan-McGregor <alice@gothcandy.com>
// Descripton: Display of a string resource w/ scrollbar.

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

// $Log: TextForm.h,v $
// Revision 1.2  2002/07/25 11:51:53  amcgregor
// Code run through: astyle --style=kr --indent=tab=2 -S *.c *.h
//
// Revision 1.1  2002/07/18 07:55:48  amcgregor
// Added release notes dialog, fixed crash w/ no VFS
//

#ifndef _TEXTFORM_H_
#define _TEXTFORM_H_

void FrmInformation ( Int16 font, UInt16 msgID, Char *title );

#endif
