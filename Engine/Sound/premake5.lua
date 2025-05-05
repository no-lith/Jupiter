project "Sound"
	kind "SharedLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"
	warnings "Extra"

	characterset ("ASCII")

	targetdir (bindir)
	objdir (intdir .. "/%{prj.name}")

	libdirs { bindir.. "/**" }

	files
	{
		-- for future additions to the project itself
		"src/**.h",
		"src/**.cpp",
		"src/**.c",

		rundir .. "/sound/src/sys/s_dx8/StdAfx.h",
		rundir .. "/sound/src/sys/s_dx8/s_dx8.h",

		rundir .. "/kernel/src/sys/win/counter.cpp",
		rundir .. "/sound/src/sys/s_dx8/parsewavefile.cpp",
		rundir .. "/sound/src/sys/s_dx8/StdAfx.cpp",
		rundir .. "/sound/src/sys/s_dx8/s_dx8.cpp"
	}

	includedirs
	{
		"src",

		-- sdk
		sdkdir,

		-- runtime
		rundir,
		rundir .. "/kernel/src",
		rundir .. "/shared/src/sys/win",
		rundir .. "/sound/src",

		-- external libs
		libdir .. "/LIB-Lith/src",
		libdir .. "/LIB-LTMem/src"
	}

	-- EAX (Environmental Audio Extensions) include
	externalincludedirs
	{
		vendir .. "/eax/include"
	}

	syslibdirs
	{
		vendir .. "/eax/lib"
	}

	links
	{
		"odbc32",
		"odbccp32",
		"dxguid",
		"winmm",
		"msacm32",
		"eaxguid",

		"LIB-Lith",
		"LIB-LTMem"
	}