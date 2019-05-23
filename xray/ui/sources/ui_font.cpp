////////////////////////////////////////////////////////////////////////////
//	Created		: 30.01.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "ui_font.h"

ui_font::ui_font()
:m_char_map(NULL)
{}

ui_font::~ui_font()
{
	FREE				(m_char_map);
}

void ui_font::on_config_loaded(xray::resources::queries_result &data)
{
	R_ASSERT(data.is_successful(), "Can't find font config '%s'", data[0].get_requested_path());

	configs::lua_config_ptr config = static_cast_resource_ptr<configs::lua_config_ptr>(data[0].get_unmanaged_resource());

	m_height			= (*config)["symbol_height"];
	m_char_count		= 256;
	
	m_ts_size.x			= 256.0f;
	m_ts_size.y			= 256.0f;

	FREE				(m_char_map);
	m_char_map			= (float3*)REALLOC(m_char_map, sizeof(float3) * m_char_count, "font charmap");

	const configs::lua_config_value &symbol_coords = (*config)["symbol_coords"];
	R_ASSERT(symbol_coords.size() >= m_char_count);

	for (u32 idx=0; idx<m_char_count; ++idx)
	{
		const float4& coord	= symbol_coords[idx];
		float3& ch			= m_char_map[idx];
		ch.x				= coord.x;
		ch.y				= coord.y;
		ch.z				= coord.z - coord.x;
	}
}

void ui_font::init_font(pcstr name)
{
	XRAY_UNREFERENCED_PARAMETER	(name);
	//. create render-specific resource(shader+geom)

	resources::query_resource_and_wait("resources/textures/ui/ui_font_arial_21_1024.lua", resources::config_lua_class, 
		boost::bind(&ui_font::on_config_loaded, this, _1), ui::g_allocator);
}

float3 ui_font::get_char_tc_ts(u8 const& ch)	const
{
	float3	result		= get_char_tc(ch);
	result.x 			/= m_ts_size.x;
	result.y 			/= m_ts_size.y;
	result.z 			/= m_ts_size.x;

 	result.x			+= (0.5f / m_ts_size.x);
 	result.y			+= (0.5f / m_ts_size.y);
	
	return				result;
}

float ui_font::get_height_ts()	const
{
	return get_height()/m_ts_size.y;
}

font_manager::font_manager()
{
	initialize_fonts();
}

font_manager::~font_manager()
{}

void font_manager::initialize_fonts()
{
	m_font.init_font("arial_14");
}

void font_manager::on_device_reset()
{
	initialize_fonts();
}

f32 calc_string_length(ui_font const& f, pcstr str)
{
	pcstr p			= str;
	f32	result		= 0.0f;
	while (*p)
	{
		result		+= f.get_char_tc(*p).z;
		++p;
	}
	return			result;
}

f32 calc_string_length_n	(ui_font const& f, pcstr str, u16 const str_len)
{
	pcstr p				= str;
	f32	result			= 0.0f;
	u16 char_counter	= str_len;
	while (*p && char_counter)
	{
		result		+= f.get_char_tc(*p).z;
		++p;
		--char_counter;
	}
	return			result;
}
