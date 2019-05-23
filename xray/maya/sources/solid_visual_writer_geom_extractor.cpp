////////////////////////////////////////////////////////////////////////////
//	Created		: 11.05.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "solid_visual_writer.h"
using namespace xray;

void transform_maya_to_xray( float3& pt )
{
	MDistance dst_x	( pt.x );
	MDistance dst_y	( pt.y );
	MDistance dst_z	( pt.z );
	pt.set		( (float)dst_x.asMeters(), (float)dst_y.asMeters(), -(float)dst_z.asMeters() );
}

int get_local_idx(int const value, MIntArray const& arr, MStatus& status)
{
	status = MStatus::kSuccess;

	for(u32 i=0; i<arr.length(); ++i)
		if(arr[i]==value)
			return i;

	status = MStatus::kFailure;
	return -1;
}

MStatus solid_visual_writer::extract_render_geometry( MDagPath& fnDagPath )
{
	MFnMesh fnMesh				( fnDagPath );

	MObjectArray				PolygonSets;
	MObjectArray				PolygonComponents;

	MString						CurrentUVSetName;
	MStatus						result;

	MSpace::Space current_space	= MSpace::kWorld;

	MIntArray					triangles;
	MIntArray					triangle_vertices;

	result = fnMesh.getCurrentUVSetName( CurrentUVSetName );
	
	if(result != MS::kSuccess)
		return result;

	result = fnMesh.getTriangles( triangles, triangle_vertices );

	if(result != MS::kSuccess)
		return result;

	const float* raw_points		= fnMesh.getRawPoints( &result );
	
	if(result != MS::kSuccess)
		return result;
	
	//Have to make the path include the shape below it so that
	//we can determine if the underlying shape node is instanced.
	//By default, dag paths only include transform nodes.
	fnDagPath.extendToShape( );

	//If the shape is instanced then we need to determine which
	//instance this path refers to.
	int instanceNum				= 0;

	if( fnDagPath.isInstanced( ) )
		instanceNum	= fnDagPath.instanceNumber();

	//Get the connected sets and members - these will be used to determine texturing of different faces
	result = fnMesh.getConnectedSetsAndMembers(instanceNum, PolygonSets, PolygonComponents, true);
	
	if(result != MS::kSuccess)
		return result;

	// fill object surfaces
	unsigned int setCount = PolygonSets.length();

	for(u32 set_idx = 0; set_idx < setCount; ++set_idx ) 
	{
		int surface_tri_count			= 0;

		MObject set						= PolygonSets[set_idx];
		MObject comp					= PolygonComponents[set_idx];

		MFnSet fnSet					(set, &result);
		
		if(result != MS::kSuccess)
			return result;

		if(setCount > 1 && comp.isNull())
			continue;

        //Make sure the set is a polygonal set.  If not, continue.
		MItMeshPolygon itMeshPolygon	(fnDagPath, comp, &result);
		
		if(result != MS::kSuccess)
			return result;

		MObject shaderNode				= findShader( set, fnMesh, result );
		
		if(result != MS::kSuccess)
			return result;

		// shader props
		MString sh_name, tex_name;
		bool doublesided;

		result = shader_props( shaderNode, sh_name, tex_name, doublesided );
		
		if(result != MS::kSuccess)
			return result;

		render_surface*	current_surface		= NEW(render_surface)();
		current_surface->m_bbox.invalidate	();
		m_render_surfaces.push_back			(current_surface);
		
		current_surface->m_shader_name	= sh_name;
		current_surface->m_texture_name	= tex_name;

		u32 point_struct_size		= 3 ; // raw_points pointer float[3]

		for (itMeshPolygon.reset(); !itMeshPolygon.isDone(); itMeshPolygon.next())
		{
			bool b_has_valid_triangulation = itMeshPolygon.hasValidTriangulation();
			
			if(!b_has_valid_triangulation)
				return MS::kFailure;

			u32 face_idx			= itMeshPolygon.index();

			MIntArray face_vertex_list;
			result = itMeshPolygon.getVertices( face_vertex_list );
			
			if(result != MS::kSuccess)
				return result;

			MFloatVectorArray face_normals;
			result = fnMesh.getFaceVertexNormals(face_idx, face_normals, current_space);

			if(result != MS::kSuccess)
				return result;

			u32 tri_count = triangles[face_idx];
			surface_tri_count += tri_count;

			for(u32 tri_idx=0; tri_idx<tri_count; ++tri_idx)
			{
				int vertexIndicies[3] = {-1};
				result				= fnMesh.getPolygonTriangleVertices(face_idx, tri_idx, vertexIndicies);
				
				if(result != MS::kSuccess)
					return result;

				static int vert_remap_021[] = {0,2,1};
				static int vert_remap_012[] = {0,1,2};

				int const* vert_remap			= vert_remap_021;
				float normal_mul		= 1.0f;
				for(int d=0; d<2; ++d)
				{
					if(d==1)
					{
						if(!doublesided)
							break;
						normal_mul = -1.0f;
						vert_remap			= vert_remap_012;
					}

				for(int v_c=0; v_c<3; ++v_c)
				{
					int vc					= vert_remap[v_c];
					int vindex				= vertexIndicies[vc];

					int local_vertex_index	= get_local_idx( vindex, face_vertex_list, result );
					
					if(result != MS::kSuccess)
						return result;

					int offset			= point_struct_size * vindex;
					float3 pos		(	*(raw_points + offset + 0), 
										*(raw_points + offset + 1), 
										*(raw_points + offset + 2) );

					MFloatVector	v = face_normals[local_vertex_index];
					float3 norm		( v.x*normal_mul, v.y*normal_mul, -v.z*normal_mul );

					float			uv[2];
					result			= itMeshPolygon.getUV(	local_vertex_index, 
															uv,
															&CurrentUVSetName );

					if(result != MS::kSuccess)
						return result;
					
					transform_maya_to_xray			( pos );
					current_surface->m_bbox.modify	( pos );
					current_surface->add_vertex		( pos, norm, float2(uv[0], 1.0f - uv[1]) );
				}
				}
			} // all triangles in face
		} // all faces
		
		MString info= "surface: ";
		info			+= surface_tri_count;
		info			+= " triangles (";
		info			+= current_surface->m_vertices.size();
		info			+= " vertices, ";
		info			+= current_surface->m_indices.size();
		info			+= " indices) shader[";
		info			+= current_surface->m_shader_name;
		info			+= "] tex[";
		info			+= current_surface->m_texture_name;
		info			+= " ] doublesided : ";
		info			+= (doublesided) ? "YES" : "NO";

		MGlobal::displayInfo( info );
	} // all PolygonSets

	return MStatus::kSuccess;
}

