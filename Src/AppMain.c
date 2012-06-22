// $Id: AppMain.c,v 1.11 2002/10/01 15:05:58 amcgregor Exp $

// Author:     Alice Bevan-McGregor <alice@gothcandy.com>
// Creator:    David Kessler
// Descripton: The application main function.

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

// $Log: AppMain.c,v $
// Revision 1.11  2002/10/01 15:05:58  amcgregor
// Fixed some minor bugs
//
// Revision 1.10  2002/09/24 04:38:19  amcgregor
// Updates to the plugin source, makefiles.
//
// Revision 1.9  2002/08/25 13:09:41  amcgregor
// Bugfixes (closing files,) "Home" button, and Version right column added.
//
// Revision 1.8  2002/07/25 15:09:00  amcgregor
// Second thought: tabs bad.
//
// Revision 1.7  2002/07/25 11:51:53  amcgregor
// Code run through: astyle --style=kr --indent=tab=2 -S *.c *.h
//
// Revision 1.6  2002/07/24 16:20:55  amcgregor
// Prerelease 3, major bugfixes.
//
// Revision 1.5  2002/07/19 03:26:58  amcgregor
// Fixed first major bug, added release notes.
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
//#include <CallbackFix.h>    // might want this... oops!
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
TPreferences        Preferences;
Boolean             CardEnabled = false,
                    HiResEnabled = false;
UInt16              HRLibRef;
UInt32              ScreenWidth = 160,
                    ScreenHeight = 160;
UInt32              PalmOSVersion;
Char                DebugMessage[128];

UInt16              plugInCount_g = 0;
MemHandle           plugInArrayH_g = NULL;

#define DEBUGINFO(m) showDebugInfo(__FILE__, __LINE__, m)
void showDebugInfo(const char *file, UInt32 lineNo, char *msg);
Boolean mainFormEventHandler ( EventPtr eventP );
void deleteItemList(void);

extern void Plugin_Search ( void );

static void errorLogFormInit(FormPtr frmP) {
#if 0
  UInt16 numErr;
  //  ErrLogEntry_t *log;
  UInt16 i;

  if (numErr > 0) {
    errStrP_g = (Char *)MemPtrNew(20 * numErr);
    for (i = 0; i < numErr; i++) {
      //      StrPrintF(temp, "%d: %lx\n", log[i].line, (UInt32)(log[i].err));
      //      StrCat(errStrP_g, temp);
    }
  } else {
    //    errStrP_g = MemPtrNew(20);
    //    StrCopy(errStrP_g, "No errors");
  }

  // copy to the text field
  //  FldSetTextPtr(getObjectPtr(frmP, ERRORLOG_TEXT), errStrP_g);
  //  FldRecalculateField(getObjectPtr(frmP, ERRORLOG_TEXT), true);
#endif
}


Boolean errorLogFormEventHandler(EventPtr eventP) {
  Boolean handled = false;
  FormPtr frmP = FrmGetActiveForm();
  switch (eventP->eType) {

  case frmOpenEvent:
    FrmDrawForm(frmP);
    errorLogFormInit(frmP);
    handled = true;
    break;

  case ctlSelectEvent: {
      switch (eventP->data.ctlSelect.controlID) {
      case ERRORLOG_DONE_BTN:
        // if (errStrP_g) {
        //   MemPtrFree(errStrP_g);
        //   errStrP_g = NULL;
        // }
        FrmReturnToForm(mainFormID);
        handled = true;
        break;
      }
    }
    break;
  default:
    break;
  }
  return handled;
}

