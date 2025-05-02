-- Jupiter-Engine
workspace "Jupiter-Engine"
	architecture "x86"
	startproject "LithTech"

	characterset ("ASCII")

	configurations { "Debug", "Release", "Final" }

	filter "configurations:*"
		defines { "WIN32", "_WINDOWS" }

	filter "Debug"
		defines { "_DEBUG", "DEBUG" }
		runtime "Debug"
		symbols "on"

	filter "Release or Final"
		defines { "NDEBUG" }
		runtime "Release"
		optimize "full"

	filter "Final"
		defines { "_FINAL" }

	filter {}

	flags { "MultiProcessorCompile" }

	group "External Libs"
		includeexternal "Libs/LIB-ButeMgr"
		includeexternal "Libs/LIB-CryptMgr"
		includeexternal "Libs/LIB-DShow"
		includeexternal "Libs/LIB-DTXMgr"
		includeexternal "Libs/LIB-GenRegMgr"
		includeexternal "Libs/LIB-GuiMgr"
		includeexternal "Libs/LIB-Lith"
		includeexternal "Libs/LIB-LTAMgr"
		includeexternal "Libs/LIB-LTMem"
		includeexternal "Libs/LIB-MFCStub"
		includeexternal "Libs/LIB-RegMgr"
		includeexternal "Libs/LIB-RegMgr32"
		includeexternal "Libs/LIB-RezMgr"
		includeexternal "Libs/LIB-StackTracer"
		includeexternal "Libs/LIB-STDLith"
		includeexternal "Libs/LIB-ZLib"
	group ""
	
	group "Engine Libs"
		include "LIB-Info"
		include "LIB-Render"
		include "LIB-Sound"
		include "LIB-UI"
	group ""

	group "Engine Core"
		include "ClientFX"
		include "LithTech"
		include "Resources"
		include "Server"
		include "Sound"
	group ""