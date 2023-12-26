project "CShell"
	kind "SharedLib"
	language "C++"
	cppdialect "C++14"
	staticruntime "on"

	targetdir (local_bindir)
	objdir (local_intdir .. "/%{prj.name}")

	libdirs { bindir.. "/**" }

	defines "_CLIENTBUILD"

	files
	{
		"src/**.h",
		"src/**.cpp",

		local_shrdir .. "/src/commandids.h",
		local_shrdir .. "/src/msgids.h",

		sdkdir .. "/iltbaseclass.cpp",
		sdkdir .. "/ltmodule.cpp"
	}

	includedirs
	{
		"src",
		local_shrdir .. "/src",
		
		sdkdir,

		-- external libs
		libdir .. "/LIB-StackTracer/src"
	}

	links
	{
		"LIB-StackTracer"
	}