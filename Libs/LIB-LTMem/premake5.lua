project "LIB-LTMem"
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
		libdir .. "/LIB-STDLith/src",
		sdkdir,
		rundir .. "/shared/src",
		rundir .. "/kernel/src",
		rundir .. "/kernel/mem/src"
	}
	
	links 
	{ 
		"LIB-STDLith"
	}

	removefiles "src/ltheap.*"