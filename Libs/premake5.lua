-- Jupiter-Libs
workspace "Jupiter-Libs"
	architecture "x86"
	startproject "LIB-ButeMgr"

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
	