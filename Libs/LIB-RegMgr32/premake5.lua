project "LIB-RegMgr32"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"

	targetdir (bindir)
	objdir (intdir .. "/%{prj.name}")

	files
	{
		"src/**.h", "src/**.cpp", "src/**.c"
	}

	includedirs
	{
		"src"
	}