-- Jupiter-Samples
workspace "RenderDemo"
	architecture "x86"
	startproject "cshell"

	characterset ("ASCII")

	configurations { "Debug", "Release" }

	flags { "MultiProcessorCompile" }

	filter "configurations:Debug"
		defines { "WIN32", "_DEBUG", "DEBUG", "_WINDOWS" }
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines { "WIN32", "NDEBUG", "_WINDOWS" }
		runtime "Release"
		optimize "full"

	local_shrdir = "%{wks.location}/shared"
	local_bindir = "%{wks.location}/bin/%{cfg.buildcfg}"
	local_intdir = "%{wks.location}/bin-int/%{cfg.buildcfg}"

	-- Projects
	include "cres"
    include "cshell"
    include "sres"
    include "sshell"