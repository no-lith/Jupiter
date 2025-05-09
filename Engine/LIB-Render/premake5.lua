project "LIB-Render"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"
	warnings "Extra"

	characterset ("ASCII")

	pchheader "precompile.h"
	pchsource "%{rundir}/render_a/src/sys/d3d/precompile.cpp"

	targetdir (bindir)
	objdir (intdir .. "/%{prj.name}")

	libdirs {bindir.. "/**"}

	defines { "__D3D", "__D3DREND", "MODEL_SUPPORT_ABC", "D3D_DEBUG_INFO" }

	files
	{
		-- for future additions to the project itself
		"src/**.h",
		"src/**.cpp",

		sdkdir .. "/iclientshell.h",
		sdkdir .. "/iltbenchmark.h",
		sdkdir .. "/iltclient.h",
		sdkdir .. "/iltcommon.h",
		sdkdir .. "/iltcsbase.h",
		sdkdir .. "/iltdirectmusic.h",
		sdkdir .. "/iltdrawprim.h",
		sdkdir .. "/iltinfo.h",
		sdkdir .. "/iltmath.h",
		sdkdir .. "/iltrenderstyles.h",
		sdkdir .. "/iltsoundmgr.h",
		sdkdir .. "/iltspritecontrol.h",
		sdkdir .. "/iltstream.h",
		sdkdir .. "/ilttexinterface.h",
		sdkdir .. "/lithtech.h",
		sdkdir .. "/ltanimtracker.h",
		sdkdir .. "/ltassert.h",
		sdkdir .. "/ltbasedefs.h",
		sdkdir .. "/ltbasetypes.h",
		sdkdir .. "/ltcodes.h",
		sdkdir .. "/ltinteger.h",
		sdkdir .. "/ltlink.h",
		sdkdir .. "/ltmatrix.h",
		sdkdir .. "/ltmodule.h",
		sdkdir .. "/ltplane.h",
		sdkdir .. "/ltpvalue.h",
		sdkdir .. "/ltquatbase.h",
		sdkdir .. "/ltrect.h",
		sdkdir .. "/ltrenderstyle.h",
		sdkdir .. "/ltrotation.h",
		sdkdir .. "/ltvector.h",
		rundir .. "/client/src/client_formatmgr.h",
		rundir .. "/kernel/mem/src/de_memory.h",
		rundir .. "/kernel/src/dsys.h",
		rundir .. "/kernel/src/sys/win/counter.h",
		rundir .. "/kernel/src/sys/win/dsys_interface.h",
		rundir .. "/kernel/src/sys/win/interface_helpers.h",
		rundir .. "/kernel/src/sys/win/lthread.h",
		rundir .. "/kernel/src/sys/win/render.h",
		rundir .. "/kernel/src/syscounter.h",
		rundir .. "/model/src/animtracker.h",
		rundir .. "/model/src/ltb.h",
		rundir .. "/model/src/model_ops.h",
		rundir .. "/model/src/model.h",
		rundir .. "/model/src/modelallocations.h",
		rundir .. "/model/src/transformmaker.h",
		rundir .. "/render_a/src/sys/d3d/AABB.h",
		rundir .. "/render_a/src/sys/d3d/clipline.h",
		rundir .. "/render_a/src/sys/d3d/common_draw.h",
		rundir .. "/render_a/src/sys/d3d/common_init.h",
		rundir .. "/render_a/src/sys/d3d/common_stuff.h",
		rundir .. "/render_a/src/sys/d3d/d3d_convar.h",
		rundir .. "/render_a/src/sys/d3d/d3d_defs.h",
		rundir .. "/render_a/src/sys/d3d/d3d_device_wrapper.h",
		rundir .. "/render_a/src/sys/d3d/d3d_device.h",
		rundir .. "/render_a/src/sys/d3d/d3d_draw.h",
		rundir .. "/render_a/src/sys/d3d/d3d_init.h",
		rundir .. "/render_a/src/sys/d3d/d3d_renderblock.h",
		rundir .. "/render_a/src/sys/d3d/d3d_rendershader_base.h",
		rundir .. "/render_a/src/sys/d3d/d3d_rendershader_default.h",
		rundir .. "/render_a/src/sys/d3d/d3d_rendershader_dynamiclight.h",
		rundir .. "/render_a/src/sys/d3d/d3d_rendershader_glow.h",
		rundir .. "/render_a/src/sys/d3d/d3d_rendershader_gouraud_fullbright.h",
		rundir .. "/render_a/src/sys/d3d/d3d_rendershader_gouraud.h",
		rundir .. "/render_a/src/sys/d3d/d3d_rendershader_lightmap.h",
		rundir .. "/render_a/src/sys/d3d/d3d_rendershader.h",
		rundir .. "/render_a/src/sys/d3d/d3d_renderstatemgr.h",
		rundir .. "/render_a/src/sys/d3d/d3d_renderstyle.h",
		rundir .. "/render_a/src/sys/d3d/d3d_renderstyleinterface.h",
		rundir .. "/render_a/src/sys/d3d/d3d_renderworld_occluder.h",
		rundir .. "/render_a/src/sys/d3d/d3d_renderworld.h",
		rundir .. "/render_a/src/sys/d3d/d3d_shell.h",
		rundir .. "/render_a/src/sys/d3d/d3d_surface.h",
		rundir .. "/render_a/src/sys/d3d/d3d_texture.h",
		rundir .. "/render_a/src/sys/d3d/d3d_utils.h",
		rundir .. "/render_a/src/sys/d3d/d3d_viewparams.h",
		rundir .. "/render_a/src/sys/d3d/d3dmeshrendobj_rigid.h",
		rundir .. "/render_a/src/sys/d3d/d3dmeshrendobj_skel.h",
		rundir .. "/render_a/src/sys/d3d/d3dmeshrendobj_vertanim.h",
		rundir .. "/render_a/src/sys/d3d/devicelightlist.h",
		rundir .. "/render_a/src/sys/d3d/dirtyrect.h",
		rundir .. "/render_a/src/sys/d3d/draw_canvas.h",
		rundir .. "/render_a/src/sys/d3d/drawlight.h",
		rundir .. "/render_a/src/sys/d3d/drawobjects.h",
		rundir .. "/render_a/src/sys/d3d/drawsky.h",
		rundir .. "/render_a/src/sys/d3d/drawworldmodel.h",
		rundir .. "/render_a/src/sys/d3d/FixedPoint.h",
		rundir .. "/render_a/src/sys/d3d/iaggregateshader.h",
		rundir .. "/render_a/src/sys/d3d/LTEffectImpl.h",
		rundir .. "/render_a/src/sys/d3d/LTEffectInclude.h",
		rundir .. "/render_a/src/sys/d3d/lteffectshadermgr.h",
		rundir .. "/render_a/src/sys/d3d/ltpixelshadermgr.h",
		rundir .. "/render_a/src/sys/d3d/LTShaderDeviceStateImp.h",
		rundir .. "/render_a/src/sys/d3d/ltvertexshadermgr.h",
		rundir .. "/render_a/src/sys/d3d/memstats_world.h",
		rundir .. "/render_a/src/sys/d3d/modeldebug.h",
		rundir .. "/render_a/src/sys/d3d/modelshadowshader.h",
		rundir .. "/render_a/src/sys/d3d/objectgroupmgr.h",
		rundir .. "/render_a/src/sys/d3d/polyclip.h",
		rundir .. "/render_a/src/sys/d3d/precompile.h",
		rundir .. "/render_a/src/sys/d3d/relevantlightlist.h",
		rundir .. "/render_a/src/sys/d3d/rendererconsolevars.h",
		rundir .. "/render_a/src/sys/d3d/rendererframestats.h",
		rundir .. "/render_a/src/sys/d3d/renderlight.h",
		rundir .. "/render_a/src/sys/d3d/rendermodelinfolist.h",
		rundir .. "/render_a/src/sys/d3d/rendermodelpiecelist.h",
		rundir .. "/render_a/src/sys/d3d/rendershadowlist.h",
		rundir .. "/render_a/src/sys/d3d/renderstylelookuptables.h",
		rundir .. "/render_a/src/sys/d3d/renderstylemap.h",
		rundir .. "/render_a/src/sys/d3d/rendertarget.h",
		rundir .. "/render_a/src/sys/d3d/rendertargetmgr.h",
		rundir .. "/render_a/src/sys/d3d/screenglowmgr.h",
		rundir .. "/render_a/src/sys/d3d/setupmodel.h",
		rundir .. "/render_a/src/sys/d3d/setuptouchinglights.h",
		rundir .. "/render_a/src/sys/d3d/SimD_Utilities.h",
		rundir .. "/render_a/src/sys/d3d/tagnodes.h",
		rundir .. "/render_a/src/sys/d3d/texturescriptdefs.h",
		rundir .. "/render_a/src/sys/d3d/texturescriptevaluator.h",
		rundir .. "/render_a/src/sys/d3d/texturescriptinstance.h",
		rundir .. "/render_a/src/sys/d3d/texturescriptinterpreter.h",
		rundir .. "/render_a/src/sys/d3d/texturescriptmgr.h",
		rundir .. "/render_a/src/sys/d3d/texturescriptvarmgr.h",
		rundir .. "/render_a/src/sys/d3d/VertexBufferController.h",
		rundir .. "/render_a/src/sys/d3d/3d_ops.h",
		rundir .. "/render_a/src/sys/shadows/d3dshadowtexture.h",
		rundir .. "/shared/src/bdefs.h",
		rundir .. "/shared/src/colorops.h",
		rundir .. "/shared/src/conparse.h",
		rundir .. "/shared/src/DebugGeometry.h",
		rundir .. "/shared/src/dtxmgr.h",
		rundir .. "/shared/src/gamemath.h",
		rundir .. "/shared/src/geometry.h",
		rundir .. "/shared/src/geomroutines.h",
		rundir .. "/shared/src/lightmap_compress.h",
		rundir .. "/shared/src/nexus.h",
		rundir .. "/shared/src/objectmgr.h",
		rundir .. "/shared/src/pixelformat.h",
		rundir .. "/shared/src/renderinfostruct.h",
		rundir .. "/shared/src/RenderObject.h",
		rundir .. "/shared/src/sys/win/d3dddstructs.h",
		rundir .. "/shared/src/sys/win/renderstruct.h",
		rundir .. "/shared/src/sysddstructs.h",
		rundir .. "/shared/src/version_info.h",
		rundir .. "/world/src/de_mainworld.h",
		rundir .. "/world/src/de_objects.h",
		rundir .. "/world/src/de_sprite.h",
		rundir .. "/world/src/de_world.h",
		rundir .. "/world/src/fullintersectline.h",
		rundir .. "/world/src/intersect_line.h",
		rundir .. "/world/src/light_table.h",
		rundir .. "/world/src/visit_pvs.h",
		rundir .. "/world/src/world_tree.h",
		rundir .. "/world/src/worldtreehelper.h",

		rundir .. "/render_a/src/sys/d3d/3d_ops.cpp",
		rundir .. "/render_a/src/sys/d3d/AABB.cpp",
		rundir .. "/render_a/src/sys/d3d/common_draw.cpp",
		rundir .. "/render_a/src/sys/d3d/common_init.cpp",
		rundir .. "/render_a/src/sys/d3d/common_stuff.cpp",
		rundir .. "/render_a/src/sys/d3d/d3d_device_wrapper.cpp",
		rundir .. "/render_a/src/sys/d3d/d3d_device.cpp",
		rundir .. "/render_a/src/sys/d3d/d3d_draw.cpp",
		rundir .. "/render_a/src/sys/d3d/d3d_drawsky.cpp",
		rundir .. "/render_a/src/sys/d3d/d3d_init.cpp",
		rundir .. "/render_a/src/sys/d3d/d3d_optimizedsurface.cpp",
		rundir .. "/render_a/src/sys/d3d/d3d_renderblock.cpp",
		rundir .. "/render_a/src/sys/d3d/d3d_rendershader_dynamiclight.cpp",
		rundir .. "/render_a/src/sys/d3d/d3d_rendershader_glow.cpp",
		rundir .. "/render_a/src/sys/d3d/d3d_rendershader_gouraud_fullbright.cpp",
		rundir .. "/render_a/src/sys/d3d/d3d_rendershader_gouraud.cpp",
		rundir .. "/render_a/src/sys/d3d/d3d_rendershader_lightmap.cpp",
		rundir .. "/render_a/src/sys/d3d/d3d_renderstatemgr.cpp",
		rundir .. "/render_a/src/sys/d3d/d3d_renderstyle.cpp",
		rundir .. "/render_a/src/sys/d3d/d3d_renderstyleinterface.cpp",
		rundir .. "/render_a/src/sys/d3d/d3d_renderworld_occluder.cpp",
		rundir .. "/render_a/src/sys/d3d/d3d_renderworld.cpp",
		rundir .. "/render_a/src/sys/d3d/d3d_shell.cpp",
		rundir .. "/render_a/src/sys/d3d/d3d_surface.cpp",
		rundir .. "/render_a/src/sys/d3d/d3d_texture.cpp",
		rundir .. "/render_a/src/sys/d3d/d3d_utils.cpp",
		rundir .. "/render_a/src/sys/d3d/d3d_viewparams.cpp",
		rundir .. "/render_a/src/sys/d3d/d3dmeshrendobj_rigid.cpp",
		rundir .. "/render_a/src/sys/d3d/d3dmeshrendobj_skel.cpp",
		rundir .. "/render_a/src/sys/d3d/d3dmeshrendobj_vertanim.cpp",
		rundir .. "/render_a/src/sys/d3d/devicelightlist.cpp",
		rundir .. "/render_a/src/sys/d3d/dirtyrect.cpp",
		rundir .. "/render_a/src/sys/d3d/draw_canvas.cpp",
		rundir .. "/render_a/src/sys/d3d/drawlight.cpp",
		rundir .. "/render_a/src/sys/d3d/drawlinesystem.cpp",
		rundir .. "/render_a/src/sys/d3d/drawmodel.cpp",
		rundir .. "/render_a/src/sys/d3d/drawobjects.cpp",
		rundir .. "/render_a/src/sys/d3d/drawparticles_A.cpp",
		rundir .. "/render_a/src/sys/d3d/drawparticles.cpp",
		rundir .. "/render_a/src/sys/d3d/drawpolygrid.cpp",
		rundir .. "/render_a/src/sys/d3d/drawsky.cpp",
		rundir .. "/render_a/src/sys/d3d/drawsprite.cpp",
		rundir .. "/render_a/src/sys/d3d/drawvolumeeffects.cpp",
		rundir .. "/render_a/src/sys/d3d/drawworldmodel.cpp",
		rundir .. "/render_a/src/sys/d3d/LTEffectImpl.cpp",
		rundir .. "/render_a/src/sys/d3d/LTEffectInclude.cpp",
		rundir .. "/render_a/src/sys/d3d/lteffectshadermgr.cpp",
		rundir .. "/render_a/src/sys/d3d/ltpixelshadermgr.cpp",
		rundir .. "/render_a/src/sys/d3d/LTShaderDeviceStateImp.cpp",
		rundir .. "/render_a/src/sys/d3d/ltvertexshadermgr.cpp",
		rundir .. "/render_a/src/sys/d3d/memstats_world.cpp",
		rundir .. "/render_a/src/sys/d3d/modeldebug.cpp",
		rundir .. "/render_a/src/sys/d3d/modelshadowshader.cpp",
		rundir .. "/render_a/src/sys/d3d/objectgroupmgr.cpp",
		rundir .. "/render_a/src/sys/d3d/precompile.cpp",
		rundir .. "/render_a/src/sys/d3d/relevantlightlist.cpp",
		rundir .. "/render_a/src/sys/d3d/rendererconsolevars.cpp",
		rundir .. "/render_a/src/sys/d3d/rendererframestats.cpp",
		rundir .. "/render_a/src/sys/d3d/renderlight.cpp",
		rundir .. "/render_a/src/sys/d3d/rendermodelinfolist.cpp",
		rundir .. "/render_a/src/sys/d3d/rendermodelpiecelist.cpp",
		rundir .. "/render_a/src/sys/d3d/rendershadowlist.cpp",
		rundir .. "/render_a/src/sys/d3d/renderstylelookuptables.cpp",
		rundir .. "/render_a/src/sys/d3d/renderstylemap.cpp",
		rundir .. "/render_a/src/sys/d3d/rendertarget.cpp",
		rundir .. "/render_a/src/sys/d3d/rendertargetmgr.cpp",
		rundir .. "/render_a/src/sys/d3d/screenglowmgr.cpp",
		rundir .. "/render_a/src/sys/d3d/setupmodel.cpp",
		rundir .. "/render_a/src/sys/d3d/setuptouchinglights.cpp",
		rundir .. "/render_a/src/sys/d3d/tagnodes.cpp",
		rundir .. "/render_a/src/sys/d3d/texturescriptinstance.cpp",
		rundir .. "/render_a/src/sys/d3d/texturescriptinterpreter.cpp",
		rundir .. "/render_a/src/sys/d3d/texturescriptmgr.cpp",
		rundir .. "/render_a/src/sys/d3d/texturescriptvarmgr.cpp",
		rundir .. "/render_a/src/sys/d3d/VertexBufferController.cpp",
		rundir .. "/render_a/src/sys/shadows/d3dshadowtexture.cpp"
	}

	includedirs
	{
		"src",

		-- sdk
		sdkdir,
		sdkdir .. "/physics",

		-- runtime
		rundir,
		rundir .. "/client/src",
		rundir .. "/kernel/io/src",
		rundir .. "/kernel/mem/src",
		rundir .. "/kernel/src",
		rundir .. "/kernel/src/sys/win",
		rundir .. "/lithtemplate",
		rundir .. "/model/src",
		rundir .. "/model/src/sys/d3d",
		rundir .. "/render_a/src/sys/d3d",
		rundir .. "/shared/src",
		rundir .. "/shared/src/sys/win",
		rundir .. "/world/src",
		
		-- external libs
		libdir .. "/LIB-STDLith/src"
	}

	-- DX9 / DX8 includes
	externalincludedirs
	{
		"$(DXSDK_DIR)Include"
	}

	-- DX9 / DX8 library
	syslibdirs
	{
		"$(DXSDK_DIR)Lib/x86"
	}

	links
	{
		"LIB-STDLith"
	}

	filter { "files:**/runtime/render_a/src/sys/d3d/texturescriptinterpreter.cpp" }
		flags "NoPCH"