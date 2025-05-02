project "ClientFX"
	kind "SharedLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"

	pchheader "stdafx.h"
	pchsource "src/stdafx.cpp"

	targetdir (bindir)
	objdir (intdir .. "/%{prj.name}")

	libdirs { bindir.. "/**" }

	targetextension (".fxd")

	defines { "NOPS2", "MEMTRACK_CLIENTFX" }
	
	files
	{
		"src/bouncychunkfx.h",
		"src/camjitterfx.h",
		"src/camwobblefx.h",
		"src/clientfx.h",
		"src/CreateFX.h",
		"src/cycletimer.h",
		"src/dynalightfx.h",
		"src/fallingstufffx.h",
		"src/FlareSpriteFX.h",
		"src/lightningfx.h",
		"src/LTBbouncychunkfx.h",
		"src/ltbmodelfx.h",
		"src/memarray.h",
		"src/nullfx.h",
		"src/ParticleSystemFX.h",
		"src/playrandomsoundfx.h",
		"src/playsoundfx.h",
		"src/polyfanfx.h",
		"src/polytubefx.h",
		"src/resource.h",
		"src/spritefx.h",
		"src/spritesystemfx.h",
		"src/stdafx.h",
		"src/swaplist.h",
		"src/Shared/BankedList_impl.h",
		"src/Shared/BankedList.h",
		"src/Shared/BaseFx.h",
		"src/Shared/DebugNew_impl.h",
		"src/Shared/DebugNew.h",

		sdkdir .. "/ltmodule.cpp",
		sdkdir .. "/ltquatbase.cpp",
		"src/BaseFx.cpp",
		"src/bouncychunkfx.cpp",
		"src/camjitterfx.cpp",
		"src/camwobblefx.cpp",
		"src/clientfx.cpp",
		"src/CreateFX.cpp",
		"src/cycletimer.cpp",
		"src/dynalightfx.cpp",
		"src/fallingstufffx.cpp",
		"src/FlareSpriteFX.cpp",
		"src/lightningfx.cpp",
		"src/LTBbouncychunkfx.cpp",
		"src/ltbmodelfx.cpp",
		"src/nullfx.cpp",
		"src/ParticleSystemFX.cpp",
		"src/playrandomsoundfx.cpp",
		"src/playsoundfx.cpp",
		"src/polyfanfx.cpp",
		"src/polytubefx.cpp",
		"src/spritefx.cpp",
		"src/spritesystemfx.cpp",
		"src/stdafx.cpp",
		"src/Shared/DebugNew.cpp"
	}

	includedirs
	{
		"src",
		"src/shared",
		
		sdkdir,

		libdir .. "/LIB-MFCStub/src",
		libdir .. "/LIB-STDLith/src"
	}

	links 
	{ 
		"LIB-MFCStub",
		"LIB-STDLith"
	}

	filter { "files:**/ltmodule.cpp" }
		flags "NoPCH"

	filter { "files:**/ltquatbase.cpp" }
		flags "NoPCH"