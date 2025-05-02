project "Object"
	kind "SharedLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"

	targetdir (local_bindir)
	objdir (local_intdir .. "/%{prj.name}")

	targetextension ".lto"

	files
	{
		"src/*.h",
		"src/*.cpp",

		engdir .. "/clientfx/src/shared/ClientFXDB.h",
		engdir .. "/clientfx/src/shared/ClientFXMgr.h",
		local_shrdir .. "/src/helper.h",
		local_shrdir .. "/src/msgids.h",
		
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