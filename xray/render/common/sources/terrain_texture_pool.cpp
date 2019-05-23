////////////////////////////////////////////////////////////////////////////
//	Created		: 05.03.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "terrain_texture_pool.h"

namespace xray {
namespace render {

terrain_texture_pool::terrain_texture_pool	( u32 const size, u32 const textures_count):
m_tiles				( (u32)math::ceil(math::sqrt( (float)textures_count)), (u32)math::ceil(math::sqrt( (float)textures_count))),
m_tile_size			( size),
m_textures_count	( m_tiles.x*m_tiles.y)
{
	ASSERT( m_tiles.x*size <= 4096 && m_tiles.y*size <= 4096);


//	if( hw_wrapper::get_ref().support(D3DFMT_DXT1,D3DRTYPE_TEXTURE,D3DUSAGE_AUTOGENMIPMAP) )
//		m_pool_texture	= resource_manager::get_ref().create_texture( get_pool_texture_name(), m_tiles*size, D3DFMT_DXT1, 0, 1);//, 10); // MAGIC! 10 is levels need for 1024 texture
//		
// 	if( hw_wrapper::get_ref().support( D3DFMT_R8G8B8,D3DRTYPE_TEXTURE,D3DUSAGE_AUTOGENMIPMAP))
// 		m_pool_texture	= resource_manager::get_ref().create_texture( get_pool_texture_name(), m_tiles*size, D3DFMT_R8G8B8, D3DUSAGE_AUTOGENMIPMAP, 0);
// 	
// 	else if( hw_wrapper::get_ref().support( D3DFMT_A8R8G8B8,D3DRTYPE_TEXTURE,D3DUSAGE_AUTOGENMIPMAP))
// 		m_pool_texture	= resource_manager::get_ref().create_texture( get_pool_texture_name(), m_tiles*size, D3DFMT_A8R8G8B8, D3DUSAGE_AUTOGENMIPMAP, 0);

//	m_pool_texture->set_autogen_mip_filter( D3DTEXF_LINEAR);

	const bool use_mipmaps = true;

	m_pool_texture_1 = resource_manager::get_ref().create_texture(get_pool_texture_name_1(), m_tiles*size, D3DFMT_DXT1, 0, use_mipmaps ? 5 : 1);
	m_pool_texture_2 = resource_manager::get_ref().create_texture(get_pool_texture_name_2(), m_tiles*size, D3DFMT_DXT1, 0, use_mipmaps ? 5 : 1);

	ASSERT(m_pool_texture_1 && m_pool_texture_2);
}

terrain_texture_pool::~terrain_texture_pool	()
{

}

int	terrain_texture_pool::add_texture		( texture_string const & name, bool deferred_load)
{
	int		ind		= -1;
	for( u32 i = 0; i < m_textures_count; ++i)
	{
		if( m_texture_items[i].name.length() == 0 )
		{
			if( ind == -1)
				ind = i;
		}else 
		if( m_texture_items[i].name == name)
		{
			if( !deferred_load && !m_texture_items[i].loaded)
				load_texture				(i);

			return i;
		}
	}

	if( ind >= 0)
	{
		m_texture_items[ind].name			= name;
		m_texture_items[ind].loaded			= false;

		if( !deferred_load)
			load_texture					(ind);
	}

	return ind;
}

void terrain_texture_pool::remove_texture	( u32 id)
{
	ASSERT( id < terrain_texture_max_count);
	ASSERT( m_texture_items[id].name != "" );

	m_texture_items[id].name			= "";
	m_texture_items[id].loaded			= false;
}

void terrain_texture_pool::load_textures	()
{
	for( u32 i = 0; i < m_textures_count; ++i)
		if( m_texture_items[i].name.length() > 0 && !m_texture_items[i].loaded )
			load_texture				(i);
}

math::rectangle<int2>	terrain_texture_pool::get_tile_rect	(u32 ind)
{
	ASSERT(ind < m_tiles.x*m_tiles.y);
	int top		= ind/m_tiles.x;
	int left	= ind-top*m_tiles.x;

	return math::rectangle<int2>( int2(left*m_tile_size, top*m_tile_size), int2((left+1)*m_tile_size, (top+1)*m_tile_size) );
}

bool	terrain_texture_pool::exchange_texture( texture_string const & old_texture, texture_string const &  new_texture, bool deffered_load)
{
	int	ind = get_texture_id		( old_texture);
	ASSERT							( ind >= 0);

	m_texture_items[ind].name		= new_texture;
	m_texture_items[ind].loaded		= false;

	if( !deffered_load)
		load_texture					(ind);
	
	return false;
}

int		terrain_texture_pool::get_texture_id	( texture_string const & name)
{
	for( u32 i = 0; i < m_textures_count; ++i)
	{
		if( m_texture_items[i].name ==  name)
			return i;
	}
	return -1;
}

void	terrain_texture_pool::load_texture	( u32 ind)
{
	ASSERT( m_texture_items[ind].name.length() > 0);
	ASSERT( !m_texture_items[ind].loaded);

/*
	texture_string name = m_texture_items[ind].name;
	resource_manager::get_ref().copy_texture_from_file(m_pool_texture_1, get_tile_rect(ind), name.c_str());	
	name += "_shift";
	resource_manager::get_ref().copy_texture_from_file(m_pool_texture_2, get_tile_rect(ind), name.c_str());
*/
	texture_string path;
	path.assignf("resources/textures/%s.dds", m_texture_items[ind].name.c_str());
	resources::query_resource(path.c_str(),
		resources::raw_data_class,
		boost::bind(&terrain_texture_pool::on_texture_loaded, this, _1, ind),
		g_allocator);
	m_texture_items[ind].loaded		= true;
}

void	terrain_texture_pool::on_texture_loaded(resources::queries_result& data, u32 tile_id)
{
	if(!data.is_successful())
	{
		LOG_ERROR("Can't find texture '%s'", data[0].get_requested_path());
		return;
	}

	math::rectangle<int2> dest_rect = get_tile_rect(tile_id);
	resources::pinned_ptr_const<u8> ptr(data[0].get_managed_resource());

	D3DXIMAGE_INFO image_info;
	R_CHK(D3DXGetImageInfoFromFileInMemory(ptr.c_ptr(), ptr.size(), &image_info));
	
	// ordinary
	DWORD levels = ((ID3DTexture2D*)m_pool_texture_1->get_surface())->GetLevelCount();
	for (DWORD l = 0; l < levels; l++)
	{
		math::rectangle<int2> r;
		ID3DSurface* surface = NULL;

		R_CHK(((ID3DTexture2D*)m_pool_texture_1->get_surface())->GetSurfaceLevel(l, &surface));

		r			= dest_rect / (1<<l);
		r.right		= math::max(r.right, 1);
		r.bottom	= math::max(r.bottom, 1);
		R_CHK(D3DXLoadSurfaceFromFileInMemory(surface, NULL, (LPRECT)&r, ptr.c_ptr(), ptr.size(), NULL, D3DX_FILTER_TRIANGLE, 0, NULL));

		surface->Release();
	}

	((ID3DTexture2D*)m_pool_texture_1->get_surface())->AddDirtyRect((LPRECT)&dest_rect);

	// shifted
	math::rectangle<int2> dest_rects[4];
	int hw = dest_rect.width() / 2, hh = dest_rect.height() / 2;

	dest_rects[0] = math::rectangle<int2>(dest_rect.min, dest_rect.max - math::int2(hw,hh));
	dest_rects[1] = dest_rects[0]; dest_rects[1].move(math::int2(hw,0));
	dest_rects[2] = dest_rects[0]; dest_rects[2].move(math::int2(0,hh));
	dest_rects[3] = dest_rects[0]; dest_rects[3].move(math::int2(hw,hh));

	math::rectangle<int2> src_rects[4];
	hw = image_info.Width / 2, hh = image_info.Height / 2;

	src_rects[0] = math::rectangle<int2>(math::int2(0, 0), math::int2(hw, hh));
	src_rects[1] = src_rects[0]; src_rects[1].move(math::int2(hw, 0));
	src_rects[2] = src_rects[0]; src_rects[2].move(math::int2(0, hh));
	src_rects[3] = src_rects[0]; src_rects[3].move(math::int2(hw, hh));

	levels = ((ID3DTexture2D*)m_pool_texture_2->get_surface())->GetLevelCount();
	for (DWORD l = 0; l < levels; l++)
	{
		math::rectangle<int2> dest;

		ID3DSurface* surface = NULL;
		R_CHK(((ID3DTexture2D*)m_pool_texture_2->get_surface())->GetSurfaceLevel(l, &surface));

		for(u32 r = 0; r < 4; r++)
		{
			dest = dest_rects[3-r] / (1<<l);
			dest.right = math::max(dest.right, 1);
			dest.bottom = math::max(dest.bottom, 1);

			R_CHK(D3DXLoadSurfaceFromFileInMemory(surface, NULL, (LPRECT)&dest, ptr.c_ptr(), ptr.size(), (LPRECT)&src_rects[r], D3DX_FILTER_TRIANGLE, 0, NULL));
		}

		surface->Release();
	}

	((ID3DTexture2D*)m_pool_texture_2->get_surface())->AddDirtyRect((LPRECT)&dest_rect);
}

} // namespace render
} // namespace xray
