////////////////////////////////////////////////////////////////////////////
//	Created 	: 28.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_LIBRARY_LINKAGE_H_INCLUDED
#define XRAY_RENDER_LIBRARY_LINKAGE_H_INCLUDED

#if XRAY_PLATFORM_WINDOWS
#	include	<xray/render/dx9/library_linkage.h>
//#	include	<xray/render/dx10/library_linkage.h>
#	include <xray/render/base/library_linkage.h>

#pragma comment(lib, XRAY_LIBRARY_NAME(render_core_dx11, lib))
#pragma comment(lib, XRAY_LIBRARY_NAME(render_engine_dx11, lib))

#elif XRAY_PLATFORM_XBOX_360	// #if XRAY_PLATFORM_WINDOWS
#	include	<xray/render/xbox360/library_linkage.h>
#elif XRAY_PLATFORM_PS3			// #elif XRAY_PLATFORM_XBOX_360
#	include	<xray/render/ps3/library_linkage.h>
#endif // #elif XRAY_PLATFORM_PS3

#endif // #ifndef XRAY_RENDER_LIBRARY_LINKAGE_H_INCLUDED