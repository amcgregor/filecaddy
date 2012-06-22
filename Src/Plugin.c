#include <PalmOS.h>
#include "Globals.h"
#include "Plugin.h"

// $Id: Plugin.c,v 1.10 2002/10/01 15:05:59 amcgregor Exp $

// Copyright (c) 2002, Alice Bevan-McGregor.

// $Log: Plugin.c,v $
// Revision 1.10  2002/10/01 15:05:59  amcgregor
// Fixed some minor bugs
//
// Revision 1.9  2002/09/24 04:38:19  amcgregor
// Updates to the plugin source, makefiles.
//

Plug_InternalRef     *PluginListP[24];

#define DEBUGINFO(m) showDebugInfo(__FILE__, __LINE__, m)
void showDebugInfo(const char *file, UInt32 lineNo, char *msg);

Plug_Main    *gPlugInResP;				// pointer to our (locked) plug-in resource
DmOpenRef     gPlugInDbRef;				// reference to plug-in database

void Plugin_Search ( void ) {
#if 0
	// The following code will locate our plug-in resource (which may be in our own
	// resource file or in a separate PRC file to be opened by type and creator as
	// demonstrated below)
	DmSearchStateType   searchState;
	UInt16              tempCardNo[32];
	MemHandle           tempPlugInResH;				// handle to our plug-in resource
	Boolean             newSearch = true;
	LocalID             tempPlugInDbID[32];
	Err                 theErr = 0;
  UInt8               i = 0;

  Plug_ParamBlock     plugInParamBlock;

	// The following code could be modified to iterate through
	// multiple (separate) plug-in database(s), if desired...
  do {
    theErr = DmGetNextDatabaseByTypeCreator(newSearch, &searchState, kPlugInFileType, 
        kPlugInFileCreator, false /* all versions */, &tempCardNo[i], &tempPlugInDbID[i]);
    newSearch = false;
    if ( !theErr ) i++;
  } while ( !theErr );

	if (theErr) { // no plug-in database found
		tempPlugInDbID = 0;
		gPlugInDbRef = 0;

	}	else {      // we found a plug-in database
		if (tempPlugInDbID) {				// open the plug-in database
			gPlugInDbRef = DmOpenDatabase(tempCardNo, tempPlugInDbID, dmModeReadOnly);
		}
	}

	// Get a handle to our plug-in resource
	tempPlugInResH = DmGetResource(kPlugInResourceType, kFirstPlugInResID);

	// Make sure we found it
	if (!tempPlugInResH)
		return 1;

	// Lock down the plug-in so it doesn't move
	gPlugInResP = (Plug_Main *) MemHandleLock(tempPlugInResH);
	
  plugInParamBlock.eType = cmdStartup;
  plugInParamBlock.hostVersion = plugVersion;
  (*gPlugInResP)(&plugInParamBlock);

#endif
	return 0;
}

#if 0
void Plugin_Search ( void ) {
	DmSearchStateType   searchState;
	UInt16              tempCardNo;
	MemHandle           tempPlugInResH;				// handle to our plug-in resource
	Boolean             newSearch = true;
	LocalID             tempPlugInDbID = 0;
	Err                 theErr = 0;
  UInt8               curPlug = 0;
  MemHandle           nameH;
  Char               *nameP;
  
  Plug_ParamBlock     plugInParamBlock;

  do {
    DEBUGINFO("Top of loop.");
    theErr = DmGetNextDatabaseByTypeCreator(newSearch, &searchState, kPlugInFileType, 
        kPlugInFileCreator, false /* all versions */, &tempCardNo, &tempPlugInDbID);
    newSearch = false;
    
    DEBUGINFO("After search.");
    
    if ( !theErr ) {
      DEBUGINFO("No error.");
      DEBUGINFO("abba 6");
      PluginListP[curPlug] = (Plug_InternalRef *)MemPtrNew(sizeof(Plug_InternalRef));
      DEBUGINFO("abba 7");
      PluginListP[curPlug]->gPlugInDbRef = DmOpenDatabase(tempCardNo, tempPlugInDbID, dmModeReadOnly);
      DEBUGINFO("abba 3");
      tempPlugInResH = DmGetResource(kPlugInResourceType, kFirstPlugInResID);
      DEBUGINFO("abba 4");
      if ( !tempPlugInResH ) continue;
      DEBUGINFO("abba 5");

      // Get a lock on the code resource.      
      DEBUGINFO("abba");
      PluginListP[curPlug]->gPlugInResP = (Plug_Main *)MemHandleLock(tempPlugInResH);
      DEBUGINFO("abba");
      
      // Get the plugin's long name.
      DEBUGINFO("abba");
      nameH = DmGet1Resource ( 'tAIN', 1000 );
      DEBUGINFO("abba");
      if ( nameH ) {
      DEBUGINFO("abba");
        nameP = MemHandleLock(nameH);
        DEBUGINFO(nameP);
        StrCopy ( PluginListP[curPlug]->longname, nameP );
        MemHandleUnlock ( nameH );
      }
      
      DEBUGINFO("blorg");
      
      // Get the plugin's short (list) name.
      DEBUGINFO("abba");
      nameH = DmGet1Resource ( 'tSTR', 1 );
      DEBUGINFO("abba");
      if ( nameH ) {
      DEBUGINFO("abba");
        nameP = MemHandleLock(nameH);
        DEBUGINFO(nameP);
        StrCopy ( PluginListP[curPlug]->shortname, nameP );
        MemHandleUnlock ( nameH );
      }

      DEBUGINFO("shnee");

      // Send the plugin a startup event...
      plugInParamBlock.eType = cmdStartup;
      theErr = (*PluginListP[curPlug]->gPlugInResP) (&plugInParamBlock);
      
      DEBUGINFO("whoot");
      
      if ( theErr ) theErr = 0;
    }

  } while (!theErr);
}
#endif