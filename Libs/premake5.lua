-- Jupiter-Libs
workspace "Jupiter-Libs"
	architecture "x86"
	startproject "LIB-ButeMgr"

	characterset ("ASCII")

	configurations
	{
		"Debug",
		"Release"
	}

	flags
	{
		"MultiProcessorCompile"
	}

	filter "configurations:Debug"
		defines { "WIN32", "_DEBUG", "DEBUG", "_WINDOWS" }
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines { "WIN32", "NDEBUG", "_WINDOWS" }
		runtime "Release"
		optimize "full"

	-- Projects
	include "LIB-ButeMgr"
	include "LIB-CryptMgr"
	include "LIB-DIBMgr"
	include "LIB-DShow"
	include "LIB-DTXMgr"
	include "LIB-GenRegMgr"
	include "LIB-GuiMgr"
	include "LIB-Lith"
	include "LIB-LTAMgr"
	include "LIB-LTMem"
	include "LIB-MFCStub"
	include "LIB-RegMgr"
	include "LIB-RegMgr32"
	include "LIB-RezMgr"
	include "LIB-StackTracer"
	include "LIB-STDLith"
	include "LIB-ZLib"
	