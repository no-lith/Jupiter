// ----------------------------------------------------------------------- //
//
// MODULE  : main.cpp
//
// PURPOSE : Main file for Linux server application.
//
// (c) 2004 Monolith Productions, Inc.  All Rights Reserved
// (c) 2004 Touchdown Entertainment Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#include "ServerApp.h"

int main(int argc, char* argv[], char* env[])
{
	// instantiate server class
	CServerApp cServerApp;
	
	// run it
	int nResult = cServerApp.Run(argc, argv);
	
	return nResult;
}

