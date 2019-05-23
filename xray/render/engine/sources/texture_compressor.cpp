#include "pch.h"

//#pragma comment(lib,"ati_compress_mt.lib")
#pragma comment(lib,"ATI_Compress_1_7_DLL.lib")

#include "ddraw.h"
#include "d3d9types.h"
#include <amd\compress\texture_compressor\api.h>
#include <amd\compress\ati_compress.h>
#include <xray\logging_extensions.h>

#define FOURCC_ATI1N		        MAKEFOURCC('A', 'T', 'I', '1')
#define FOURCC_ATI2N		        MAKEFOURCC('A', 'T', 'I', '2')
#define FOURCC_ATI2N_XY		        MAKEFOURCC('A', '2', 'X', 'Y')
#define FOURCC_ATI2N_DXT5	        MAKEFOURCC('A', '2', 'D', '5')
#define FOURCC_DXT5_xGBR	        MAKEFOURCC('x', 'G', 'B', 'R')
#define FOURCC_DXT5_RxBG	        MAKEFOURCC('R', 'x', 'B', 'G')
#define FOURCC_DXT5_RBxG	        MAKEFOURCC('R', 'B', 'x', 'G')
#define FOURCC_DXT5_xRBG	        MAKEFOURCC('x', 'R', 'B', 'G')
#define FOURCC_DXT5_RGxB	        MAKEFOURCC('R', 'G', 'x', 'B')
#define FOURCC_DXT5_xGxR	        MAKEFOURCC('x', 'G', 'x', 'R')
#define FOURCC_APC1			        MAKEFOURCC('A', 'P', 'C', '1')
#define FOURCC_APC2			        MAKEFOURCC('A', 'P', 'C', '2')
#define FOURCC_APC3			        MAKEFOURCC('A', 'P', 'C', '3')
#define FOURCC_APC4			        MAKEFOURCC('A', 'P', 'C', '4')
#define FOURCC_APC5			        MAKEFOURCC('A', 'P', 'C', '5')
#define FOURCC_APC6			        MAKEFOURCC('A', 'P', 'C', '6')
#define FOURCC_ATC_RGB			    MAKEFOURCC('A', 'T', 'C', ' ')
#define FOURCC_ATC_RGBA_EXPLICIT    MAKEFOURCC('A', 'T', 'C', 'A')
#define FOURCC_ATC_RGBA_INTERP	    MAKEFOURCC('A', 'T', 'C', 'I')
#define FOURCC_ETC_RGB			    MAKEFOURCC('E', 'T', 'C', ' ')
#define FOURCC_BC1                  MAKEFOURCC('B', 'C', '1', ' ')
#define FOURCC_BC2                  MAKEFOURCC('B', 'C', '2', ' ')
#define FOURCC_BC3                  MAKEFOURCC('B', 'C', '3', ' ')
#define FOURCC_BC4                  MAKEFOURCC('B', 'C', '4', ' ')
#define FOURCC_BC4S                 MAKEFOURCC('B', 'C', '4', 'S')
#define FOURCC_BC4U                 MAKEFOURCC('B', 'C', '4', 'U')
#define FOURCC_BC5                  MAKEFOURCC('B', 'C', '5', ' ')
#define FOURCC_BC5S                 MAKEFOURCC('B', 'C', '5', 'S')
// Deprecated but still supported for decompression
#define FOURCC_DXT5_GXRB            MAKEFOURCC('G', 'X', 'R', 'B')
#define FOURCC_DXT5_GRXB            MAKEFOURCC('G', 'R', 'X', 'B')
#define FOURCC_DXT5_RXGB            MAKEFOURCC('R', 'X', 'G', 'B')
#define FOURCC_DXT5_BRGX            MAKEFOURCC('B', 'R', 'G', 'X')

typedef struct
{
   DWORD dwFourCC;
   ATI_TC_FORMAT nFormat;
} ATI_TC_FourCC;