MObject solid_visual_writer::findShader( const MObject& setNode, MFnMesh& fnMesh, MStatus& status ) 
//Summary:	finds the shading node for the given shading group set node
//Args   :	setNode - the shading group set node
//Returns:  the shader node for setNode if found;
//			MObject::kNullObj otherwise
{
	status					= MStatus::kSuccess;

	MFnDependencyNode fnNode( setNode );
	MPlug shaderPlug		= fnNode.findPlug( "surfaceShader" );
			
	if (!shaderPlug.isNull()) 
	{			
		MPlugArray connectedPlugs;

		//get all the plugs that are connected as the destination of this 
		//surfaceShader plug so we can find the surface shaderNode
		//
		shaderPlug.connectedTo		( connectedPlugs, true, false, &status );

		if( !status )
		{
			MGlobal::displayError	("MPlug::connectedTo");
			return					MObject::kNullObj;
		}

		if(1 != connectedPlugs.length()) 
		{
			MGlobal::displayError("Error getting shader for: " + fnMesh.partialPathName());
		}else 
		{
			status	= MStatus::kSuccess;
			return	connectedPlugs[0].node();
		}
	}
	
	return MObject::kNullObj;
}

static MStatus get_texture(MPlug &colorPlug, MString &textureName)
{
	MStatus result;

	MItDependencyGraph itDG(colorPlug, MFn::kFileTexture,
	MItDependencyGraph::kUpstream, MItDependencyGraph::kBreadthFirst,
	MItDependencyGraph::kNodeLevel, &result);

	if(result != MStatus::kSuccess)
		return result;

	//disable automatic pruning so that we can locate a specific plug 
	itDG.disablePruningOnFilter();

	if(itDG.isDone()) 
	{
		MGlobal::displayError("shader has no connected FileTexture");
		return (MStatus::kFailure);
	}

	//retrieve the filename and pass it in to output texture information
	MObject textureNode		= itDG.thisNode( &result );

	if(result != MStatus::kSuccess) 
		return result;

	MPlug filename_plug		= MFnDependencyNode(textureNode).findPlug("fileTextureName", result);
	
	if(result != MStatus::kSuccess)
		return result;

	filename_plug.getValue(textureName);

	return MStatus::kSuccess;
}

static MStatus get_shader(MPlug &shaderPlug, MString &shaderName)
{
	MStatus result;
	short index;
	MFnEnumAttribute enm_att = shaderPlug.attribute(&result);

	if(result != MStatus::kSuccess)
		return result;

	result = shaderPlug.getValue(index);
	
	if(result != MStatus::kSuccess)
		return result;

	shaderName = enm_att.fieldName(index, &result);

	return result;
}