void startApp ( void ) {
  UInt16  prefSize = sizeof(TPreferences);
  Err     status, error;
  UInt32  resolution;
  RectangleType waitbox = { { 80-40, 80-(4+12+4)/2 }, { 80, 4+12+4 } };
  FontID oldFont;
#ifndef NOHIRES
  SonySysFtrSysInfoP infoP;       // sony hi-res info
#endif

  error = FtrGet ( sysFtrCreator, sysFtrNumROMVersion, &PalmOSVersion );

  WinDrawRectangleFrame ( dialogFrame, &waitbox );
  oldFont = FntSetFont ( boldFont );
  WinDrawChars ( "Starting up...", 14, 80 - ( FntCharsWidth("Starting up...", 14) / 2 ), 80-(4+12+4)/2+4 );
  FntSetFont ( oldFont );

  // The entirety of the following will be relplaced once PalmOS 5 is released to the wilds.
  // Until then, Sony, here we come!
#ifndef NOHIRES
  if ( ( error = FtrGet ( sonySysFtrCreator, sonySysFtrNumSysInfoP, (UInt32 *)&infoP ) ) ) {
    // No high-resolution support.
    HiResEnabled = false;
  } else {
    if ( infoP->libr & sonySysFtrSysInfoLibrHR ) {
      // High-resolution available!
      if ( ( error = SysLibFind ( sonySysLibNameHR, &HRLibRef ) ) ) {
        if ( error == sysErrLibNotFound ) {
          // Not loaded, so we do it ourselves.
          error = SysLibLoad ( 'libr', sonySysFileCHRLib, &HRLibRef );
        }
      }
      
      if ( !error ) {
        // Now we can play...
        HiResEnabled = true;
        
        error = HROpen ( HRLibRef );
        if ( error ) {
          // Handle Error
          DEBUGINFO("Unable to HROpen!");
        } else {
          ScreenWidth = hrWidth;
          ScreenHeight = hrHeight;
          error = HRWinScreenMode ( HRLibRef, winScreenModeSet,
              &ScreenWidth, &ScreenHeight, NULL, NULL );
          if ( error != errNone ) {
            // screen unchanged!
            ScreenWidth = 160;
            ScreenHeight = 160;
            HiResEnabled = false;
          } else {
            // high-resolution!
            
          }
        }
      }
    }
  }
#endif

  status = PrefGetAppPreferences ( AppCreator, 1000, &Preferences,
                                   &prefSize, false );

  if ((status == noPreferenceFound) ||
      (prefSize != sizeof(TPreferences)) ) { // ||
    //(Preferences.Version != FM_VERSION)) {

    //default everything to 0 just to be safe
    MemSet ( &Preferences, sizeof(TPreferences), 0 );

    // default initialization, since discovered Prefs was missing or old.
    Preferences.Version                   = FM_VERSION;

    Preferences.General.LeftHanded        = false;
    Preferences.General.InstallTS         = TimGetSeconds();
    Preferences.General.UpdateReminder    = 30;   // days

    Preferences.Volume.Number             = 0;    // default to internal view
    StrCopy ( Preferences.Volume.Path, "/" );     // default to CF root

    Preferences.Display.ShowIcons         = true;
    Preferences.Display.BoldDirectories   = true;
    Preferences.Display.ShowROM           = false;
    Preferences.Display.UseColor          = false;
    Preferences.Display.ShowHidden        = true;
    Preferences.Display.DirectoriesFirst  = true;
    Preferences.Display.PalmOnly          = false;
    Preferences.Display.StartSorted       = true;
    Preferences.Display.ReverseSort       = false;
    Preferences.Display.Column            = colSize;
    Preferences.Display.Sort              = sortByName;
    
    Preferences.HomeIsRoot                = true;
    StrCopy ( Preferences.HomePath, "/" );

    // save changed preferences
    PrefSetAppPreferences(AppCreator, 1000, 1, &Preferences,
                          sizeof(TPreferences), false);

    // Display welcome dialog... information on how to change options and
    // website notice.  Also, give an option to look at the GPL...
    // Or, display release notes for this version... it has potential...
    FrmInformation ( 0, 9000, "Release Notes" );
  }

#if defined(UPGRADE_REMINDER)
  // add option to never be asked again... :D
  if ( TimGetSeconds() > Preferences.General.InstallTS +
       ( Preferences.General.UpdateReminder * 86400L ) ) {
    // remind user to check for updates
    FrmAlert(ALERT_UPDATE_REMINDER);
    Preferences.General.UpdateReminder = Preferences.General.UpdateReminder * 2;
  }
#endif

  if ( !CurrentDirectory ) {
    CurrentDirectory = MemPtrNew ( StrLen(Preferences.Volume.Path) );
    StrCopy ( CurrentDirectory, Preferences.Volume.Path );
  }

  if ( !Preferences.Display.StartSorted ) {
    Preferences.Display.Sort = sortNone;
  }

  if ( Preferences.Display.UseColor && !HiResEnabled ) {
    WinScreenMode ( winScreenModeGetSupportedDepths, NULL, NULL, &resolution, NULL );
    if ( resolution & 0x80 ) resolution = 8;
    else if ( resolution & 0x8 ) resolution = 4;
    else if ( resolution & 0x4 ) resolution = 2;
    WinScreenMode ( winScreenModeSet, NULL, NULL, &resolution, NULL );
  }

  if ( !CardEnabled ) Preferences.Volume.Number = 0;

  if ( !CardEnabled || ( !isDirectory ( Preferences.Volume.Number, Preferences.Volume.Path ) ) ) {
    Preferences.Volume.Number = 0;
    StrCopy ( Preferences.Volume.Path, "/" );
  }

  Cache_Init();
  Plugin_Search();
}