ATI_TC_FourCC g_FourCCs[] =
{
   {FOURCC_DXT1,               ATI_TC_FORMAT_DXT1},
   {FOURCC_DXT3,               ATI_TC_FORMAT_DXT3},
   {FOURCC_DXT5,               ATI_TC_FORMAT_DXT5},
   {FOURCC_DXT5_xGBR,          ATI_TC_FORMAT_DXT5_xGBR},
   {FOURCC_DXT5_RxBG,          ATI_TC_FORMAT_DXT5_RxBG},
   {FOURCC_DXT5_RBxG,          ATI_TC_FORMAT_DXT5_RBxG},
   {FOURCC_DXT5_xRBG,          ATI_TC_FORMAT_DXT5_xRBG},
   {FOURCC_DXT5_RGxB,          ATI_TC_FORMAT_DXT5_RGxB},
   {FOURCC_DXT5_xGxR,          ATI_TC_FORMAT_DXT5_xGxR},
   {FOURCC_DXT5_GXRB,          ATI_TC_FORMAT_DXT5_xRBG},
   {FOURCC_DXT5_GRXB,          ATI_TC_FORMAT_DXT5_RxBG},
   {FOURCC_DXT5_RXGB,          ATI_TC_FORMAT_DXT5_xGBR},
   {FOURCC_DXT5_BRGX,          ATI_TC_FORMAT_DXT5_RGxB},
   {FOURCC_ATI1N,              ATI_TC_FORMAT_ATI1N},
   {FOURCC_ATI2N,              ATI_TC_FORMAT_ATI2N},
   {FOURCC_ATI2N_XY,           ATI_TC_FORMAT_ATI2N_XY},
   {FOURCC_ATI2N_DXT5,         ATI_TC_FORMAT_ATI2N_DXT5},
   {FOURCC_BC1,                ATI_TC_FORMAT_BC1},
   {FOURCC_BC2,                ATI_TC_FORMAT_BC2},
   {FOURCC_BC3,                ATI_TC_FORMAT_BC3},
   {FOURCC_BC4,                ATI_TC_FORMAT_BC4},
   {FOURCC_BC4S,               ATI_TC_FORMAT_BC4},
   {FOURCC_BC4U,               ATI_TC_FORMAT_BC4},
   {FOURCC_BC5,                ATI_TC_FORMAT_BC5},
   {FOURCC_BC5S,               ATI_TC_FORMAT_BC5},
   {FOURCC_ATC_RGB,            ATI_TC_FORMAT_ATC_RGB},
   {FOURCC_ATC_RGBA_EXPLICIT,  ATI_TC_FORMAT_ATC_RGBA_Explicit},
   {FOURCC_ATC_RGBA_INTERP,    ATI_TC_FORMAT_ATC_RGBA_Interpolated},
   {FOURCC_ETC_RGB,            ATI_TC_FORMAT_ETC_RGB},
};
DWORD g_dwFourCCCount = sizeof(g_FourCCs) / sizeof(g_FourCCs[0]);

ATI_TC_FORMAT GetFormat(DWORD dwFourCC)
{
   for(DWORD i = 0; i < g_dwFourCCCount; i++)
      if(g_FourCCs[i].dwFourCC == dwFourCC)
         return g_FourCCs[i].nFormat;

   return ATI_TC_FORMAT_Unknown;
}

DWORD GetFourCC(ATI_TC_FORMAT nFormat)
{
   for(DWORD i = 0; i < g_dwFourCCCount; i++)
      if(g_FourCCs[i].nFormat == nFormat)
         return g_FourCCs[i].dwFourCC;

   return 0;
}

bool IsDXT5SwizzledFormat(ATI_TC_FORMAT nFormat)
{
   if(nFormat == ATI_TC_FORMAT_DXT5_xGBR || nFormat == ATI_TC_FORMAT_DXT5_RxBG || nFormat == ATI_TC_FORMAT_DXT5_RBxG ||
      nFormat == ATI_TC_FORMAT_DXT5_xRBG || nFormat == ATI_TC_FORMAT_DXT5_RGxB || nFormat == ATI_TC_FORMAT_DXT5_xGxR ||
      nFormat == ATI_TC_FORMAT_ATI2N_DXT5)
      return true;
   else
      return false;
}

typedef struct
{
   ATI_TC_FORMAT nFormat;
   TCHAR* pszFormatDesc;
} ATI_TC_FormatDesc;

