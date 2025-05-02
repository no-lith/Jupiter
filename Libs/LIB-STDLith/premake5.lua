project "LIB-STDLith"
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
		libdir .. "/LIB-ZLib/src"
	}
	
	links 
	{ 
		"LIB-ZLib"
	}