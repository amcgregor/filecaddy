#ifndef _RESOURCE_H_
#define _RESOURCE_H_

// $Id: Resource.h,v 1.7 2002/08/25 13:09:41 amcgregor Exp $

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

// $Log: Resource.h,v $
// Revision 1.7  2002/08/25 13:09:41  amcgregor
// Bugfixes (closing files,) "Home" button, and Version right column added.
//
// Revision 1.6  2002/07/19 18:58:41  amcgregor
// Enhanced Volume Information dialog
//
// Revision 1.5  2002/07/18 07:55:48  amcgregor
// Added release notes dialog, fixed crash w/ no VFS
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

#define MAIN_MENU                 2000
#define VOLUME_MENU               1000
#define VOLUME_CREATE_DIR         1000
#define VOLUME_REFRESH            1001
#define VOLUME_FIND_FILES         1002
#define VOLUME_INFO               1003
#define OPTIONS_MENU              1005
#define OPTIONS_PREFS             1006
#define OPTIONS_SORT              1007
#define HELP_MENU                 1010
#define HELP_RELEASE              1011
#define HELP_ABOUT                1012

#define DONE_ALERT                1000
#define DEBUG_ALERT               1001
#define DELETE_ALERT              1002
#define DEL_DIR_NOT_EMPTY         1003
#define ALERT_FULL_CARD           1004
#define ALERT_FULL_RAM            1005
#define ALERT_DIR_FULL            1006
#define ALERT_BATTERY             1007
#define ALERT_UPDATE_REMINDER     1008
#define OLD_VERSION_ALERT         1009
#define READONLY_ALERT            1010
#define NOCF_ALERT                1011
 #define NOIMPLEMENT_ALERT         1012
 #define FILE_DELETE_ALERT         1013
 #define NO_DEL_ROOT               1014

#define MAIN_FORM                 1000
#define MAIN_FORM_LEFT            1001
#define MAIN_TABLE                1002
#define DIRECTORY_FLD             1700
#define MAIN_SCROLLBAR            1200
#define MAIN_COPY_BTN             1300
#define MAIN_DEL_BTN              1301
#define MAIN_CARD_LIST            1302
#define MAIN_CARD_POPUP           1303
#define MAIN_INFO_BTN             1304
#define MAIN_DIR_BTN              1306
#define MAIN_SORT_LABEL           1307
#define MAIN_SORT_POPUP           1308
#define MAIN_SORT_LIST            1309
#define MAIN_INFO_POPUP           1310
#define MAIN_INFO_LIST            1311
#define MAIN_PLUGIN_LIST          1312
#define MAIN_MOVE_BTN             1313
#define MAIN_SELECT_BTN           1314
#define MAIN_SELECT_LIST          1315
#define MAIN_NAMESORT_BTN         1316
#define MAIN_GOHOME               1317

#define HOME_BITMAP               2400
#define FOLDER_BITMAP             2401
#define FILE_BITMAP               2402
#define TEXTFILE_BITMAP           2403

#define MAIN_NO_CF                2900
#define NOCF_VERSION              2901
#define NOCF_CFFAT                2902
#define NOCF_REG_NUM              2903
#define NOCF_REG_ID               2904

#define SETTINGS_FORM             3000
#define APPEARANCE_POPUP          3001
#define APPEARANCE_LIST           3002
#define CF_SPEED_POPUP            3003
#define CF_SPEED_LIST             3004
#define SETTINGS_SAVE_BTN         3005
#define SETTINGS_CANCEL_BTN       3006
#define SET_START_SORT_CB         3008
#define SET_SHOW_ICONS_CB         3009
#define SET_BOLD_DIRECTORIES_CB   3010
#define SET_SHOW_ROM_CB           3011
#define SET_USE_COLOR_CB          3012
#define SET_SHOW_HIDDEN_CB        3013
#define SET_DIRECTORIES_FIRST_CB  3014
#define SET_PALM_ONLY_CB          3015
#define SET_REVERSE_SORT_CB       3016

#define ERRORLOG_FORM             4000
#define ERRORLOG_TEXT             4001
#define ERRORLOG_DONE_BTN         4002

#define TEXT_FORM                 4100
#define TEXTFORM_BOX              4101
#define TEXTFORM_SCROLLBAR        4102
#define TEXTFORM_DONE             4103

#define DIRECTORY_FORM            5000
#define DIRECTORY_LIST            5001
#define DIR_SAVE_BTN              5002
#define DIR_CANCEL_BTN            5003
#define DIR_DOWN_BTN              5004
#define DIR_UP_BTN                5005
#define DIR_NEW_BTN               5006
#define DIR_DEL_BTN               5007
#define DIR_CURRENT               5008

#define NEWDIR_FORM               6000
#define NEWDIR_LABEL              6001
#define NEWDIR_NAME               6002
#define NEWDIR_OK_BTN             6003
#define NEWDIR_CANCEL_BTN         6004

#define INFO_FORM                 7000
#define INFO_HIDDEN               7001
#define INFO_LABEL                7002
#define INFO_FS                   7003
#define INFO_NUM                  7004
#define INFO_MEDIA                7005
#define INFO_RAMLABEL             7006
#define INFO_MANUFAC              7007
#define INFO_RAMNUM               7008
#define INFO_FORMAT               7009
#define INFO_OK                   7010
#define INFO_CANCEL               7011

#define ABOUT_FORM                7100
#define ABOUT_VERSION             7101

#endif