ATI_TC_FormatDesc g_FormatDesc[] =
{
   {ATI_TC_FORMAT_Unknown,                 "Unknown"},
   {ATI_TC_FORMAT_ARGB_8888,               "ARGB_8888"},
   {ATI_TC_FORMAT_RGB_888,                 "RGB_888"},
   {ATI_TC_FORMAT_RG_8,                    "RG_8"},
   {ATI_TC_FORMAT_R_8,                     "R_8"},
   {ATI_TC_FORMAT_ARGB_2101010,            "ARGB_2101010"},
   {ATI_TC_FORMAT_ARGB_16,                 "ARGB_16"},
   {ATI_TC_FORMAT_RG_16,                   "RG_16"},
   {ATI_TC_FORMAT_R_16,                    "R_16"},
   {ATI_TC_FORMAT_ARGB_16F,                "ARGB_16F"},
   {ATI_TC_FORMAT_RG_16F,                  "RG_16F"},
   {ATI_TC_FORMAT_R_16F,                   "R_16F"},
   {ATI_TC_FORMAT_ARGB_32F,                "ARGB_32F"},
   {ATI_TC_FORMAT_RG_32F,                  "RG_32F"},
   {ATI_TC_FORMAT_R_32F,                   "R_32F"},
   {ATI_TC_FORMAT_DXT1,                    "DXT1"},
   {ATI_TC_FORMAT_DXT3,                    "DXT3"},
   {ATI_TC_FORMAT_DXT5,                    "DXT5"},
   {ATI_TC_FORMAT_DXT5_xGBR,               "DXT5_xGBR"},
   {ATI_TC_FORMAT_DXT5_RxBG,               "DXT5_RxBG"},
   {ATI_TC_FORMAT_DXT5_RBxG,               "DXT5_RBxG"},
   {ATI_TC_FORMAT_DXT5_xRBG,               "DXT5_xRBG"},
   {ATI_TC_FORMAT_DXT5_RGxB,               "DXT5_RGxB"},
   {ATI_TC_FORMAT_DXT5_xGxR,               "DXT5_xGxR"},
   {ATI_TC_FORMAT_ATI1N,                   "ATI1N"},
   {ATI_TC_FORMAT_ATI2N,                   "ATI2N"},
   {ATI_TC_FORMAT_ATI2N_XY,                "ATI2N_XY"},
   {ATI_TC_FORMAT_ATI2N_DXT5,              "ATI2N_DXT5"},
   {ATI_TC_FORMAT_BC1,                     "BC1"},
   {ATI_TC_FORMAT_BC2,                     "BC2"},
   {ATI_TC_FORMAT_BC3,                     "BC3"},
   {ATI_TC_FORMAT_BC4,                     "BC4"},
   {ATI_TC_FORMAT_BC5,                     "BC5"},
   {ATI_TC_FORMAT_ATC_RGB,                 "ATC_RGB"},
   {ATI_TC_FORMAT_ATC_RGBA_Explicit,       "ATC_RGBA_Explicit"},
   {ATI_TC_FORMAT_ATC_RGBA_Interpolated,   "ATC_RGBA_Interpolated"},
   {ATI_TC_FORMAT_ETC_RGB,                 "ETC_RGB"},
};
DWORD g_dwFormatDescCount = sizeof(g_FormatDesc) / sizeof(g_FormatDesc[0]);

#ifdef _WIN64
#	define POINTER_64 __ptr64

#pragma pack(4)

typedef struct _DDSURFACEDESC2_64
{
   DWORD               dwSize;                 // size of the DDSURFACEDESC structure
   DWORD               dwFlags;                // determines what fields are valid
   DWORD               dwHeight;               // height of surface to be created
   DWORD               dwWidth;                // width of input surface
   union
   {
      LONG            lPitch;                 // distance to start of next line (return value only)
      DWORD           dwLinearSize;           // Formless late-allocated optimized surface size
   } DUMMYUNIONNAMEN(1);
   union
   {
      DWORD           dwBackBufferCount;      // number of back buffers requested
      DWORD           dwDepth;                // the depth if this is a volume texture
   } DUMMYUNIONNAMEN(5);
   union
   {
      DWORD           dwMipMapCount;          // number of mip-map levels requestde
      // dwZBufferBitDepth removed, use ddpfPixelFormat one instead
      DWORD           dwRefreshRate;          // refresh rate (used when display mode is described)
      DWORD           dwSrcVBHandle;          // The source used in VB::Optimize
   } DUMMYUNIONNAMEN(2);
   DWORD               dwAlphaBitDepth;        // depth of alpha buffer requested
   DWORD               dwReserved;             // reserved
   void* __ptr32       lpSurface;              // pointer to the associated surface memory
   union
   {
      DDCOLORKEY      ddckCKDestOverlay;      // color key for destination overlay use
      DWORD           dwEmptyFaceColor;       // Physical color for empty cubemap faces
   } DUMMYUNIONNAMEN(3);
   DDCOLORKEY          ddckCKDestBlt;          // color key for destination blt use
   DDCOLORKEY          ddckCKSrcOverlay;       // color key for source overlay use
   DDCOLORKEY          ddckCKSrcBlt;           // color key for source blt use
   union
   {
      DDPIXELFORMAT   ddpfPixelFormat;        // pixel format description of the surface
      DWORD           dwFVF;                  // vertex format description of vertex buffers
   } DUMMYUNIONNAMEN(4);
   DDSCAPS2            ddsCaps;                // direct draw surface capabilities
   DWORD               dwTextureStage;         // stage in multitexture cascade
} DDSURFACEDESC2_64;