MStatus	solid_visual_writer::shader_props( const MObject& shader_object, 
											MString& shader_name, 
											MString& texture_name, 
											bool& doublesided )
{
	MStatus				result;
	MFnDependencyNode	shader_dep_node(shader_object);

	MPlug colorPlug = shader_dep_node.findPlug("color", &result);

	if(result != MS::kSuccess || get_texture(colorPlug, texture_name) != MS::kSuccess)
	{
		MGlobal::displayWarning("Cannot get texture name, set to \"notexture\"");
		texture_name = "notexture";
	}

	MPlug shaderPlug = shader_dep_node.findPlug(shader_name_att, &result);

	if(result != MS::kSuccess || get_shader(shaderPlug, shader_name) != MS::kSuccess)
	{
		MGlobal::displayWarning("Cannot get shader name, set to \"default\"");
		shader_name = "default";
	}

	MPlug doublesidedPlug = shader_dep_node.findPlug(doublesided_att_name, &result);

	if(result != MS::kSuccess || doublesidedPlug.getValue(doublesided) != MS::kSuccess)
	{
		MGlobal::displayWarning("Cannot get doublesided flag, set to false");
		doublesided = false;
	}

	MStringArray rgFolders;

	if (texture_name.index('\\') != -1) 
		texture_name.split('\\', rgFolders);
	else
		texture_name.split('/', rgFolders);

	int count = rgFolders.length();

	if(count < 2)
		texture_name = rgFolders[0];
	else
		texture_name = rgFolders[count-2] + "/" + rgFolders[count-1];

	// cut file extension
	texture_name	= texture_name.toLowerCase();

	int ext	= texture_name.index('.');

	if(ext > 0)
		texture_name = texture_name.substring(0, ext-1 );

	return MS::kSuccess;
}

MStatus solid_visual_writer::extract_collision_geometry( MDagPath& fnDagPath )
{
	m_collision_surface.clear	( );
	MFnMesh fnMesh				( fnDagPath );

	MObjectArray				PolygonSets;
	MObjectArray				PolygonComponents;

	MStatus						result;

	MIntArray					triangles;
	MIntArray					triangle_vertices;

	result						= fnMesh.getTriangles( triangles, triangle_vertices );
	CHK_STAT_R					(result);

	const float* raw_points		= fnMesh.getRawPoints( &result );
	CHK_STAT_R					(result);
	
	int surface_tri_count			= 0;
    //Make sure the set is a polygonal set.  If not, continue.
	MItMeshPolygon itMeshPolygon	(fnDagPath, MObject::kNullObj, &result);
	CHK_STAT_R						(result);
	
	u32 point_struct_size		= 3 ; // raw_points pointer float[3]

	for (itMeshPolygon.reset(); !itMeshPolygon.isDone(); itMeshPolygon.next())
	{
		bool b_has_valid_triangulation = itMeshPolygon.hasValidTriangulation();
		ASSERT					(b_has_valid_triangulation);

		u32 face_idx			= itMeshPolygon.index();

		MIntArray				face_vertex_list;
		result					= itMeshPolygon.getVertices( face_vertex_list );
		CHK_STAT_R				(result);

		u32 tri_count			= triangles[face_idx];
		surface_tri_count		+= tri_count;
		u32 tri_idx				= 0;
		bool doublesided		= false;

		for(tri_idx=0; tri_idx<tri_count; ++tri_idx)
		{
			int vertexIndicies[3] = {-1};
			result				= fnMesh.getPolygonTriangleVertices(face_idx, tri_idx, vertexIndicies);
			CHK_STAT_R			(result);
			

			static int vert_remap_021[] = {0,2,1};
			static int vert_remap_012[] = {0,1,2};

			int const* vert_remap			= vert_remap_021;
			for(int d=0; d<2; ++d)
			{
				if(d==1)
				{
					if(!doublesided)
						break;
					vert_remap			= vert_remap_012;
				}

				for(int v_c=0; v_c<3; ++v_c)
				{
					int vc					= vert_remap[v_c];
					int vindex				= vertexIndicies[vc];

					int offset			= point_struct_size * vindex;
					float3 pos		(	*(raw_points + offset + 0), 
										*(raw_points + offset + 1), 
										*(raw_points + offset + 2) );
					
					transform_maya_to_xray( pos );
					m_collision_surface.add_vertex ( pos );
				}
			}
		} // all triangles in polygon
	} // all polygons
		
	MString info= "surface: ";
	info			+= surface_tri_count;
	info			+= " triangles (";
	info			+= m_collision_surface.m_vertices.size();
	info			+= " vertices, ";
	info			+= m_collision_surface.m_indices.size();
	info			+= " indices)";
	MGlobal::displayInfo( info );

	return MStatus::kSuccess;
}

MStatus solid_visual_writer::build_collision_geometry_by_render( )
{
	m_collision_surface.clear	( );
	
	render_surfaces_it it		= m_render_surfaces.begin();
	render_surfaces_it it_e		= m_render_surfaces.end();
	for(; it!=it_e; ++it)
	{
		render_surface* s		= *it;
		u32 count				= s->m_indices.size();
		
		for(u32 i=0; i<count; ++i)
			m_collision_surface.add_vertex( s->m_vertices[ s->m_indices[i] ] );
	}

	return MStatus::kSuccess;
}
