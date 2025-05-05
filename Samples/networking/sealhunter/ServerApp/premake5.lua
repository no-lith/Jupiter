project "ServerApp"
	kind "WindowedApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"

	pchheader "stdafx.h"
	pchsource "src/win32/StdAfx.cpp"

	targetdir (local_bindir)
	objdir (local_intdir .. "/%{prj.name}")

	libdirs { bindir }
	
	files
	{
		"src/**.h",
		"src/**.cpp",
		"src/**.rc",

		sdkdir .. "/ltmodule.h",
		sdkdir .. "/ltmodule.cpp"
	}

	includedirs
	{
		"src",
		"src/shared",
		"src/win32",
		local_shrdir .. "/src",
		
		sdkdir,

		libdir .. "/LIB-Lith/src",
		libdir .. "/LIB-STDLith/src"
	}

	links
	{
		"LIB-Lith",
		"LIB-STDLith",
		"Server"
	}

	filter { "files:**/ltmodule.cpp" }
		flags "NoPCH"

	filter {}