project "Object"
	kind "SharedLib"
	language "C++"
	cppdialect "C++14"
	staticruntime "on"

	targetdir (local_bindir)
	objdir (local_intdir .. "/%{prj.name}")

	targetextension ".lto"

	files
	{
		"src/**.h",
		"src/**.cpp",

		engdir .. "/clientfx/src/shared/ParsedMsg.cpp",
		local_shrdir .. "/src/helper.h",
		local_shrdir .. "/src/msgids.h",
		
		sdkdir .. "/iltbaseclass.cpp",
		sdkdir .. "/ltengineobjects.cpp",
		sdkdir .. "/ltmodule.cpp",
		sdkdir .. "/ltquatbase.cpp"
	}

	includedirs
	{
		"src",
		local_shrdir .. "/src",
		
		sdkdir,
	    engdir .. "/clientfx/src/shared"
	}