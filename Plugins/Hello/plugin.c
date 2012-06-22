#include <PalmOS.h>
#include <Standalone.h>
#include "Plugin.h"

STANDALONE_CODE_RESOURCE_ID(kFirstPlugInResID);

typedef struct {
  UInt32    called;
} plugGlobals;

UInt32 PlugInMain ( Plug_ParamBlockPtr eventP ) {
  UInt32 result = 0;
  plugGlobals *globals = 0;
  
  if ( eventP->globals )
    globals = (plugGlobals *)eventP->globals;
  
  if ( globals ) globals->called++;
  
  switch ( eventP->eType ) {
    case cmdStartup:
      if ( eventP->hostVersion != plugVersion ) {
        result = plugErrIncompatible;
        break;
      }
      
      eventP->globals = MemPtrNew(sizeof(plugGlobals));
      if ( !eventP->globals ) {
        result = plugErrGlobalSpace;
        break;
      }
      
      globals = (plugGlobals *)eventP->globals;
      
      eventP->data.startup.menuItem = true;
      globals->called = 1;
      
      result = 0;
      break;
      
    case cmdShutdown:
      if ( eventP->globals )
        MemPtrFree ( eventP->globals );
        
      if ( eventP->globals )
        result = plugErrGlobalSpace;
      else
        result = 0;
      break;
      
    default:
      result = plugErrNotImplemented;
      break;
  }

  return result;
}