////////////////////////////////////////////////////////////////////////////
//	Created 	: 10.02.2008
//	Author		: Konstantin Slipchenko
//	Description : contact
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "contact.h"


contact::contact( const contact_joint_info& c ):m_contact( 0 )
{
	init_contact_ode		( m_contact, c );
}

void	contact::render		( xray::render::debug::renderer& renderer ) const
{
	joint_base_impl::render( &m_contact, renderer );
	::render				( m_contact, renderer );
	//m_joint.render(  renderer );
}
