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
		
		sdkdir .. "/iltbaseclass.cpp",
		sdkdir .. "/ltengineobjects.cpp",
		sdkdir .. "/ltmodule.cpp",
		sdkdir .. "/ltquatbase.cpp",
		engdir .. "/clientfx/src/shared/ParsedMsg.cpp"
	}

	includedirs
	{
		"src",
		local_shrdir .. "/src",
		
	    engdir .. "/clientfx/src/shared",
		sdkdir
	}