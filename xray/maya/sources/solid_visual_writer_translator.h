#ifndef SOLID_VISUAL_WRITER_TRANSLATOR_H
#define SOLID_VISIAL_WRITER_TRANSLATOR_H

class solid_visual_file_translator: public MPxFileTranslator
{
	public:

	static void * creator(void);
	virtual MStatus writer(const MFileObject &file, const MString &options, FileAccessMode mode);

	virtual bool haveWriteMethod() const
	{
		return true;
	}

	virtual MString defaultExtension() const
	{
		return "ogf";
	}

	virtual MString filter() const
	{
		return "*.ogf";
	}
};

#endif