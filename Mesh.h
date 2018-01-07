#ifndef SHARP_MESH_H
#define SHARP_MESH_H

#define SHARP_VF_POSITION		0x00000001
#define SHARP_VF_NORMAL			0x00000002
#define SHARP_VF_COLOR			0x00000004
#define SHARP_VF_TANGENT		0x00000008

#define SHARP_VF_TEXCOORD_MASK	0xFF000000
#define SHARP_VF_TEXCOORD_SHIFT	24
#define SHARP_VF_TEXCOORD(n)	(n<<SHARP_VF_TEXCOORD_SHIFT)

namespace Sharp
{
	class Mesh
	{
	public:
		struct Vertex;
		struct Face;

		Mesh();
		Mesh(MDagPath _dagPath, MSpace::Space _vertexSpace);
		~Mesh();

		bool Create(MDagPath _dagPath, MSpace::Space _vertexSpace);

		uint32 GetVertexCount() const;
		uint32 GetFaceCount() const;
		
		uint32 GetVertexStride() const;
		uint32 GetVertexFormat() const;

		Vertex& GetVertex(uint32 _uiVertex);
		const Vertex& GetVertex(uint32 _uiVertex) const;

		Face& GetFace(uint32 _uiFace);
		const Face& GetFace(uint32 _uiFace) const;

	public:

		struct FaceVertex
		{
			FaceVertex();
			bool operator==(const FaceVertex& fv) const;
			int posIndex;
			int normalIndex;
			int colorIndex;
			int uvIndex[8];
		};

		struct Vertex 
		{
			float  pos[4];
			float  normal[3];
			uint32 color;
			float  uv[8][2];

			FaceVertex srcFaceVertex;
			uint32 uiNextIndex;	//next vertex in array that has the same source vertex
		};

		struct Face
		{
			FaceVertex vertices[3];
		};

	private:
		uint32 muiVertexFormat;
		std::vector<Vertex> mVertices;
		std::vector<Face>   mFaces;

	};
}

#endif