project "Resources"
	kind "SharedLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"
	warnings "Extra"

	targetdir (bindir)
	objdir (intdir .. "/%{prj.name}")

	files { "**.cpp", "src/lang/en/ltmsg.rc" }

	includedirs "src/lang/en"

	filter "files:ltmsg.rc"
		buildaction "ResourceCompile"