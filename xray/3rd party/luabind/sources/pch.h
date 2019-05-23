#ifndef PCH_H
#define PCH_H

#if defined(SN_TARGET_PS3) && !defined(NDEBUG)
namespace scestd = std;

#include <assert.h>

namespace std {
	inline void _SCE_Assert(const char *first, const char *second)
	{
		return	scestd::_SCE_Assert(first, second);
	}

	inline void _Assert(const char *first, const char *second)
	{
		return	scestd::_Assert(first, second);
	}
} // namespace std
#endif // #if defined(SN_TARGET_PS3) && !defined(NDEBUG)

#ifdef NDEBUG
#	define LUABIND_NO_ERROR_CHECKING
#endif // #ifdef NDEBUG

#include <luabind/lua_include.hpp>
#include <luabind/luabind.hpp>

#endif // PCH_H