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

		engdir .. "/clientfx/src/shared/ClientFXDB.h",
		engdir .. "/clientfx/src/shared/ClientFXMgr.h",
		local_shrdir .. "/src/commandids.h",
		local_shrdir .. "/src/msgids.h",

		sdkdir .. "/iltbaseclass.cpp",
		sdkdir .. "/ltmodule.cpp",
		engdir .. "/clientfx/src/shared/ClientFXDB.cpp",
		engdir .. "/clientfx/src/shared/ClientFXMgr.cpp",
		engdir .. "/clientfx/src/shared/CommonUtilities.cpp",
		engdir .. "/clientfx/src/shared/ParsedMsg.cpp",
		engdir .. "/clientfx/src/shared/WinUtil.cpp"
	}

	includedirs
	{
		"src",
		local_shrdir .. "/src",
		
		sdkdir,
	    engdir .. "/clientfx/src/shared"
	}