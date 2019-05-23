#include "pch.h"
#include "solid_visual_writer_translator.h"
#include "solid_visual_writer.h"

void * solid_visual_file_translator::creator(void)
{
	return CRT_NEW(solid_visual_file_translator);
}

MStatus solid_visual_file_translator::writer(const MFileObject &file, const MString &options, FileAccessMode mode)
{
	MStatus r;

	if(mode != FileAccessMode::kExportActiveAccessMode)
	{
		MGlobal::displayError("Sorry, only 'Export Selected' allowed yet.");
		return MS::kNotImplemented;
	}

	MSelectionList sellist;
	MGlobal::getActiveSelectionList(sellist);

	MItSelectionList selit(sellist, MFn::kMesh, &r);

	if(r != MS::kSuccess)
		return MS::kFailure;

	if(selit.isDone())
	{
		MGlobal::displayError("Select any mesh, please...");
		return MS::kFailure;
	}

	MSyntax syntax;
	MArgList arglist;
	MArgDatabase dummy(syntax, arglist);

	solid_visual_writer w(dummy, r);

	if(r != MS::kSuccess)
		return MS::kFailure;

	while(!selit.isDone())
	{
		MDagPath mesh_dp;
		r = selit.getDagPath(mesh_dp);

		if(r != MS::kSuccess)
			return MS::kFailure;

		LOG_INFO("%s", mesh_dp.fullPathName().asChar());

		r = w.extract_render_geometry(mesh_dp);

		if(r != MS::kSuccess)
			return MS::kFailure;

		selit.next();
	}

	r = w.build_collision_geometry_by_render();

	if(r != MS::kSuccess)
		return MS::kFailure;

	xray::memory::writer mw(&xray::maya::g_allocator);

	r = w.write_render_geometry(mw);

	if(r != MS::kSuccess)
		return MS::kFailure;

	r  = w.write_collision_geometry(mw);

	if(r != MS::kSuccess)
		return MS::kFailure;

	mw.save_to(file.fullName().asChar());

	return MS::kSuccess;
}