#define DDSD2 DDSURFACEDESC2_64
#else
#define DDSD2 DDSURFACEDESC2
#endif

static const DWORD DDS_HEADER = MAKEFOURCC('D', 'D', 'S', ' ');

void un_rle(u8 *src, u8 *dest, u32 dest_length, u32 bpp)
{
	u32 pos = 0;
	u8 i;

	while(pos<dest_length)
	{
		u8 hdr = *src++;
		u8 len = (hdr & 0x7F) + 1;

		if(hdr & 0x80)
		{
			u8 c[4];
			for(i = 0; i < bpp; i++) c[i] = *src++;

			while(len > 0)
			{
				for(i = 0; i < bpp; i++)
					dest[pos++] = c[i];

				len--;
			}
		}
		else
		{
			while(len > 0)
			{
				for(i = 0; i < bpp; i++)
					dest[pos++] = *src++;

				len--;
			}
		}
	}
}

void make_dds(u8* dest_buf,ATI_TC_Texture& texture)
{
	//copy-paste from samples
	DDSD2 ddsd;
	memset(&ddsd, 0, sizeof(DDSD2));
	ddsd.dwSize = sizeof(DDSD2);
	ddsd.dwFlags = DDSD_CAPS|DDSD_WIDTH|DDSD_HEIGHT|DDSD_PIXELFORMAT|DDSD_MIPMAPCOUNT|DDSD_LINEARSIZE;
	ddsd.dwWidth = texture.dwWidth;
	ddsd.dwHeight = texture.dwHeight;
	ddsd.dwMipMapCount = 1;

	ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
	ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE|DDSCAPS_COMPLEX|DDSCAPS_MIPMAP;

	ddsd.ddpfPixelFormat.dwFourCC = GetFourCC(texture.format);
	if(ddsd.ddpfPixelFormat.dwFourCC)
	{
		ddsd.dwLinearSize = texture.dwDataSize;
		ddsd.ddpfPixelFormat.dwFlags = DDPF_FOURCC;
		if(IsDXT5SwizzledFormat(texture.format))
		{
			ddsd.ddpfPixelFormat.dwPrivateFormatBitCount = ddsd.ddpfPixelFormat.dwFourCC;
			ddsd.ddpfPixelFormat.dwFourCC = FOURCC_DXT5;
		}
	}
	else
	{
		switch(texture.format)
		{
		case ATI_TC_FORMAT_ARGB_8888:
			ddsd.ddpfPixelFormat.dwRBitMask = 0x00ff0000;
			ddsd.ddpfPixelFormat.dwGBitMask = 0x0000ff00;
			ddsd.ddpfPixelFormat.dwBBitMask = 0x000000ff;
			ddsd.lPitch = texture.dwPitch;
			ddsd.ddpfPixelFormat.dwRGBBitCount = 32;
			ddsd.ddpfPixelFormat.dwFlags=DDPF_ALPHAPIXELS|DDPF_RGB;
			ddsd.ddpfPixelFormat.dwRGBAlphaBitMask = 0xff000000;
			break;

		case ATI_TC_FORMAT_RGB_888:
			ddsd.ddpfPixelFormat.dwRBitMask = 0x00ff0000;
			ddsd.ddpfPixelFormat.dwGBitMask = 0x0000ff00;
			ddsd.ddpfPixelFormat.dwBBitMask = 0x000000ff;
			ddsd.lPitch = texture.dwPitch;
			ddsd.ddpfPixelFormat.dwRGBBitCount = 24;
			ddsd.ddpfPixelFormat.dwFlags=DDPF_RGB;
			break;

		case ATI_TC_FORMAT_RG_8:
			ddsd.ddpfPixelFormat.dwRBitMask = 0x0000ff00;
			ddsd.ddpfPixelFormat.dwGBitMask = 0x000000ff;
			ddsd.lPitch = texture.dwPitch;
			ddsd.ddpfPixelFormat.dwRGBBitCount = 16;
			ddsd.ddpfPixelFormat.dwFlags=DDPF_ALPHAPIXELS|DDPF_LUMINANCE;
			ddsd.ddpfPixelFormat.dwRGBAlphaBitMask = 0xff000000;
			break;

		case ATI_TC_FORMAT_R_8:
			ddsd.ddpfPixelFormat.dwRBitMask = 0x000000ff;
			ddsd.lPitch = texture.dwPitch;
			ddsd.ddpfPixelFormat.dwRGBBitCount = 8;
			ddsd.ddpfPixelFormat.dwFlags= DDPF_LUMINANCE;
			break;

		case ATI_TC_FORMAT_ARGB_2101010:
			ddsd.ddpfPixelFormat.dwRBitMask = 0x000003ff;
			ddsd.ddpfPixelFormat.dwGBitMask = 0x000ffc00;
			ddsd.ddpfPixelFormat.dwBBitMask = 0x3ff00000;
			ddsd.ddpfPixelFormat.dwRGBAlphaBitMask = 0xc0000000;
			ddsd.lPitch = texture.dwPitch;
			ddsd.ddpfPixelFormat.dwRGBBitCount = 32;
			ddsd.ddpfPixelFormat.dwFlags=DDPF_ALPHAPIXELS|DDPF_RGB;
			break;

		case ATI_TC_FORMAT_ARGB_16:
			ddsd.dwLinearSize = texture.dwDataSize;
			ddsd.ddpfPixelFormat.dwFlags = DDPF_FOURCC|DDPF_ALPHAPIXELS;
			ddsd.ddpfPixelFormat.dwFourCC = D3DFMT_A16B16G16R16;
			break;

		case ATI_TC_FORMAT_RG_16:
			ddsd.dwLinearSize = texture.dwDataSize;
			ddsd.ddpfPixelFormat.dwFlags = DDPF_FOURCC;
			ddsd.ddpfPixelFormat.dwFourCC = D3DFMT_G16R16;
			break;

		case ATI_TC_FORMAT_R_16:
			ddsd.dwLinearSize = texture.dwDataSize;
			ddsd.ddpfPixelFormat.dwFlags = DDPF_FOURCC;
			ddsd.ddpfPixelFormat.dwFourCC = D3DFMT_L16;
			break;

		case ATI_TC_FORMAT_ARGB_16F:
			ddsd.dwLinearSize = texture.dwDataSize;
			ddsd.ddpfPixelFormat.dwFlags = DDPF_FOURCC|DDPF_ALPHAPIXELS;
			ddsd.ddpfPixelFormat.dwFourCC = D3DFMT_A16B16G16R16F;
			break;

		case ATI_TC_FORMAT_RG_16F:
			ddsd.dwLinearSize = texture.dwDataSize;
			ddsd.ddpfPixelFormat.dwFlags = DDPF_FOURCC;
			ddsd.ddpfPixelFormat.dwFourCC = D3DFMT_G16R16F;
			break;

		case ATI_TC_FORMAT_R_16F:
			ddsd.dwLinearSize = texture.dwDataSize;
			ddsd.ddpfPixelFormat.dwFlags = DDPF_FOURCC;
			ddsd.ddpfPixelFormat.dwFourCC = D3DFMT_R16F;
			break;

		case ATI_TC_FORMAT_ARGB_32F:
			ddsd.dwLinearSize = texture.dwDataSize;
			ddsd.ddpfPixelFormat.dwFlags = DDPF_FOURCC|DDPF_ALPHAPIXELS;
			ddsd.ddpfPixelFormat.dwFourCC = D3DFMT_A32B32G32R32F;
			break;

		case ATI_TC_FORMAT_RG_32F:
			ddsd.dwLinearSize = texture.dwDataSize;
			ddsd.ddpfPixelFormat.dwFlags = DDPF_FOURCC;
			ddsd.ddpfPixelFormat.dwFourCC = D3DFMT_G32R32F;
			break;

		case ATI_TC_FORMAT_R_32F:
			ddsd.dwLinearSize = texture.dwDataSize;
			ddsd.ddpfPixelFormat.dwFlags = DDPF_FOURCC;
			ddsd.ddpfPixelFormat.dwFourCC = D3DFMT_R32F;
			break;

		default:
			assert(0);
			break;
		}
	}
	//write texture to memory
	memcpy(dest_buf,&DDS_HEADER,sizeof(DWORD));
	memcpy(dest_buf + sizeof(DWORD),&ddsd,sizeof(ddsd));
	memcpy(dest_buf + sizeof(DWORD) + sizeof(ddsd), texture.pData, texture.dwDataSize);
}

