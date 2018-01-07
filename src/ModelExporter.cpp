#include "SharpExporter.h"
#include "ModelExporter.h"
#include "Mesh.h"
#include <cstdio>


namespace Sharp
{
	ModelExporter::ModelExporter()
	{

	}

	ModelExporter::~ModelExporter()
	{

	}

	MStatus ModelExporter::reader(const  MFileObject & file, const  MString & optionsString,  FileAccessMode  mode)
	{
		MStatus status;
		return status;
	}

	MStatus ModelExporter::writer(const MFileObject & file, const MString & optionsString, FileAccessMode mode)
	{
		/*
		//open file for writing
		MString path = file.resolvedFullName();

		FILE* fp;
		fopen_s(&fp, path.asChar(), "wb");
		if(fp == NULL)
			return MStatus::kFailure;

		fclose(fp);
		*/

		return MStatus::kSuccess;
	}

	bool ModelExporter::haveReadMethod() const
	{
		return false;
	}

	bool ModelExporter::haveWriteMethod() const
	{
		return true;
	}

	bool ModelExporter::haveReferenceMethod() const
	{
		return true;
	}

	MString ModelExporter::defaultExtension() const
	{
		return "smdl";
	}

	MString ModelExporter::filter() const
	{
		return "*.smdl";
	}

	MPxFileTranslator::MFileKind ModelExporter::identifyFile(const MFileObject & file, const char* buffer, short size) const
	{
		return kIsMyFileType;
	}

	void* ModelExporter::Create()
	{
		return new ModelExporter();
	}
};
