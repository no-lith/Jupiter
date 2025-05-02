project "LIB-UI"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"

	characterset ("ASCII")

	targetdir (bindir)
	objdir (intdir .. "/%{prj.name}")

	libdirs {bindir.. "/**"}

	defines { "__D3D", "__D3DREND" }

	files
	{
		-- for future additions to the project itself
		"src/**.h",
		"src/**.cpp",

		sdkdir .. "/cui.h",
		sdkdir .. "/cuibase.h",
		sdkdir .. "/cuibutton.h",
		sdkdir .. "/cuicheck.h",
		sdkdir .. "/cuidropdownlist.h",
		sdkdir .. "/cuifont.h",
		sdkdir .. "/cuiformattedpolystring.h",
		sdkdir .. "/cuiinterval.h",
		sdkdir .. "/cuilist.h",
		sdkdir .. "/cuimessage.h",
		sdkdir .. "/cuioption.h",
		sdkdir .. "/cuipolystring.h",
		sdkdir .. "/cuiprogress.h",
		sdkdir .. "/cuislider.h",
		sdkdir .. "/cuistaticimage.h",
		sdkdir .. "/cuistatictext.h",
		sdkdir .. "/cuitypes.h",
		sdkdir .. "/cuiwidget.h",
		sdkdir .. "/cuiwindow.h",
		sdkdir .. "/iltfontmanager.h",
		sdkdir .. "/iltwidgetmanager.h",
		rundir .. "/ui/src/cuibase_impl.h",
		rundir .. "/ui/src/cuibitmapfont.h",
		rundir .. "/ui/src/cuibutton_impl.h",
		rundir .. "/ui/src/cuicheck_impl.h",
		rundir .. "/ui/src/cuidebug.h",
		rundir .. "/ui/src/cuidropdownlist_impl.h",
		rundir .. "/ui/src/cuifontmanager.h",
		rundir .. "/ui/src/cuifont_impl.h",
		rundir .. "/ui/src/cuiformattedpolystring_impl.h",
		rundir .. "/ui/src/cuiinterval_impl.h",
		rundir .. "/ui/src/cuilinklist.h",
		rundir .. "/ui/src/cuilist_impl.h",
		rundir .. "/ui/src/cuimessagequeue.h",
		rundir .. "/ui/src/cuioption_impl.h",
		rundir .. "/ui/src/cuipolystring_impl.h",
		rundir .. "/ui/src/cuipolytex.h",
		rundir .. "/ui/src/cuiprogress_impl.h",
		rundir .. "/ui/src/cuiredblack.h",
		rundir .. "/ui/src/cuirenderstate.h",
		rundir .. "/ui/src/cuislider_impl.h",
		rundir .. "/ui/src/cuistaticimage_impl.h",
		rundir .. "/ui/src/cuistatictext_impl.h",
		rundir .. "/ui/src/cuivectorfont.h",
		rundir .. "/ui/src/cuiwidgetmanager.h",
		rundir .. "/ui/src/cuiwidget_impl.h",
		rundir .. "/ui/src/cuiwindow_impl.h",

		-- runtime sources
		rundir .. "/ui/src/cuibase.cpp",
		rundir .. "/ui/src/cuibase_impl.cpp",
		rundir .. "/ui/src/cuibitmapfont.cpp",
		rundir .. "/ui/src/cuibutton.cpp",
		rundir .. "/ui/src/cuibutton_impl.cpp",
		rundir .. "/ui/src/cuicheck.cpp",
		rundir .. "/ui/src/cuicheck_impl.cpp",
		rundir .. "/ui/src/cuidropdownlist.cpp",
		rundir .. "/ui/src/cuidropdownlist_impl.cpp",
		rundir .. "/ui/src/cuifontmanager.cpp",
		rundir .. "/ui/src/cuifont_impl.cpp",
		rundir .. "/ui/src/cuiformattedpolystring.cpp",
		rundir .. "/ui/src/cuiformattedpolystring_impl.cpp",
		rundir .. "/ui/src/cuiinterval.cpp",
		rundir .. "/ui/src/cuiinterval_impl.cpp",
		rundir .. "/ui/src/cuilinklist.cpp",
		rundir .. "/ui/src/cuilist.cpp",
		rundir .. "/ui/src/cuilist_impl.cpp",
		rundir .. "/ui/src/cuimessagequeue.cpp",
		rundir .. "/ui/src/cuioption.cpp",
		rundir .. "/ui/src/cuioption_impl.cpp",
		rundir .. "/ui/src/cuipolystring.cpp",
		rundir .. "/ui/src/cuipolystring_impl.cpp",
		rundir .. "/ui/src/cuipolytex.cpp",
		rundir .. "/ui/src/cuiprogress.cpp",
		rundir .. "/ui/src/cuiprogress_impl.cpp",
		rundir .. "/ui/src/cuiredblack.cpp",
		rundir .. "/ui/src/cuirenderstate.cpp",
		rundir .. "/ui/src/cuislider.cpp",
		rundir .. "/ui/src/cuislider_impl.cpp",
		rundir .. "/ui/src/cuistaticimage.cpp",
		rundir .. "/ui/src/cuistaticimage_impl.cpp",
		rundir .. "/ui/src/cuistatictext.cpp",
		rundir .. "/ui/src/cuistatictext_impl.cpp",
		rundir .. "/ui/src/cuivectorfont.cpp",
		rundir .. "/ui/src/cuiwidget.cpp",
		rundir .. "/ui/src/cuiwidgetmanager.cpp",
		rundir .. "/ui/src/cuiwidget_impl.cpp",
		rundir .. "/ui/src/cuiwindow.cpp",
		rundir .. "/ui/src/cuiwindow_impl.cpp",
		rundir .. "/ui/src/sys/d3d/cuidebug.cpp"
	}

	includedirs
	{
		"src",

		-- sdk
		sdkdir,

		-- runtime
		rundir,
		rundir .. "/client/src",
		rundir .. "/controlfilemgr",
		rundir .. "/kernel/io/src",
		rundir .. "/kernel/mem/src",
		rundir .. "/kernel/net/src",
		rundir .. "/kernel/src",
		rundir .. "/kernel/src/sys/win",
		rundir .. "/lithtemplate",
		rundir .. "/model/src",
		rundir .. "/render_b/src",
		rundir .. "/server/src",
		rundir .. "/shared/src",
		rundir .. "/shared/src/sys/win",
		rundir .. "/sound/src",
		rundir .. "/ui/src",
		rundir .. "/world/src,",
		
		-- external libs
		libdir .. "/LIB-Lith/src",
		libdir .. "/LIB-RezMgr/src",
		libdir .. "/LIB-STDLith/src"
	}

	links
	{
		"LIB-Lith",
		"LIB-RezMgr",
		"LIB-STDLith"
	}