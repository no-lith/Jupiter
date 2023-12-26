project "LIB-ZLib"
	kind "StaticLib"
	language "C++"
	cppdialect "C++14"
	staticruntime "on"

	targetdir (bindir)
	objdir (intdir .. "/%{prj.name}")

	-- C1189: Macro definition of vsnprintf conflicts with Standard Library function declaration
	-- C++14 defines vsnprintf in stdio.h, zutil.h defines vsnprintf without considering the windows sdk version
	defines { "HAVE_VSNPRINTF" }

	files
	{
		"src/**.h", "src/**.cpp", "src/**.c"
	}

	includedirs
	{
		"src"
	}