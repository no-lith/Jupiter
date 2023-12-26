###############################################################
#
# Makefile for Linux_ServerApp.
#
# (c) 2004 Monolith Productions, Inc. All Rights Reserved.
#
###############################################################

# Build tools
ifeq "$(CPP_TOOL)" "" 
CPP_TOOL=g++
endif
ifeq "$(CC_TOOL)" ""
CC_TOOL=gcc
endif
ifeq "$(LIB_TOOL)" ""
LIB_TOOL=ar
endif
ifeq "$(EXE_TOOL)" ""
EXE_TOOL=g++
endif

# Project name
ProjectName=ServerApp

# Set default configuration
ifeq "$(ConfigurationName)" ""
ConfigurationName = Debug
endif

# Platform-specific variables
LINUX_DEFINES=-D_LINUX -D__LINUX -DDISABLE_LT_MEM_TRACKING

# SourceFiles
SOURCEFILES=./Main.cpp \
	../Shared/DedicatedServerBase.cpp \
	./ServerApp.cpp \
	./linux_ltcriticalsection.cpp \
	./linux_ltlibraryloader.cpp \
	./linux_ltthread.cpp \
	../../../../../Engine/sdk/inc/ltmodule.cpp 
		
# Object files
OBJECTFILES=$(IntDir)/Main.o \
	$(IntDir)/DedicatedServerBase.o \
	$(IntDir)/ServerApp.o \
	$(IntDir)/linux_ltcriticalsection.o \
	$(IntDir)/linux_ltlibraryloader.o \
	$(IntDir)/linux_ltthread.o \
	$(IntDir)/ltmodule.o

###############################################################
# Configuration: Release
###############################################################
ifeq "$(ConfigurationName)" "Release"

# Output and Intermediate directories for this configuration
OutDir=$(ConfigurationName)Linux
IntDir=$(OutDir)

EngineBuilt=../../../../../Engine/built/$(OutDir)/
LibsBuilt=../../../../../Libs/built/$(OutDir)/

# Defines, includes, and libraries
DEFINES=$(LINUX_DEFINES)  -DNDEBUG
INCLUDES=-I.  -I./ -I../Shared -I../../Shared -I../../../../../Engine/sdk/inc -I../../../../../libs/stdlith -I../../../../../libs/lith

LIBRARYPATHS=-L$(EngineBuilt)libs/Lib_RezMgr -L$(EngineBuilt)libs/Lib_ltmem -L$(EngineBuilt)Runtime/DLL_Server -L$(EngineBuilt)Runtime/Lib_Info -L$(LibsBuilt)Lib_Lith -L$(LibsBuilt)Lib_StdLith -L$(LibsBuilt)Lib_ZLib

LIBRARIES=-ldl \
	-lserver \
	-lLib_RezMgr \
        -lltmem \
        -llibltinfo \
        -lLib_Lith \
        -lLib_StdLith \
        -lLib_ZLib

# assumes ServerApp.bin is in SealHunter directory
RPATHS=-Wl,-rpath,./sshell/obj/$(OutDir) -Wl,-rpath,./ -Wl,-rpath,../../../Engine/built/$(OutDir)/Runtime/DLL_Server
	
# Compile and Link commands
CPP_COMPILE=$(CPP_TOOL) -rdynamic -pthread -c -o "$(IntDir)/$(*F).o" $(DEFINES) $(INCLUDES)
CC_COMPILE=$(CC_TOOL) -rdynamic -pthread -c -o "$(IntDir)/$(*F).o" $(DEFINES) $(INCLUDES)

TargetPath=$(OutDir)/$(ProjectName).bin
			
LINK=$(EXE_TOOL) -rdynamic -pthread -o "$(TargetPath)" $(OBJECTFILES) $(RPATHS) $(LIBRARIES) $(LIBRARYPATHS)
    
MAKEDEPEND=$(CC_TOOL) $(DEFINES) $(INCLUDES) -w -MM -MP

# Build rules
$(TargetPath): $(IntDir) $(OutDir) $(OBJECTFILES)
	@($(SHELL) -ec 'if [ "$(filter '%.a', '$(TargetPath)')" != "" ]; then echo "Archiving $(TargetPath)"; else echo "Linking $(TargetPath)"; fi')
	@($(LINK))
	@(echo "Done.")

$(IntDir):
	@(mkdir -p "$(IntDir)")
	
$(OutDir):
	@(mkdir -p "$(OutDir)")

$(IntDir)/%.o:
	@(echo "Compiling $(filter %/$*.cpp %/$*.c, $(SOURCEFILES))")
	@($(SHELL) -ec 'if [ "$(filter %/$*.cpp, $(SOURCEFILES))" != "" ]; \
		then $(CPP_COMPILE) "$(filter %/$*.cpp, $(SOURCEFILES))"; \
		elif [ "$(filter %/$*.c, $(SOURCEFILES))" != "" ]; \
		then $(CC_COMPILE) "$(filter %/$*.c, $(SOURCEFILES))"; \
		fi')

