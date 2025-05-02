project "LIB-ButeMgr"
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

	defines
	{
		"_NOMFC",
		"_SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS"
	}

	includedirs
	{
		"src",
		libdir .. "/LIB-CryptMgr/src",
		libdir .. "/LIB-Lith/src",
		libdir .. "/LIB-MFCStub/src",
		libdir .. "/LIB-STDLith/src"		
	}

	links 
	{
		"LIB-CryptMgr",
		"LIB-Lith",
		"LIB-MFCStub",
		"LIB-STDLith"
	}