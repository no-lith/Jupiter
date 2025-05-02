project "LIB-RezMgr"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"

	targetdir (bindir)
	objdir (intdir .. "/%{prj.name}")

	libdirs { bindir.. "/**" }

	files
	{
		"src/**.h", "src/**.cpp", "src/**.c"
	}

	includedirs
	{
		"src",
		libdir .. "/LIB-Lith/src",
        engdir .. "/sdk/inc"
	}
	
	links 
	{ 
		"LIB-Lith"
	}		