////////////////////////////////////////////////////////////////////////////
//	Created		: 24.12.2009
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/os_include.h>

namespace xray {
namespace core {

pcstr   get_command_line ( )
{
	static string512	s_command_line	=	"";
	static bool			s_initialized	=	false;

	if ( ! s_initialized )
	{
		strings::copy						(s_command_line, GetCommandLine());
		s_initialized					=	true;
	}

	return									s_command_line;
}

} // namespace core
} // namespace xray

