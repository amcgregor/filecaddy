# Compiler Flags -- missing -Werror
PalmCustomCFlags = -Wno-unknown-pragmas -I$(rscDir)

# Creator ID
progCreator	=FCad
progPrcFile	=$(progName).prc

progFile	  =filecaddy
progName	  =FileCaddy

# ------------------------------------------------------------
# Compile objects 
# ------------------------------------------------------------

objList = $(objDir)/AppMain.o \
          $(objDir)/MainForm.o \
          $(objDir)/Utility.o \
          $(objDir)/Progress.o \
          $(objDir)/IconCache.o \
          $(objDir)/TextForm.o \
          $(objDir)/Plugin.o

rscList = $(rscDir)/$(progName).rcp