u8* create_texture(u32& size, pvoid (*allocate_func)(u32,bool), void (*free_func)(pvoid), const u8* tga_buf, ATI_TC_FORMAT format,
				   xray::texture_compressor::compression_speed speed, bool mt)
{
	ATI_TC_Texture texture, dest_texture;
	ATI_TC_CompressOptions options;
	u8* dest_buf;

	xray::texture_compressor::load_tga(allocate_func,free_func,tga_buf,texture);

	dest_texture.dwSize = sizeof(ATI_TC_Texture);
	dest_texture.dwHeight = texture.dwHeight;
	dest_texture.dwWidth = texture.dwWidth;
	dest_texture.dwPitch = 0;
	dest_texture.format = format;
	dest_texture.dwDataSize = ATI_TC_CalculateBufferSize(&dest_texture);
	dest_texture.pData = (u8*)allocate_func(dest_texture.dwDataSize,true);

	memset(&options,0,sizeof(options));
	options.dwSize = sizeof(options);
	options.bDisableMultiThreading = !mt;
	options.nCompressionSpeed = (ATI_TC_Speed)speed;
	
	ATI_TC_ERROR result = ATI_TC_ConvertTexture(&texture,&dest_texture,&options,NULL,NULL,NULL);

	if(result != ATI_TC_OK)
	{
		LOGI_ERROR("texture_compressor","ATI TC reported an error %u",result);
	}

	size = sizeof(DWORD) + sizeof(DDSD2) + dest_texture.dwDataSize;
	dest_buf = (u8*)allocate_func(size,false);
	make_dds(dest_buf,dest_texture);

	//cleanup
	free_func(dest_texture.pData);
	free_func(texture.pData);

	return dest_buf;
}