void stopApp ( void ) {
  Err error;

  // save the current preferences
  PrefSetAppPreferences(AppCreator, 1000, 1, &Preferences,
                        sizeof(TPreferences), false);

  deleteItemList();
  Cache_Delete();

  //MemHandleUnlock(FolderIconH);
  //MemHandleUnlock(FileIconH);
  //MemHandleUnlock(TextFileIconH);
  //DmReleaseResource(FolderIconH);
  //DmReleaseResource(FileIconH);
  //DmReleaseResource(TextFileIconH);

#ifndef NOHIRES
  if ( HiResEnabled ) {
    error = HRWinScreenMode ( HRLibRef, winScreenModeSetToDefaults,
        NULL, NULL, NULL, NULL );
    if ( error != errNone ) {
      // screen unchanged
    } else {
      // switched back
    }
    error = HRClose ( HRLibRef );
  }
#endif

  if ( Preferences.Display.UseColor && !HiResEnabled )
    WinScreenMode ( winScreenModeSetToDefaults, NULL, NULL, NULL, NULL );

  // Free all plugins here...
}

Boolean appHandleEvent ( EventPtr event ) {
  FormPtr frm;
  Int16  formId;
  Boolean handled = false;

  if ( event->eType == frmLoadEvent ) {
    formId = event->data.frmLoad.formID;
    frm = FrmInitForm ( formId );
    FrmSetActiveForm ( frm );
    switch ( formId ) {
    case MAIN_FORM:
      mainFormID = MAIN_FORM;
      FrmSetEventHandler ( frm, mainFormEventHandler );
      break;
    case MAIN_FORM_LEFT:
      mainFormID = MAIN_FORM_LEFT;
      FrmSetEventHandler ( frm, mainFormEventHandler );
      break;
    case ERRORLOG_FORM:
      FrmSetEventHandler ( frm, errorLogFormEventHandler );
      break;
    }
    handled = true;
  }
  return handled;
}

static Boolean isLatestVersion ( void ) {
  UInt16            cardNo;
  LocalID           dbID;
  UInt16            version;
  Char              nameStr[33];
  Char              locStr[17];
  DmSearchStateType stateInfo;
  Err               err;
  UInt16            selection;

  // check if this is newest FileCaddy
  err = DmGetNextDatabaseByTypeCreator ( true, &stateInfo, 'appl', AppCreator,
                                         true, &cardNo, &dbID );
  if ( !err ) {
    err = DmDatabaseInfo ( cardNo, dbID, nameStr, 0, &version, 0, 0, 0, 0, 0,
                           0, 0, 0 );
    if ( !err ) {
      if ( version > BUILD_NUMBER ) {
        if ( cardNo ) {
          StrCopy ( locStr, "on the module" );
        } else {
          StrCopy ( locStr, "in RAM" );
        }
        selection = FrmCustomAlert ( OLD_VERSION_ALERT, nameStr, locStr, " " );
        if ( !selection ) {
          return false;
        }
      }
    }
  }

  return true;
}

Boolean CardEnabledDevice ( void ) {
  Err    err;
  UInt32 vfsMgrVersion;

  err = FtrGet ( sysFileCVFSMgr, vfsFtrIDVersion, &vfsMgrVersion );
  if ( err ) return false;

  return true;
}

UInt32 PilotMain(UInt16 cmd, void *cmdPBP, UInt16 launchFlags) {
  EventType event;
  Err error;

  if (cmd == sysAppLaunchCmdNormalLaunch) {
    if (!isLatestVersion) {
      return 0;
    }

    CardEnabled = CardEnabledDevice();
    startApp();

    if ( Preferences.General.LeftHanded ) {
      FrmGotoForm ( MAIN_FORM_LEFT );
    } else {
      FrmGotoForm ( MAIN_FORM );
    }

    do {
      EvtGetEvent(&event, evtWaitForever); // no reason to wait 30, is there?
      if ( !SysHandleEvent ( &event ) )
        if ( !MenuHandleEvent ( 0, &event, &error ) )
          if ( !appHandleEvent ( &event ) )
            FrmDispatchEvent ( &event );
    } while ( event.eType != appStopEvent );

    stopApp();

    FrmCloseAllForms();
  }

  return 0;
}
