-- Jupiter-Engine
workspace "Jupiter-Engine"
	architecture "x86"
	startproject "LithTech"

	characterset ("ASCII")

	configurations {  "Debug", "Release" }

	flags { "MultiProcessorCompile" }

	filter "configurations:Debug"
		defines { "WIN32", "_DEBUG", "_WINDOWS" }
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines { "WIN32", "NDEBUG", "_WINDOWS" }
		runtime "Release"
		optimize "full"

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