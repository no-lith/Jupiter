project "LIB-MFCStub"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"
	warnings "Extra"

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
		libdir .. "/LIB-STDLith/src"
	}
	
    links 
	{ 
		"LIB-STDLith"
	}

    removefiles "src/mfcstub.cpp"