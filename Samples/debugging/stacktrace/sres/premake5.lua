project "SRes"
	kind "SharedLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"
	warnings "Extra"

	targetdir (local_bindir)
	objdir (local_intdir.. "/%{prj.name}")

	files { "src/**.cpp", "src/**.h", "src/**.rc" }

	includedirs "src"

	filter "files:**.rc"
		buildaction "ResourceCompile"