#ifndef SHARP_ENVRENDEREXPORTER_H
#define SHARP_ENVRENDEREXPORTER_H

namespace Sharp
{
	class EnvRenderExporter : public MPxFileTranslator
	{
	public:
		EnvRenderExporter();
		virtual ~EnvRenderExporter();

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

	private:
		struct EnvMesh
		{
			uint32 uiVertexBufferOffset;
			uint32 uiIndexOffset;
			uint32 uiVertexFormat;
			uint32 uiVertexCount;
			uint32 uiPrimCount;
			uint8  uiPrimType;
			uint8  uiVertexStride;
			uint8  uiPad[2];
		};
	};
}

#endif