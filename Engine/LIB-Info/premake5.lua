project "LIB-Info"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"
	warnings "Extra"

	characterset ("ASCII")

	targetdir (bindir)
	objdir (intdir .. "/%{prj.name}")

	libdirs {bindir.. "/**"}

	files
	{
		-- for future additions to the project itself
		"src/**.h",
		"src/**.cpp",

		sdkdir .. "/libltinfo.h",
		sdkdir .. "/libltinfo/iltoutputredir.h",
		rundir .. "/info/src/outputredir.h",

		rundir .. "/info/src/outputredir.cpp",
		rundir .. "/info/src/sys/win32/outputredir_win32.cpp",
	}

	includedirs
	{
		"src",

		-- sdk
		sdkdir,
		sdkdir .. "/libltinfo",

		-- runtime
		rundir,
		rundir .. "/info/src",
		rundir .. "/kernel/src",
		rundir .. "/shared/src"
	}

	filter { "files:**/runtime/info/src/outputredir.cpp or files:**/runtime/info/src/sys/win32/outputredir_win32.cpp" }
		flags "NoPCH"