ifneq "$(MAKECMDGOALS)" "clean"
-include $(OBJECTFILES:%.o=%.d)
endif

$(IntDir)/%.d: $(IntDir)
	@($(SHELL) -ec 'if ! $(MAKEDEPEND) -MT "$(IntDir)/$(*F).o $(IntDir)/$(*F).d" "$(filter %/$*.cpp %/$*.c, $(SOURCEFILES))" > "$(IntDir)/$(*F).d"; then rm "$(IntDir)/$(*F).d"; fi')

clean:
	-rm -f $(TargetPath)
	-rm -f $(IntDir)/*.d
	-rm -f $(IntDir)/*.o
	
rebuild: clean $(TargetPath)
	
endif                               

	
###############################################################
# Configuration: Debug
###############################################################
ifeq "$(ConfigurationName)" "Debug"

# Output and Intermediate directories for this configuration
OutDir=$(ConfigurationName)Linux
IntDir=$(OutDir)

EngineBuilt=../../../../../Engine/built/$(OutDir)/
LibsBuilt=../../../../../Libs/built/$(OutDir)/

# Defines, includes, and libraries
DEFINES=$(LINUX_DEFINES) -D_DEBUG

INCLUDES=-I.  -I./ -I../Shared -I../../Shared -I../../../../../Engine/sdk/inc -I.././../../../libs/stdlith -I../../../../../libs/lith

LIBRARYPATHS=-L$(EngineBuilt)libs/Lib_RezMgr -L$(EngineBuilt)libs/Lib_ltmem -L$(EngineBuilt)Runtime/DLL_Server -L$(EngineBuilt)Runtime/Lib_Info -L$(LibsBuilt)Lib_Lith -L$(LibsBuilt)Lib_StdLith -L$(LibsBuilt)Lib_ZLib

LIBRARIES=-ldl \
	-lserver \
	-lLib_RezMgr \
        -lltmem \
        -llibltinfo \
        -lLib_Lith \
        -lLib_StdLith \
        -lLib_ZLib
		
# assumes ServerApp.bin is in SealHunter directory
RPATHS=-Wl,-rpath,./sshell/obj/$(OutDir) -Wl,-rpath,./ -Wl,-rpath,../../../Engine/built/$(OutDir)/Runtime/DLL_Server

# Compile and Link commands
CPP_COMPILE=$(CPP_TOOL) -rdynamic -pthread -c -g -o "$(IntDir)/$(*F).o" $(DEFINES) $(INCLUDES)
CC_COMPILE=$(CC_TOOL) -rdynamic -pthread -c -g -o "$(IntDir)/$(*F).o" $(DEFINES) $(INCLUDES)

TargetPath=$(OutDir)/$(ProjectName).bin
			
LINK=$(EXE_TOOL) -rdynamic -pthread -g -o "$(TargetPath)" $(OBJECTFILES) $(RPATHS) $(LIBRARYPATHS) $(LIBRARIES) 
    
MAKEDEPEND=$(CC_TOOL) $(DEFINES) $(INCLUDES) -w -MM -MP

# Build rules
$(TargetPath): $(IntDir) $(OutDir) $(OBJECTFILES)
	@($(SHELL) -ec 'if [ "$(filter '%.a', '$(TargetPath)')" != "" ]; then echo "Archiving $(TargetPath)"; else echo "Linking $(TargetPath)"; fi')
	($(LINK))
	@(echo "Done.")

$(IntDir):
	@(mkdir -p "$(IntDir)")
	
$(OutDir):
	@(mkdir -p "$(OutDir)")

$(IntDir)/%.o:
	@(echo "Compiling $(filter %/$*.cpp %/$*.c, $(SOURCEFILES))")
	@($(SHELL) -ec 'if [ "$(filter %/$*.cpp, $(SOURCEFILES))" != "" ]; \
		then $(CPP_COMPILE) "$(filter %/$*.cpp, $(SOURCEFILES))"; \
		elif [ "$(filter %/$*.c, $(SOURCEFILES))" != "" ]; \
		then $(CC_COMPILE) "$(filter %/$*.c, $(SOURCEFILES))"; \
		fi')

ifneq "$(MAKECMDGOALS)" "clean"
-include $(OBJECTFILES:%.o=%.d)
endif

$(IntDir)/%.d: $(IntDir)
	@($(SHELL) -ec 'if ! $(MAKEDEPEND) -MT "$(IntDir)/$(*F).o $(IntDir)/$(*F).d" "$(filter %/$*.cpp %/$*.c, $(SOURCEFILES))" > "$(IntDir)/$(*F).d"; then rm "$(IntDir)/$(*F).d"; fi')

clean:
	-rm -f $(TargetPath)
	-rm -f $(IntDir)/*.d
	-rm -f $(IntDir)/*.o
	
rebuild: clean $(TargetPath)
	
endif                               

	

	
