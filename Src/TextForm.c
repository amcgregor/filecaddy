// $Id: TextForm.c,v 1.3 2002/07/25 15:09:00 amcgregor Exp $

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

// $Log: TextForm.c,v $
// Revision 1.3  2002/07/25 15:09:00  amcgregor
// Second thought: tabs bad.
//
// Revision 1.2  2002/07/25 11:51:53  amcgregor
// Code run through: astyle --style=kr --indent=tab=2 -S *.c *.h
//
// Revision 1.1  2002/07/18 07:55:48  amcgregor
// Added release notes dialog, fixed crash w/ no VFS
//

#include <PalmOS.h>
#include "Resource.h"

#define DEBUGINFO(m) showDebugInfo(__FILE__, __LINE__, m)
void showDebugInfo(const char *file, UInt32 lineNo, char *msg);

Char temp[64];

static void TextFormUpdateScrollBar ( FormPtr formP ) {
  UInt16 scrollPos;
  UInt16 textHeight;
  UInt16 fieldHeight;
  Int16 maxValue;
  FieldPtr fld;
  ScrollBarPtr bar;

  fld = FrmGetObjectPtr ( formP, FrmGetObjectIndex ( formP, TEXTFORM_BOX ) );
  bar = FrmGetObjectPtr ( formP, FrmGetObjectIndex ( formP, TEXTFORM_SCROLLBAR ) );

  FldGetScrollValues (fld, &scrollPos, &textHeight,  &fieldHeight);

  if (textHeight > fieldHeight) {
    // On occasion, such as after deleting a multi-line selection of text,
    // the display might be the last few lines of a field followed by some
    // blank lines.  To keep the current position in place and allow the user
    // to "gracefully" scroll out of the blank area, the number of blank lines
    // visible needs to be added to max value.  Otherwise the scroll position
    // may be greater than maxValue, get pinned to maxvalue in SclSetScrollBar
    // resulting in the scroll bar and the display being out of sync.
    maxValue = (textHeight - fieldHeight) + FldGetNumberOfBlankLines (fld);

  } else if (scrollPos)
    maxValue = scrollPos;
  else
    maxValue = 0;

  SclSetScrollBar (bar, scrollPos, 0, maxValue, fieldHeight-1);
}

static void TextFormScroll ( Int16 linesToScroll, Boolean updateScrollbar ) {
  UInt16   blankLines;
  FieldPtr   fld;
  FormPtr       formP = FrmGetActiveForm();

  fld = FrmGetObjectPtr ( formP, FrmGetObjectIndex ( formP, TEXTFORM_BOX ) );
  blankLines = FldGetNumberOfBlankLines (fld);

  if (linesToScroll < 0)
    FldScrollField (fld, -linesToScroll, winUp);
  else if (linesToScroll > 0)
    FldScrollField (fld, linesToScroll, winDown);

  // If there were blank lines visible at the end of the field
  // then we need to update the scroll bar.
  if ( blankLines && ( linesToScroll < 0 ) || updateScrollbar) {
    TextFormUpdateScrollBar ( formP );
  }
}

static void TextFormPageScroll ( WinDirectionType direction ) {
  UInt16        linesToScroll;
  FormPtr       formP;
  FieldPtr      fldP;

  formP = FrmGetActiveForm();
  fldP = FrmGetObjectPtr ( formP, FrmGetObjectIndex ( formP, TEXTFORM_BOX ) );

  if ( FldScrollable ( fldP, direction ) ) {
    linesToScroll = FldGetVisibleLines(fldP);

    if ( direction == winUp )
      linesToScroll = -linesToScroll;

    TextFormScroll ( linesToScroll, true );

    return;
  }

  SndPlaySystemSound (sndInfo);
}


static Boolean TextFormHandleEvent ( EventType * event ) {
  Boolean       handled = false;
  FormPtr       formP;
  FieldPtr      fldP;

  formP = FrmGetActiveForm();

  if ( event->eType == keyDownEvent ) {
    if ( EvtKeydownIsVirtual ( event ) ) {
      if ( event->data.keyDown.chr == vchrPageUp ) {
        TextFormPageScroll ( winUp );
        handled = true;

      } else if ( event->data.keyDown.chr == vchrPageDown ) {
        TextFormPageScroll ( winDown );
        handled = true;
      }
    }

  } else if (event->eType == menuCmdBarOpenEvent) {
    fldP = FrmGetObjectPtr ( formP, FrmGetObjectIndex ( formP, TEXTFORM_BOX ) );
    FldHandleEvent ( fldP, event );

    // Beam on the left
    //MenuCmdBarAddButton(menuCmdBarOnLeft, BarBeamBitmap, menuCmdBarResultMenuItem, BeamMemoCmd, 0);
    // Delete on the Right
    //MenuCmdBarAddButton(menuCmdBarOnRight, BarDeleteBitmap, menuCmdBarResultMenuItem, DeleteMemoCmd, 0);

    // Prevent the field package to automatically add cut, copy, paste, and undo buttons as applicable
    // since it was done previously
    event->data.menuCmdBarOpen.preventFieldButtons = true;

  } else if ( event->eType == sclRepeatEvent ) {
    TextFormScroll (event->data.sclRepeat.newValue - event->data.sclRepeat.value, false);
  }

  return ( handled );
}


void FrmInformation ( Int16 font, UInt16 msgID, Char *title ) {
  MemHandle     messageStr;
  FormType     *formP, *oldFormP;
  FieldType    *fldP;
  Boolean       done = false;
  EventType     event;

  MenuEraseStatus( 0 );             // having a menu or command bar open is bad mojo
  InsPtEnable ( false );            // don't want a blinky cursor

  oldFormP = FrmGetActiveForm();

  formP = FrmInitForm ( TEXT_FORM );
  FrmCopyTitle ( formP, title );

  messageStr = DmGetResource ( strRsc, msgID );
  fldP = FrmGetObjectPtr ( formP, FrmGetObjectIndex ( formP, TEXTFORM_BOX ) );
  FldSetTextHandle ( fldP, messageStr );
  FldSetFont ( fldP, font );

  FrmSetActiveForm ( formP );
  FrmDrawForm ( formP );

  TextFormUpdateScrollBar ( formP );

  while ( !done ) {
    EvtGetEvent ( &event, evtWaitForever );
    if ( !SysHandleEvent ( &event ) )
      FrmHandleEvent ( formP, &event );

    TextFormHandleEvent ( &event );

    if ( event.eType == ctlSelectEvent ) {
      if ( event.data.ctlSelect.controlID == TEXTFORM_DONE )
        done = true;

    } else if ( event.eType == appStopEvent ) {
      EvtAddEventToQueue ( &event );
      break;
    }
  }

  FldSetTextHandle ( fldP, 0 );
  FrmEraseForm ( formP );
  FrmDeleteForm ( formP );

  if ( oldFormP )
    FrmSetActiveForm ( oldFormP );
}
