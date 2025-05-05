project "LIB-DTXMgr"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"
	warnings "Extra"

	targetdir (bindir)
	objdir (intdir .. "/%{prj.name}")

	libdirs { bindir.. "/**", vendir.. "/S3TC" }

	files
	{
		"src/**.h", "src/**.cpp", "src/**.c"
	}

	includedirs
	{
		"src",

		-- sdk
		sdkdir,

		-- tool
		toldir .. "/shared/engine",
		toldir .. "/shared/world",

		-- external lib
		libdir .. "/LIB-STDLith/src"
	}

	-- EAX (Environmental Audio Extensions) include
	externalincludedirs
	{
		vendir .. "/s3tc/include"
	}

	syslibdirs
	{
		vendir .. "/s3tc/lib"
	}
	
	links 
	{ 
		"LIB-STDLith",
		"s3tc"
	}