namespace xray {
namespace texture_compressor {

	ATI_TC_FORMAT parse_format(char* pszFormat)
	{
		if(pszFormat == NULL)
			return ATI_TC_FORMAT_Unknown;

		for(DWORD i = 0; i < g_dwFormatDescCount; i++)
			if(stricmp(pszFormat, g_FormatDesc[i].pszFormatDesc) == 0)
				return g_FormatDesc[i].nFormat;

		return ATI_TC_FORMAT_Unknown;
	}

	char* get_format_dest(ATI_TC_FORMAT nFormat)
	{
		for(DWORD i = 0; i < g_dwFormatDescCount; i++)
			if(nFormat == g_FormatDesc[i].nFormat)
				return g_FormatDesc[i].pszFormatDesc;

		return g_FormatDesc[0].pszFormatDesc;
	}

	void load_tga(pvoid (*allocate_func)(u32,bool),void (*free_func)(pvoid),const u8* tga_buf,ATI_TC_Texture& texture)
	{
		XRAY_UNREFERENCED_PARAMETER(free_func);
		TgaHeader *hdr = (TgaHeader*)tga_buf;
		u8* rgba_buf;
		u8* raw_image = (u8*)tga_buf + sizeof(TgaHeader) + hdr->id_length + hdr->color_map_length;

		ASSERT(hdr->image_type == 2 || hdr->image_type == 10);

		texture.dwSize = sizeof(ATI_TC_Texture);
		texture.dwWidth = hdr->width;
		texture.dwHeight = hdr->height;
		texture.dwPitch = hdr->width * (hdr->pixel_size / 8);
		texture.format = hdr->pixel_size == 32 ? ATI_TC_FORMAT_ARGB_8888 : ATI_TC_FORMAT_RGB_888;
		texture.dwDataSize = hdr->width * hdr->height * (hdr->pixel_size / 8);

		rgba_buf = (u8*)allocate_func(texture.dwDataSize,false);

		if(hdr->image_type == 2)
		{
			memcpy(rgba_buf,raw_image,texture.dwDataSize);
		}
		else
		{
//			LOG_INFO("trying unrle");
			un_rle(raw_image,rgba_buf,texture.dwDataSize,hdr->pixel_size / 8);
		}

		if(hdr->y_origin == 0)
		{
			u8 *copy = (u8*)allocate_func(texture.dwDataSize,true);
			memcpy(copy,rgba_buf,texture.dwDataSize);

			for(u32 i = 0; i < texture.dwHeight; i++)
				memcpy(rgba_buf + (i * texture.dwPitch),copy + ((texture.dwHeight - 1 - i) * texture.dwPitch),texture.dwPitch); 

			free_func(copy);
		}

		texture.pData = rgba_buf;
	}
} //namespace texture_compressor
} //namespace xray