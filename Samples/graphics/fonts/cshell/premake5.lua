project "CShell"
	kind "SharedLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"
	warnings "Extra"

	targetdir (local_bindir)
	objdir (local_intdir .. "/%{prj.name}")

	defines "_CLIENTBUILD"

	files
	{
		"src/**.h",
		"src/**.cpp",

		local_shrdir .. "/src/commandids.h",
		local_shrdir .. "/src/msgids.h",

		sdkdir .. "/iltbaseclass.cpp",
		sdkdir .. "/LTEulerAngles.cpp",
		sdkdir .. "/ltmodule.cpp"
	}

	includedirs
	{
		"src",
		local_shrdir .. "/src",
		
		sdkdir
	}