workspace "MPFlyCam"
	architecture "x86"
	startproject "cshell"

	characterset ("ASCII")

	configurations { "Debug", "Release", "Final" }

	filter "configurations:*"
		defines { "WIN32", "_WINDOWS" }

	filter "Debug"
		defines { "_DEBUG", "DEBUG" }
		runtime "Debug"
		symbols "on"

	filter "Release or Final"
		defines { "NDEBUG" }
		runtime "Release"
		optimize "full"

	filter "Final"
		defines { "_FINAL" }

	filter {}

	flags { "MultiProcessorCompile" }

	local_shrdir = "%{wks.location}/shared"
	local_bindir = "%{wks.location}/bin/%{cfg.buildcfg}"
	local_intdir = "%{wks.location}/bin-int/%{cfg.buildcfg}"

	-- Projects
	include "cres"
    include "cshell"
    include "sres"
    include "sshell"