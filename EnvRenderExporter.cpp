#include "SharpExporter.h"
#include "EnvRenderExporter.h"
#include "Mesh.h"
#include <cstdio>


namespace Sharp
{
	EnvRenderExporter::EnvRenderExporter()
	{

	}

	EnvRenderExporter::~EnvRenderExporter()
	{

	}

	MStatus EnvRenderExporter::reader(const  MFileObject & file, const  MString & optionsString,  FileAccessMode  mode)
	{
		MStatus status;
		return status;
	}

	MStatus EnvRenderExporter::writer(const MFileObject & file, const MString & optionsString, FileAccessMode mode)
	{
		//open file for writing
		MString path = file.resolvedFullName();

		FILE* fp;
		fopen_s(&fp, path.asChar(), "wb");
		if(fp == NULL)
			return MStatus::kFailure;

		std::vector<Mesh*> meshArray;

		//iterate through all meshes in the scene
		MItDag iterDag(MItDag::kDepthFirst, MFn::kMesh);
		for(;!iterDag.isDone(); iterDag.next())
		{
			MObject  meshObj = iterDag.currentItem();
			MDagPath dagPath;
			iterDag.getPath(dagPath);

			Mesh* pMesh = new Mesh();
			if(!pMesh->Create(dagPath, MSpace::kWorld))
			{
				delete pMesh;
				continue;
			}

			//add the mesh to mesh array
			meshArray.push_back(pMesh);
		}

		//write data to disk
		uint32 uiEnvRenderMagic = 0xeeeebbbb;
		fwrite(&uiEnvRenderMagic, sizeof(uint32), 1, fp);

		//build env meshes
		uint32 uiIndexOffset = 0;
		uint32 uiVertexBufferOffset = 0;
		uint32 uiVertexBufferSize = 0;
		uint32 uiIndexBufferSize = 0;
		std::vector<EnvMesh> envMeshArray(meshArray.size());
		for(uint32 i = 0; i < (uint32)meshArray.size(); i++)
		{
			const Mesh* pMesh = meshArray[i];
			EnvMesh& envMesh = envMeshArray[i];

			uint32 uiVertexStride = pMesh->GetVertexStride();

			envMesh.uiIndexOffset = uiIndexOffset;
			envMesh.uiVertexBufferOffset = uiVertexBufferOffset;
			envMesh.uiPrimType = 0;
			envMesh.uiVertexStride = uiVertexStride;
			envMesh.uiVertexFormat = pMesh->GetVertexFormat();
			envMesh.uiVertexCount = pMesh->GetVertexCount();
			envMesh.uiPrimCount = pMesh->GetFaceCount();

			uiIndexOffset += pMesh->GetFaceCount() * 3;
			uiVertexBufferOffset += pMesh->GetVertexCount() * uiVertexStride;

			uiVertexBufferSize += pMesh->GetVertexCount() * uiVertexStride;
			uiIndexBufferSize += pMesh->GetFaceCount() * 3 * sizeof(uint16);
		}

		//write vertex buffer size
		fwrite(&uiVertexBufferSize, sizeof(uint32), 1, fp);

		//write vertex buffer
		for(uint32 i = 0; i < (uint32)meshArray.size(); i++)
		{
			const Mesh* pMesh = meshArray[i];
			uint32 uiVertexFormat = pMesh->GetVertexFormat();
			for(uint32 uiVertIndex = 0; uiVertIndex < pMesh->GetVertexCount(); uiVertIndex++)
			{
				const Mesh::Vertex& vertex = pMesh->GetVertex(uiVertIndex);

				if(uiVertexFormat & SHARP_VF_POSITION)
					fwrite(vertex.pos, sizeof(float), 3, fp);
					
				if(uiVertexFormat & SHARP_VF_NORMAL)
					fwrite(vertex.normal, sizeof(float), 3, fp);
					
				if(uiVertexFormat & SHARP_VF_COLOR)
					fwrite(&vertex.color, sizeof(uint32), 1, fp);
				
				//if(uiVertexFormat & SHARP_VF_TANGENT)
				
				uint32 uiTexCoordCount = (uiVertexFormat & SHARP_VF_TEXCOORD_MASK) >> SHARP_VF_TEXCOORD_SHIFT;
				if(uiTexCoordCount > 0)
					fwrite(vertex.uv, sizeof(float), 2 * uiTexCoordCount, fp);
			}
		}

		//write index buffer size
		fwrite(&uiIndexBufferSize, sizeof(uint32), 1, fp);

		//write index buffer
		for(uint32 i = 0; i < (uint32)meshArray.size(); i++)
		{
			const Mesh* pMesh = meshArray[i];
			
			for(uint32 uiFaceIndex = 0; uiFaceIndex < pMesh->GetFaceCount(); uiFaceIndex++)
			{
				const Mesh::Face& face = pMesh->GetFace(uiFaceIndex);

				uint16 uiFaceIndices[3];
				uiFaceIndices[0] = (uint16)face.vertices[0].posIndex;
				uiFaceIndices[1] = (uint16)face.vertices[1].posIndex;
				uiFaceIndices[2] = (uint16)face.vertices[2].posIndex;
				fwrite(uiFaceIndices, sizeof(uint16), 3, fp);
			}
		}

		//write mesh infos
		uint32 uiMeshCount = (uint32)envMeshArray.size();
		fwrite(&uiMeshCount, sizeof(uint32), 1, fp);
		for(uint32 i = 0; i < uiMeshCount; i++)
		{
			const EnvMesh& envMesh = envMeshArray[i];
			fwrite(&envMesh, sizeof(EnvMesh), 1, fp);
		}

		for(uint32 i = 0; i < (uint32)meshArray.size(); i++)
			delete meshArray[i];

		fclose(fp);

		return MStatus::kSuccess;
	}

	bool EnvRenderExporter::haveReadMethod() const
	{
		return false;
	}

	bool EnvRenderExporter::haveWriteMethod() const
	{
		return true;
	}

	bool EnvRenderExporter::haveReferenceMethod() const
	{
		return true;
	}

	MString EnvRenderExporter::defaultExtension() const
	{
		return "senv";
	}

	MString EnvRenderExporter::filter() const
	{
		return "*.senv";
	}

	MPxFileTranslator::MFileKind EnvRenderExporter::identifyFile(const MFileObject & file, const char* buffer, short size) const
	{
		return kIsMyFileType;
	}

	void* EnvRenderExporter::Create()
	{
		return new EnvRenderExporter();
	}
};
