project "Server"
	kind "SharedLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"

	characterset ("ASCII")

	pchheader "bdefs.h"
	pchsource "%{rundir}/shared/src/bdefs.cpp"

	targetdir (bindir)
	objdir (intdir .. "/%{prj.name}")

	libdirs {bindir.. "/**"}

	defines { "__D3D", "__D3DREND", "_USRDLL", "DE_SERVER_COMPILE", "DIRECTENGINE_COMPILE", "STDLITH_ALLOC_OVERRIDE", "LT15_COMPAT" }

	files
	{
		-- for future additions to the project itself
		"src/**.h",
		"src/**.cpp",

		libdir .. "/LIB-RezMgr/src/rezfile.h",
		libdir .. "/LIB-RezMgr/src/rezhash.h",
		libdir .. "/LIB-RezMgr/src/rezmgr.h",
		libdir .. "/LIB-RezMgr/src/reztypes.h",
		sdkdir .. "/iaggregate.h",
		sdkdir .. "/iclientshell.h",
		sdkdir .. "/iltbenchmark.h",
		sdkdir .. "/iltclient.h",
		sdkdir .. "/iltcommon.h",
		sdkdir .. "/iltcsbase.h",
		sdkdir .. "/iltdirectmusic.h",
		sdkdir .. "/iltdrawprim.h",
		sdkdir .. "/iltinfo.h",
		sdkdir .. "/iltmessage.h",
		sdkdir .. "/iltmodel.h",
		sdkdir .. "/iltphysics.h",
		sdkdir .. "/iltrefcount.h",
		sdkdir .. "/iltserver.h",
		sdkdir .. "/iltsoundmgr.h",
		sdkdir .. "/iltspritecontrol.h",
		sdkdir .. "/iltstream.h",
		sdkdir .. "/ilttexinterface.h",
		sdkdir .. "/ilttransform.h",
		sdkdir .. "/iservershell.h",
		sdkdir .. "/lithtech.h",
		sdkdir .. "/ltanimtracker.h",
		sdkdir .. "/ltassert.h",
		sdkdir .. "/ltbasedefs.h",
		sdkdir .. "/ltbasetypes.h",
		sdkdir .. "/ltcodes.h",
		sdkdir .. "/ltengineobjects.h",
		sdkdir .. "/ltinteger.h",
		sdkdir .. "/ltlink.h",
		sdkdir .. "/ltmatrix.h",
		sdkdir .. "/ltmodule.h",
		sdkdir .. "/ltplane.h",
		sdkdir .. "/ltpvalue.h",
		sdkdir .. "/ltquatbase.h",
		sdkdir .. "/ltrect.h",
		sdkdir .. "/ltrenderstyle.h",
		sdkdir .. "/ltserverobj.h",
		sdkdir .. "/ltvector.h",
		sdkdir .. "/physics/aabb_tree.h",
		sdkdir .. "/physics/aabb.h",
		sdkdir .. "/physics/collision_data.h",
		sdkdir .. "/physics/collision_mgr.h",
		sdkdir .. "/physics/collision_object.h",
		sdkdir .. "/physics/coordinate_frame.h",
		sdkdir .. "/physics/math_phys.h",
		sdkdir .. "/physics/matrix.h",
		sdkdir .. "/physics/quaternion.h",
		sdkdir .. "/physics/triangle.h",
		sdkdir .. "/physics/vector.h",
		sdkdir .. "/server_interface.h",
		rundir .. "/client/src/client_filemgr.h",
		rundir .. "/client/src/clientmgr.h",
		rundir .. "/client/src/clientshell.h",
		rundir .. "/client/src/console.h",
		rundir .. "/client/src/predict.h",
		rundir .. "/client/src/setupobject.h",
		rundir .. "/kernel/io/src/sys/win/de_file.h",
		rundir .. "/kernel/io/src/sysfile.h",
		rundir .. "/kernel/mem/src/de_memory.h",
		rundir .. "/kernel/net/src/localdriver.h",
		rundir .. "/kernel/net/src/netmgr.h",
		rundir .. "/kernel/net/src/sys/linux/linux_ltthread.h",
		rundir .. "/kernel/net/src/sys/linux/linux_ltthreadevent.h",
		rundir .. "/kernel/net/src/sys/win/udpdriver.h",
		rundir .. "/kernel/net/src/sys/win/win32_ltthread.h",
		rundir .. "/kernel/net/src/sys/win/win32_ltthreadevent.h",
		rundir .. "/kernel/net/src/syssocket.h",
		rundir .. "/kernel/net/src/sysudpdriver.h",
		rundir .. "/kernel/net/src/sysudpthread.h",
		rundir .. "/kernel/src/dsys.h",
		rundir .. "/kernel/src/ithread.h",
		rundir .. "/kernel/src/sys/win/appresource.h",
		rundir .. "/kernel/src/sys/win/bindmgr.h",
		rundir .. "/kernel/src/sys/win/counter.h",
		rundir .. "/kernel/src/sys/win/dsys_interface.h",
		rundir .. "/kernel/src/sys/win/dutil.h",
		rundir .. "/kernel/src/sys/win/lthread.h",
		rundir .. "/kernel/src/sys/win/ltthread.h",
		rundir .. "/kernel/src/sys/win/render.h",
		rundir .. "/kernel/src/sys/win/streamsim.h",
		rundir .. "/kernel/src/sys/win/stringmgr.h",
		rundir .. "/kernel/src/sys/win/timemgr.h",
		rundir .. "/kernel/src/sys/win/version_resource.h",
		rundir .. "/kernel/src/syscounter.h",
		rundir .. "/kernel/src/syslibraryloader.h",
		rundir .. "/kernel/src/syslthread.h",
		rundir .. "/kernel/src/sysstreamsim.h",
		rundir .. "/kernel/src/syssysteminfo.h",
		rundir .. "/kernel/src/systhread.h",
		rundir .. "/kernel/src/systimer.h",
		rundir .. "/model/src/animtracker.h",
		rundir .. "/model/src/ltb.h",
		rundir .. "/model/src/model_ops.h",
		rundir .. "/model/src/model.h",
		rundir .. "/model/src/modelallocations.h",
		rundir .. "/model/src/transformmaker.h",
		rundir .. "/server/src/classmgr.h",
		rundir .. "/server/src/game_serialize.h",
		rundir .. "/server/src/interlink.h",
		rundir .. "/server/src/ltmessage_server.h",
		rundir .. "/server/src/s_client.h",
		rundir .. "/server/src/s_concommand.h",
		rundir .. "/server/src/s_net.h",
		rundir .. "/server/src/s_object.h",
		rundir .. "/server/src/server_consolestate.h",
		rundir .. "/server/src/server_extradata.h",
		rundir .. "/server/src/server_filemgr.h",
		rundir .. "/server/src/server_loaderthread.h",
		rundir .. "/server/src/serverde_impl.h",
		rundir .. "/server/src/serverevent.h",
		rundir .. "/server/src/serverexception.h",
		rundir .. "/server/src/servermgr.h",
		rundir .. "/server/src/serverobj.h",
		rundir .. "/server/src/sloaderthread.h",
		rundir .. "/server/src/smoveabstract.h",
		rundir .. "/server/src/soundtrack.h",
		rundir .. "/shared/src/bdefs.h",
		rundir .. "/shared/src/build_options.h",
		rundir .. "/shared/src/classbind.h",
		rundir .. "/shared/src/clienthack.h",
		rundir .. "/shared/src/collision.h",
		rundir .. "/shared/src/concommand.h",
		rundir .. "/shared/src/conparse.h",
		rundir .. "/shared/src/DebugGeometry.h",
		rundir .. "/shared/src/dhashtable.h",
		rundir .. "/shared/src/dtxmgr.h",
		rundir .. "/shared/src/ftbase.h",
		rundir .. "/shared/src/ftserv.h",
		rundir .. "/shared/src/gamemath.h",
		rundir .. "/shared/src/genericprop_setup.h",
		rundir .. "/shared/src/genltstream.h",
		rundir .. "/shared/src/geometry.h",
		rundir .. "/shared/src/geomroutines.h",
		rundir .. "/shared/src/impl_common.h",
		rundir .. "/shared/src/lightmap_planes.h",
		rundir .. "/shared/src/listqueue.h",
		rundir .. "/shared/src/ltmessage.h",
		rundir .. "/shared/src/lttimer.h",
		rundir .. "/shared/src/motion.h",
		rundir .. "/shared/src/moveobject.h",
		rundir .. "/shared/src/moveplayer.h",
		rundir .. "/shared/src/musicmgr.h",
		rundir .. "/shared/src/nexus.h",
		rundir .. "/shared/src/objectmgr.h",
		rundir .. "/shared/src/packetdefs.h",
		rundir .. "/shared/src/parse_world_info.h",
		rundir .. "/shared/src/pixelformat.h",
		rundir .. "/shared/src/ratetracker.h",
		rundir .. "/shared/src/refobj.h",
		rundir .. "/shared/src/renderinfostruct.h",
		rundir .. "/shared/src/RenderObject.h",
		rundir .. "/shared/src/shared_iltcommon.h",
		rundir .. "/shared/src/stacktrace.h",
		rundir .. "/shared/src/staticfifo.h",
		rundir .. "/shared/src/stdlterror.h",
		rundir .. "/shared/src/sys/win/d3dddstructs.h",
		rundir .. "/shared/src/sys/win/renderstruct.h",
		rundir .. "/shared/src/sys/win/shellbind.h",
		rundir .. "/shared/src/sysddstructs.h",
		rundir .. "/shared/src/version_info.h",
		rundir .. "/sound/src/iltsound.h",
		rundir .. "/sound/src/soundbuffer.h",
		rundir .. "/sound/src/sounddata.h",
		rundir .. "/sound/src/soundinstance.h",
		rundir .. "/sound/src/wave.h",
		rundir .. "/world/src/de_mainworld.h",
		rundir .. "/world/src/de_objects.h",
		rundir .. "/world/src/de_sprite.h",
		rundir .. "/world/src/de_world.h",
		rundir .. "/world/src/fullintersectline.h",
		rundir .. "/world/src/intersect_line.h",
		rundir .. "/world/src/light_table.h",
		rundir .. "/world/src/loadstatus.h",
		rundir .. "/world/src/visit_pvs.h",
		rundir .. "/world/src/world_blind_object_data.h",
		rundir .. "/world/src/world_blocker_data.h",
		rundir .. "/world/src/world_client_bsp.h",
		rundir .. "/world/src/world_client.h",
		rundir .. "/world/src/world_interface.h",
		rundir .. "/world/src/world_particle_blocker_data.h",
		rundir .. "/world/src/world_server_bsp.h",
		rundir .. "/world/src/world_server.h",
		rundir .. "/world/src/world_shared_bsp.h",
		rundir .. "/world/src/world_tree.h",
		rundir .. "/world/src/worldtreehelper.h",

		sdkdir .. "/ltmodule.cpp",
		sdkdir .. "/ltobjref.cpp",
		sdkdir .. "/ltquatbase.cpp",
		rundir .. "/kernel/io/src/sys/win/de_file.cpp",
		rundir .. "/kernel/mem/src/sys/win/de_memory.cpp",
		rundir .. "/kernel/net/src/localdriver.cpp",
		rundir .. "/kernel/net/src/netmgr.cpp",
		rundir .. "/kernel/net/src/packet.cpp",
		rundir .. "/kernel/net/src/sys/win/udpdriver.cpp",
		rundir .. "/kernel/src/debugging.cpp",
		rundir .. "/kernel/src/server_interface.cpp",
		rundir .. "/kernel/src/sys/win/bindmgr.cpp",
		rundir .. "/kernel/src/sys/win/counter.cpp",
		rundir .. "/kernel/src/sys/win/dsys_interface.cpp",
		rundir .. "/kernel/src/sys/win/dutil.cpp",
		rundir .. "/kernel/src/sys/win/lthread.cpp",
		rundir .. "/kernel/src/sys/win/ltlibraryloader.cpp",
		rundir .. "/kernel/src/sys/win/ltthread.cpp",
		rundir .. "/kernel/src/sys/win/streamsim.cpp",
		rundir .. "/kernel/src/sys/win/stringmgr.cpp",
		rundir .. "/kernel/src/sys/win/systeminfo.cpp",
		rundir .. "/kernel/src/sys/win/timemgr.cpp",
		rundir .. "/model/src/animtracker.cpp",
		rundir .. "/model/src/model.cpp",
		rundir .. "/model/src/modelallocations.cpp",
		rundir .. "/model/src/model_load.cpp",
		rundir .. "/model/src/sys/d3d/d3d_model_load.cpp",
		rundir .. "/model/src/transformmaker.cpp",
		rundir .. "/server/src/classmgr.cpp",
		rundir .. "/server/src/game_serialize.cpp",
		rundir .. "/server/src/interlink.cpp",
		rundir .. "/server/src/ltmessage_server.cpp",
		rundir .. "/server/src/serverde_impl.cpp",
		rundir .. "/server/src/serverevent.cpp",
		rundir .. "/server/src/servermgr.cpp",
		rundir .. "/server/src/server_consolestate.cpp",
		rundir .. "/server/src/server_extradata.cpp",
		rundir .. "/server/src/server_filemgr.cpp",
		rundir .. "/server/src/server_iltcommon.cpp",
		rundir .. "/server/src/server_iltmodel.cpp",
		rundir .. "/server/src/server_iltphysics.cpp",
		rundir .. "/server/src/server_iltsoundmgr.cpp",
		rundir .. "/server/src/server_loaderthread.cpp",
		rundir .. "/server/src/sloaderthread.cpp",
		rundir .. "/server/src/smoveabstract.cpp",
		rundir .. "/server/src/soundtrack.cpp",
		rundir .. "/server/src/s_client.cpp",
		rundir .. "/server/src/s_concommand.cpp",
		rundir .. "/server/src/s_intersect.cpp",
		rundir .. "/server/src/s_net.cpp",
		rundir .. "/server/src/s_object.cpp",
		rundir .. "/server/src/world_server_bsp.cpp",
		rundir .. "/shared/src/bdefs.cpp",
		rundir .. "/shared/src/classbind.cpp",
		rundir .. "/shared/src/collision.cpp",
		rundir .. "/shared/src/compress.cpp",
		rundir .. "/shared/src/concommand.cpp",
		rundir .. "/shared/src/conparse.cpp",
		rundir .. "/shared/src/dhashtable.cpp",
		rundir .. "/shared/src/engine_vars.cpp",
		rundir .. "/shared/src/findobj.cpp",
		rundir .. "/shared/src/ftserv.cpp",
		rundir .. "/shared/src/gamemath.cpp",
		rundir .. "/shared/src/genericprop_setup.cpp",
		rundir .. "/shared/src/genltstream.cpp",
		rundir .. "/shared/src/geometry.cpp",
		rundir .. "/shared/src/geomroutines.cpp",
		rundir .. "/shared/src/impl_common.cpp",
		rundir .. "/shared/src/interface_linkage.cpp",
		rundir .. "/shared/src/leech.cpp",
		rundir .. "/shared/src/lightmap_planes.cpp",
		rundir .. "/shared/src/ltmessage.cpp",
		rundir .. "/shared/src/lttimer.cpp",
		rundir .. "/shared/src/modellt_impl.cpp",
		rundir .. "/shared/src/motion.cpp",
		rundir .. "/shared/src/moveobject.cpp",
		rundir .. "/shared/src/moveplayer.cpp",
		rundir .. "/shared/src/nexus.cpp",
		rundir .. "/shared/src/objectmgr.cpp",
		rundir .. "/shared/src/parse_world_info.cpp",
		rundir .. "/shared/src/ratetracker.cpp",
		rundir .. "/shared/src/shared_iltcommon.cpp",
		rundir .. "/shared/src/shared_iltphysics.cpp",
		rundir .. "/shared/src/spritecontrolimpl.cpp",
		rundir .. "/shared/src/stacktrace.cpp",
		rundir .. "/shared/src/stdlterror.cpp",
		rundir .. "/shared/src/strtools.cpp",
		rundir .. "/shared/src/transformlt_impl.cpp",
		rundir .. "/sound/src/sounddata.cpp",
		rundir .. "/sound/src/wave.cpp",
		rundir .. "/world/src/de_mainworld.cpp",
		rundir .. "/world/src/de_nodes.cpp",
		rundir .. "/world/src/de_objects.cpp",
		rundir .. "/world/src/fullintersectline.cpp",
		rundir .. "/world/src/intersectsweptsphere.cpp",
		rundir .. "/world/src/intersect_line.cpp",
		rundir .. "/world/src/light_table.cpp",
		rundir .. "/world/src/world_blind_object_data.cpp",
		rundir .. "/world/src/world_blocker_data.cpp",
		rundir .. "/world/src/world_blocker_math.cpp",
		rundir .. "/world/src/world_particle_blocker_data.cpp",
		rundir .. "/world/src/world_shared_bsp.cpp",
		rundir .. "/world/src/world_tree.cpp"
	}

	includedirs
	{
		"src",

		-- sdk
		sdkdir,
		sdkdir .. "/physics",
		sdkdir .. "/state_mgr",

		-- runtime
		rundir,
		rundir .. "/client/src",
		rundir .. "/kernel/io/src",
		rundir .. "/kernel/mem/src",
		rundir .. "/kernel/net",
		rundir .. "/kernel/net/src",
		rundir .. "/kernel/src",
		rundir .. "/kernel/src/sys/win",
		rundir .. "/lithtemplate",
		rundir .. "/model/src",
		rundir .. "/physics/src",
		rundir .. "/server/src",
		rundir .. "/shared/src",
		rundir .. "/shared/src/sys/win",
		rundir .. "/sound/src",
		rundir .. "/world/src",
		
		-- external libs
		libdir .. "/LIB-Info/src",
		libdir .. "/LIB-Lith/src",
		libdir .. "/LIB-LTMem/src",
		libdir .. "/LIB-RezMgr/src",
		libdir .. "/LIB-STDLith/src"
	}

	-- DX8 includes
	externalincludedirs
	{
		vendir
	}

	-- DX8 library
	syslibdirs
	{
		vendir .. "/directx-8.1/lib"
	}

	links
	{
		"winmm",
		"dxguid",
		"dplayx",
		"odbc32",
		"odbccp32",

		"LIB-Info",

		"LIB-Lith",
		"LIB-LTMem",
		"LIB-RezMgr",
		"LIB-STDLith"
	}

	filter { "files:**/sdk/inc/ltmodule.cpp" }
		flags "NoPCH"

	filter { "files:**/sdk/inc/ltobjref.cpp" }
		flags "NoPCH"

	filter { "files:**/sdk/inc/ltquatbase.cpp" }
		flags "NoPCH"

	filter { "files:**/runtime/kernel/src/server_interface.cpp" }
		flags "NoPCH"

	filter { "files:**/runtime/sound/src/sounddata.cpp" }
		flags "NoPCH"

	filter { "files:**/runtime/kernel/src/sys/win/stringmgr.cpp" }
		flags "NoPCH"

	filter { "files:**/runtime/sound/src/wave.cpp" }
		flags "NoPCH"

	filter { "files:**/runtime/kernel/src/sys/win/counter.cpp" }
		flags "NoPCH"

	filter { "files:**/runtime/kernel/io/src/sys/win/de_file.cpp" }
		flags "NoPCH"

	filter { "files:**/runtime/kernel/mem/src/sys/win/de_memory.cpp" }
		flags "NoPCH"

	filter { "files:**/runtime/kernel/src/sys/win/dsys_interface.cpp" }
		flags "NoPCH"

	filter { "files:**/runtime/kernel/src/sys/win/lthread.cpp" }
		flags "NoPCH"

	filter { "files:**/runtime/kernel/src/sys/win/streamsim.cpp" }
		flags "NoPCH"

	filter { "files:**/runtime/kernel/src/sys/win/timemgr.cpp" }
		flags "NoPCH"

	filter { "files:**/runtime/shared/src/interface_linkage.cpp" }
		flags "NoPCH"