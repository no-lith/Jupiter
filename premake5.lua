-- Engine directory
engdir = "%{_PREMAKE_DIR}/Engine"
-- Engine SDK directory
sdkdir = "%{_PREMAKE_DIR}/Engine/sdk/inc"
-- Engine Runtime directory
rundir = "%{_PREMAKE_DIR}/Engine/runtime"

-- Libs directory
libdir = "%{_PREMAKE_DIR}/Libs"

-- Samples directory
samdir = "%{_PREMAKE_DIR}/Samples"

-- Tool directory
toldir = "%{_PREMAKE_DIR}/Tools"

-- Vendor directory
vendir = "%{_PREMAKE_DIR}/Vendor"

bindir = "%{_PREMAKE_DIR}/bin/%{cfg.buildcfg}"
intdir = "%{_PREMAKE_DIR}/bin-int/%{cfg.buildcfg}"

workspace "Jupiter-Engine"
	include "Engine"

workspace "Jupiter-Libs"
	include "Libs"
	
-- Generate samples in separate folders
include "Samples"