project "ServerApp"
	kind "WindowedApp"
	language "C++"
	cppdialect "C++14"
	staticruntime "on"

	pchheader "stdafx.h"
	pchsource "src/win32/stdafx.cpp"

	targetdir (local_bindir)
	objdir (local_intdir .. "/%{prj.name}")

	libdirs { bindir.. "/**" }
	
	files
	{
		"src/**.h",
		"src/**.cpp",
		"src/**.rc",

		sdkdir .. "/ltmodule.h"
	}

	includedirs
	{
		"src",
		local_shrdir .. "/src",
		
		sdkdir,

		libdir .. "/LIB-Lith/src",
		libdir .. "/LIB-STDLith/src"
	}

	links
	{
		"LIB-Lith",
		"LIB-STDLith"
	}