###############################################################
#
# Makefile for Sealhunter object.dll Auto-generates subordinate
# makefiles as necessary based on the Visual C++ project files.
#
# (c) 2004 Monolith Productions, Inc. All Rights Reserved.
# (c) 2004 TouchdownEntertainment, Inc. All Rights Reserved
#
# dlj - 11/04 modified for 2002 and Jupiter and sealhunter
#
###############################################################

all: Makefile.top build

Makefile.top: sealhunter.sln
	$(SHELL) -ec 'python ../../../Linux/sln2makefile.py -s sealhunter.sln -p object > Makefile.top'

build:
	@(make -f Makefile.top VCPROJSCRIPTPATH=../../../Linux )
	@(echo 'Compiling Linux server application...')
	@(cd ServerApp/Linux;make)
	
clean:
	@(echo 'Cleaning Linux server application...')
	@(cd ServerApp/Linux;make clean)
	@(cd ../..)
	make -f Makefile.top clean
	rm -f Makefile.top

rebuild: clean build

