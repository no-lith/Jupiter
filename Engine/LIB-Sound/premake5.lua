project "LIB-Sound"
	kind "StaticLib"
	language "C++"
	cppdialect "C++14"
	staticruntime "on"

	characterset ("ASCII")

	targetdir (bindir)
	objdir (intdir .. "/%{prj.name}")

	libdirs {bindir.. "/**"}

	defines { "USE_ABSTRACT_SOUND_INTERFACES" }

	files
	{
		-- for future additions to the project itself
		"src/**.h",
		"src/**.cpp",

		rundir .. "/sound/src/iltsound.h",
		rundir .. "/sound/src/iltsound.cpp"
	}

	includedirs
	{
		"src",

		-- sdk
		sdkdir
	}