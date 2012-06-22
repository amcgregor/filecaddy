##############################################################################
# Targets:
#	all     -	build
#	clean   -	clean everything
#	check   -	run self-tests
# 	release - archive everything, move to release directory, and clean up
#
# This makefile uses the following environment variables:
# 	PALMTOOLSU - path to directory where palm tools should go.
#       	ex: "/usr/local/PalmTools"
#	
# Conventions:
#        "ALLUPPER"  case variable names are generally exported or provided by
#                    the environment
#        "initLower" case variables are local to the makefile
#
# Commonly used  Automatic Variables:
#         $< represents the first dependency
#         $^ represents all dependencies
#         $? represents all dependencies that are newer than the target
#         $@ represents the target 

##############################################################################

# Copyright (c) 2002, Kopsis, Inc.

# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the
# Free Software Foundation; either version 2 of the License, or (at your
# option) any later version.
        
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.

# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

DATE              =`date "+%Y%m%d"`
LANGUAGE          = EN

CC                = /usr/bin/m68k-palmos-gcc
PALMRC            = /usr/bin/build-prc
PALMMAKEROM       = /usr/bin/Palm-MakeROM
PILRC             = /usr/local/bin/pilrc
OBJRES            = /usr/bin/m68k-palmos-obj-res
SHELL	            = /bin/sh

PalmDefs          =-DEMULATION_LEVEL=0 -DNOHIRES
PalmCOptions      =-O2 -Wall
#PalmCustomCFlags  += -g
#PalmCustomCFlags  += -mdebug-labels

# Directory paths
buildDir          = $(CURDIR)
objDir	          = ../Obj
srcDir	          = ../Src
rscDir	          = ../Rsrc
testsDir          = ../Tests
releaseDir        = ../Release

