#include "SharpExporter.h"
#include "Mesh.h"

namespace Sharp
{
	Mesh::Mesh()
	: muiVertexFormat(SHARP_VF_POSITION | SHARP_VF_NORMAL)
	{
	}

	Mesh::Mesh(MDagPath _dagPath, MSpace::Space _vertexSpace)
	{
		Create(_dagPath, _vertexSpace);
	}

	Mesh::~Mesh()
	{
	}

	bool Mesh::Create(MDagPath _dagPath, MSpace::Space _vertexSpace)
	{
		MFnMesh meshFn(_dagPath);

		//retrieve vertices elements
		MPointArray srcVertexArray;
		MFloatVectorArray srcNormalArray;
		MColorArray srcColorArray;

		meshFn.getPoints(srcVertexArray, _vertexSpace);
		meshFn.getNormals(srcNormalArray, _vertexSpace);
		meshFn.getColors(srcColorArray);

		if(srcVertexArray.length() == 0)
			return false;

		//now build our face array
		MItMeshPolygon iterPolygon(_dagPath);

		//count number of faces (triangles)
		//a polygon is considered a planar triangle fan
		unsigned int uiFaceCount = 0;
		for(;!iterPolygon.isDone(); iterPolygon.next())
			uiFaceCount += iterPolygon.polygonVertexCount() - 2;

		if(uiFaceCount == 0)
			return false;

		//get all UV data from all UV sets
		uint32 uiUVSetCount = (uint32)meshFn.numUVSets();
		MStringArray uvSetNameArray;
		MFloatArray* pSrcUArrays = NULL;
		MFloatArray* pSrcVArrays = NULL;
		if(uiUVSetCount > 0)
		{
			meshFn.getUVSetNames(uvSetNameArray);

			//get all the UV arrays
			pSrcUArrays = new MFloatArray[uiUVSetCount];
			pSrcVArrays = new MFloatArray[uiUVSetCount];
			for(uint32 i = 0; i < uiUVSetCount; i++)
				meshFn.getUVs(pSrcUArrays[i], pSrcVArrays[i], &uvSetNameArray[i]);
		}

		muiVertexFormat = SHARP_VF_POSITION | SHARP_VF_NORMAL | SHARP_VF_TEXCOORD(uiUVSetCount);

		//create our mesh
		mFaces.resize(uiFaceCount);

		//now triangulate our mesh
		iterPolygon.reset();
		unsigned int uiCurrentFace = 0;
		for(;!iterPolygon.isDone(); iterPolygon.next())
		{
			unsigned int uiTriCount = iterPolygon.polygonVertexCount() - 2;
			unsigned int uiRootVertexIndex = iterPolygon.vertexIndex(0);
			unsigned int uiCurVertex = 1;
			for(unsigned int uiTriIndex = 0; uiTriIndex < uiTriCount; uiTriIndex++)
			{
				Face& face = mFaces[uiCurrentFace];
				face.vertices[0].posIndex = uiRootVertexIndex;
				face.vertices[1].posIndex = iterPolygon.vertexIndex(uiCurVertex);
				face.vertices[2].posIndex = iterPolygon.vertexIndex(uiCurVertex+1);

				face.vertices[0].normalIndex = iterPolygon.normalIndex(0);
				face.vertices[1].normalIndex = iterPolygon.normalIndex(uiCurVertex);
				face.vertices[2].normalIndex = iterPolygon.normalIndex(uiCurVertex+1);

				for(uint32 uiUVSetIndex = 0; uiUVSetIndex < uiUVSetCount; uiUVSetIndex++)
				{
					iterPolygon.getUVIndex(0, face.vertices[0].uvIndex[uiUVSetIndex], &uvSetNameArray[uiUVSetIndex]);
					iterPolygon.getUVIndex(uiCurVertex, face.vertices[1].uvIndex[uiUVSetIndex], &uvSetNameArray[uiUVSetIndex]);
					iterPolygon.getUVIndex(uiCurVertex+1, face.vertices[2].uvIndex[uiUVSetIndex], &uvSetNameArray[uiUVSetIndex]);
				}

				uiCurVertex++;
				uiCurrentFace++;
			}
		}

		//now that we triangulated the whole mesh create the real vertex buffer
		unsigned int uiVertexCount = 0;
		mVertices.resize(uiFaceCount * 3);
		unsigned int* puiVertexMap = new unsigned int[srcVertexArray.length()];
		memset(puiVertexMap, 0xFFFFFFFF, sizeof(unsigned int) * srcVertexArray.length());

		//for each face
		for(unsigned int uiFaceIndex = 0; uiFaceIndex < uiFaceCount; uiFaceIndex++)
		{
			Face& face = mFaces[uiFaceIndex];

			//for each vertex in the face
			for(unsigned int uiFaceVertexIndex = 0; uiFaceVertexIndex < 3; uiFaceVertexIndex++)
			{
				FaceVertex& faceVertex = face.vertices[uiFaceVertexIndex];

				//vertex not added yet? add it
				if(puiVertexMap[faceVertex.posIndex] == 0xFFFFFFFF)
				{
					Vertex& vertex = mVertices[uiVertexCount];
					srcVertexArray[faceVertex.posIndex].get(vertex.pos);
					srcNormalArray[faceVertex.normalIndex].get(vertex.normal);

					for(uint32 uiUVSetIndex = 0; uiUVSetIndex < uiUVSetCount; uiUVSetIndex++)
					{
						uint32 uiUVIndex = faceVertex.uvIndex[uiUVSetIndex];
						vertex.uv[uiUVSetIndex][0] = pSrcUArrays[uiUVSetIndex][uiUVIndex];
						vertex.uv[uiUVSetIndex][1] = 1.0f - pSrcVArrays[uiUVSetIndex][uiUVIndex];
					}

					vertex.srcFaceVertex = faceVertex;
					vertex.uiNextIndex = 0xFFFFFFFF;

					puiVertexMap[faceVertex.posIndex] = uiVertexCount;
					faceVertex.posIndex = uiVertexCount;
					uiVertexCount++;
				}
				//if vertex already exists
				else
				{
					//make sure it matches everything, else we need to generate a new one
					Vertex* pVertex = &mVertices[puiVertexMap[faceVertex.posIndex]];

					//try to find if there isn't already another vertex using the same src
					unsigned int uiLastIndex = puiVertexMap[faceVertex.posIndex];
					unsigned int uiVIndex = uiLastIndex;
					while(uiVIndex != 0xFFFFFFFF)
					{				
						const Vertex& vertex = mVertices[uiVIndex];
						if(vertex.srcFaceVertex == faceVertex)
						{
							//we found it! so just re-assign face vertex index
							faceVertex.posIndex = uiVIndex;
							break;
						}

						uiLastIndex = uiVIndex;
						uiVIndex = vertex.uiNextIndex;
					}

					//no match found so add new vertex
					if(uiVIndex == 0xFFFFFFFF)
					{
						Vertex& vertex = mVertices[uiVertexCount];
						srcVertexArray[faceVertex.posIndex].get(vertex.pos);
						srcNormalArray[faceVertex.normalIndex].get(vertex.normal);

						for(uint32 uiUVSetIndex = 0; uiUVSetIndex < uiUVSetCount; uiUVSetIndex++)
						{
							uint32 uiUVIndex = faceVertex.uvIndex[uiUVSetIndex];
							vertex.uv[uiUVSetIndex][0] = pSrcUArrays[uiUVSetIndex][uiUVIndex];
							vertex.uv[uiUVSetIndex][1] = 1.0f - pSrcVArrays[uiUVSetIndex][uiUVIndex];
						}

						vertex.srcFaceVertex = faceVertex;
						vertex.uiNextIndex = 0xFFFFFFFF;

						mVertices[uiLastIndex].uiNextIndex = uiVertexCount;
						puiVertexMap[faceVertex.posIndex] = uiVertexCount;
						faceVertex.posIndex = uiVertexCount;
						uiVertexCount++;								
					}
				}
			}
		}

		//release our vertex map
		delete[] puiVertexMap;
		delete[] pSrcUArrays;
		delete[] pSrcVArrays;

		//resize our vertex buffer to the real size
		mVertices.resize(uiVertexCount);

		return uiVertexCount > 0;
	}

