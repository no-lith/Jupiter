project "Resources"
	kind "SharedLib"
	language "C++"
	cppdialect "C++14"
	staticruntime "on"

	targetdir (bindir)
	objdir (intdir .. "/%{prj.name}")

	files { "**.cpp", "src/lang/en/ltmsg.rc" }

	includedirs "src/lang/en"

	filter "files:ltmsg.rc"
		buildaction "ResourceCompile"