# ANSI color codes, define "USECOLOR" to any value to use.
ifdef USECOLOR
RESETANSI         =[m
WHITEANSI         =[0;1;37m
DARKANSI          =[0;1;30m
ERRORANSI         =[0;1;31m
else
RESETANSI         =
WHITEANSI         =
DARKANSI          =
ERRORANSI         =
endif

# Create a file in the Build directory to customize some of the above variables.
-include customvars.make

# Target information
include $(srcDir)/FileCaddy.make
include $(srcDir)/BuildNumber.make

PalmCFlags        = $(PalmDefs) $(PalmCOptions) $(PalmCustomCFlags)

# These are fake targets used to perform certain actions. Use the .PHONY
#  command to make sure they don't get confused with actual filenames
.PHONY : default all install clean check archive release help manual

# Set the default install directory if not passed down to us
#  from the parent makefile
ifndef InstallDir
  InstallDir = ~/.palm/install
endif

#############################################################
# Master Builds
#############################################################
all: 	$(objDir) $(objDir)/$(progFile).prc
	@perl incbuild.pl $(srcDir)
	@rm -f *.bin *.grc

install:   
	@cp $(objDir)/$(progFile).prc $(InstallDir)

clean:
	@echo -e "$(WHITEANSI)	Removing unnessicary files $(DARKANSI)"
	-@rm -vf *.bin *.grc $(objDir)/* $(releaseDir)/* | clipmsg
	@echo -en "$(RESETANSI)"

check:
	@echo -e "$(WHITEANSI)	Checking PRC... $(RESETANSI)"
	@cmp $(objDir)/$(progFile).prc $(releaseDir)/$(progFile).prc

archive : clean
ifdef usecvs
	@echo -e "$(WHITEANSI)	Downloading latest snapshot from CVS $(RESETANSI)"
	@cd .. ;	cvs -z3 update -Pd
	@echo -e "$(WHITEANSI)	Rebuilding from snapshot $(RESETANSI)"
	@make all clean
endif
	@echo -e "$(WHITEANSI)	Archiving $(RESETANSI)"
	@cd ../.. ; tar czvf $(progName)-${DATE}.tgz $(progName) | $(progName)/Build/clipmsg
	@echo -en "$(RESETANSI)"

manual :
	@echo -e "$(WHITEANSI)	Building Manual (iSilo 3.x) $(DARKANSI)"
	@cd ../Manual/Source; iSiloXC -o Untitled.ixl -x Untitled.ixl
	@echo -en "$(RESETANSI)"

release : clean
ifdef usecvs
	@echo -e "$(WHITEANSI)	Downloading latest snapshot from CVS $(RESETANSI)"
	@cd .. ; cvs -z3 update -Pd
	@echo -e "$(WHITEANSI)	Rebuilding from snapshot $(DARKANSI)"
	@make clean
endif
	@make all
	@echo -e "$(WHITEANSI)	Copying files into \"$(releaseDir)\" $(DARKANSI)"
	@cp -vf ../CREDITS ../HISTORY ../INSTALL ../LICENSE ../NEWS ../README ../TODO $(objDir)/$(progName).prc $(releaseDir) | clipmsg
	@echo -e "$(WHITEANSI)	Archiving $(DARKANSI)"
	@mv $(releaseDir) ../$(progName)
	@cd .. ; tar czvf $(progName)-${DATE}-release.tgz $(progName) | Build/clipmsg
	@mv ../$(progName) $(releaseDir)
	@echo -en "$(RESETANSI)"

help :
	@echo -e "\n$(WHITEANSI)	Makefile Help $(RESETANSI)"
	@echo -e "You can use the following phony targets:"
	@echo -e "  all		- rebuild the application, if nessicary"
	@echo -e "  clean		- removes unnessicary files and empties the release directory"
	@echo -e "  install	- copies the PRC into InstallDir"
	@echo -e "  check		- check to see if there is a difference between release and\n\t\t  current PRC files"
	@echo -e "  archive	- cleans, updates, and archives the current source tree"
	@echo -e "  manual	- build the iSilo 3.x manual from HTML source"
	@echo -e "  release	- cleans, updates, archives, and builds a release package"
	@echo -e "\nThe following variable(s) $(WHITEANSI)must$(RESETANSI) be defined:"
	@echo -e "  PALMTOOLSU"
	@echo -e "\nThe following are variables and their values:"
	@echo -e "  DATE		- $(DATE)"
	@echo -e "  LANGUAGE	- $(LANGUAGE)"
	@echo -e "  PILRC		- $(PILRC)"
	@echo -e "  OBJRES	- $(OBJRES)"
	@echo -e "  SHELL		- $(SHELL)"
	@echo -e "  buildDir	- $(buildDir)"
	@echo -e "  objDir	- $(objDir)"
	@echo -e "  srcDir	- $(srcDir)"
	@echo -e "  rscDir	- $(rscDir)"
	@echo -e "  testsDir	- $(testsDir)"
	@echo -e "  releaseDir	- $(releaseDir)"
	@echo -e "  InstallDir	- $(InstallDir)"
	@echo -e "  progCreator	- $(progCreator)"
	@echo -e "  progPrcFile	- $(progPrcFile)"
	@echo -e "  progFile	- $(progFile)"
	@echo -e "  progName	- $(progName)"
	@echo -e "\nTo use CVS, define the variable 'usecvs' with any value and export these:"
	@echo -e " export CVS_RSH=\"ssh\""
	@echo -e " export CVSROOT=$(DARKANSI)loginname$(RESETANSI)@cvs.$(progFile).sourceforge.net:/cvsroot/$(progFile)"
	@echo -e "\nNote that any time the target $(WHITEANSI)all$(RESETANSI) is executed, the build number is incremented.\n"

#############################################################
# Implicit build rules
#############################################################
$(objDir)/%.o :  $(srcDir)/%.c
ifdef showcommands
	@echo -ne "\n"
endif
	@echo -e "$(WHITEANSI)	Compiling \"$<\" -> \"$@\" $(ERRORANSI)"
ifdef showcommands
	@echo -e "$(RESETANSI)$(CC) $(PalmCFlags)\\n\t-c $< -o $@ $(ERRORANSI)"
endif
	@$(CC) $(PalmCFlags) -c $< -o $@
	@echo -en "$(RESETANSI)";

#############################################################
# Program Build
#############################################################

$(objDir):
	@echo -e "$(WHITEANSI)	Creating object directory$(RESETANSI)"
	@mkdir $(objDir)

# ------------------------------------------------------------
# Link and combine with resources. 
# change the --type appl to a -L for libraries
# ------------------------------------------------------------
$(objDir)/$(progFile).prc: $(objList) $(rscList)
	@echo -e "$(WHITEANSI)	Compiling all object files -> \"$(objDir)/$(progFile)\" $(ERRORANSI)"
	@$(CC) $(PalmCFlags) $(objList) -o $(objDir)/$(progFile)
	@echo -e "$(WHITEANSI)	Compiling resources... $(DARKANSI)"
	@$(PILRC) -q -I $(rscDir) -D LINUXTOOLS $(rscDir)/$(progName).rcp
	@$(OBJRES) $(objDir)/$(progFile)
	@echo -e "$(WHITEANSI)	Compiling PRC \"$(objDir)/$(progName).prc\" $(ERRORANSI)"
	@$(PALMRC) --output $(objDir)/$(progName).prc \
                  --creator $(progCreator) \
                  --type appl \
                  --name $(progName) \
                  --version-number $(BuildNumber) \
                  *.grc *.bin
	@echo -en "$(RESETANSI)";