	uint32 Mesh::GetVertexFormat() const
	{
		return muiVertexFormat;
	}

	uint32 Mesh::GetVertexCount() const
	{
		return (uint32)mVertices.size();
	}

	uint32 Mesh::GetFaceCount() const
	{
		return (uint32)mFaces.size();
	}

	Mesh::Vertex& Mesh::GetVertex(uint32 _uiVertex)
	{
		return mVertices[_uiVertex];
	}

	const Mesh::Vertex& Mesh::GetVertex(uint32 _uiVertex) const
	{
		return mVertices[_uiVertex];
	}

	Mesh::Face& Mesh::GetFace(uint32 _uiFace)
	{
		return mFaces[_uiFace];
	}

	const Mesh::Face& Mesh::GetFace(uint32 _uiFace) const
	{
		return mFaces[_uiFace];
	}

	uint32 Mesh::GetVertexStride() const
	{
		uint32 uiStride = 0;
		if(muiVertexFormat & SHARP_VF_POSITION)
			uiStride += sizeof(float) * 3;

		if(muiVertexFormat & SHARP_VF_NORMAL)
			uiStride += sizeof(float) * 3;

		if(muiVertexFormat & SHARP_VF_COLOR)
			uiStride += sizeof(uint32);

		if(muiVertexFormat & SHARP_VF_TANGENT)
			uiStride += sizeof(float) * 4;

		uint32 uiTexCoordCount = (muiVertexFormat & SHARP_VF_TEXCOORD_MASK) >> SHARP_VF_TEXCOORD_SHIFT;
		uiStride += sizeof(float) * uiTexCoordCount * 2;

		return uiStride;
	}

	Mesh::FaceVertex::FaceVertex()
	: posIndex(0), 
	  normalIndex(0), 
	  colorIndex(0)
	{
		memset(uvIndex, 0, sizeof(uint32) * 8);
	}

	bool Mesh::FaceVertex::operator==(const FaceVertex& fv) const
	{
		return (posIndex == fv.posIndex &&
			   normalIndex == fv.normalIndex &&
			   colorIndex == fv.colorIndex &&
			   uvIndex[0] == fv.uvIndex[0] &&
			   uvIndex[1] == fv.uvIndex[1] &&
			   uvIndex[2] == fv.uvIndex[2] &&
			   uvIndex[3] == fv.uvIndex[3] &&
			   uvIndex[4] == fv.uvIndex[4] &&
			   uvIndex[5] == fv.uvIndex[5] &&
			   uvIndex[6] == fv.uvIndex[6] &&
			   uvIndex[7] == fv.uvIndex[7]);
	}
}