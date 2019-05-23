#ifndef XRAY_THM_H_INCLUDED
#define XRAY_THM_H_INCLUDED

namespace xray {

enum enum_thm_chunk_type
{
	THM_CHUNK_VERSION				 = 0x0810,
	THM_CHUNK_DATA					 = 0x0811,
	THM_CHUNK_TEXTUREPARAM			 = 0x0812,
	THM_CHUNK_TYPE					 = 0x0813,
	THM_CHUNK_TEXTURE_TYPE			 = 0x0814,
	THM_CHUNK_DETAIL_EXT			 = 0x0815,
	THM_CHUNK_MATERIAL				 = 0x0816,
	THM_CHUNK_BUMP					 = 0x0817,
	THM_CHUNK_EXT_NORMALMAP			 = 0x0818,
	THM_CHUNK_FADE_DELAY			 = 0x0819
};

enum enum_thm_texture_type
{
	ttImage	= 0,
	ttCubeMap,
	ttBumpMap,
	ttNormalMap,
	ttTerrain,
};

enum enum_thm_bump_mode
{
	tbmResereved	= 0,
	tbmNone,
	tbmUse,
	tbmUseParallax,
};

enum enum_thm_flags
{
	flDiffuseDetail		= (1<<23),
	flBumpDetail		= (1<<26),
};

} // namespace xray

#endif