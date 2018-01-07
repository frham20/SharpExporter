#ifndef SHARP_MODELEXPORTER_H
#define SHARP_MODELEXPORTER_H

#include "Mesh.h"

namespace Sharp
{
	class ModelExporter : public MPxFileTranslator
	{
	public:
		ModelExporter();
		virtual ~ModelExporter();

		//From MPxFileTranslator
		virtual MStatus reader(const  MFileObject & file, const  MString & optionsString,  FileAccessMode  mode);
		virtual MStatus writer(const MFileObject & file, const MString & optionsString, FileAccessMode mode);
		virtual bool haveReadMethod() const;
		virtual bool haveWriteMethod() const;
		virtual bool haveReferenceMethod() const;
		virtual MString defaultExtension() const;
		virtual MString filter() const;
		virtual MPxFileTranslator::MFileKind identifyFile(const MFileObject & file, const char* buffer, short size) const;

	public:
		//plugin creation function
		static void* Create();
	};
};

#endif