#ifndef __ENGINE_STRUCT_H__
#define __ENGINE_STRUCT_H__

namespace Engine{
	/////////////////////////////////////////////////////
	/* 버텍스 구조체 */
	typedef struct tagVertexColor{
		D3DXVECTOR3 vPos;
		unsigned long dwColor;
	}_VERTEX_COLOR;

	typedef struct tagVertexTexNoraml{
		D3DXVECTOR3 vPos;
		D3DXVECTOR3 vNormal;
		D3DXVECTOR2 vTextureCoord;
	}_VERTEX_TEX_NORMAL;

	typedef struct tagVertexTexCube{
		D3DXVECTOR3 vPos;
		D3DXVECTOR3 vTextureCoord;
	}_VERTEX_TEX_CUBE;

	typedef struct tagVertexViewportTex{
		D3DXVECTOR4 vPos;
		D3DXVECTOR2 vTextureCoord;
	}_VERTEX_VIEWPORT_TEX;

	/////////////////////////////////////////////////////
	/* 삼각형 인덱스 구조체 */
	typedef struct tagIndex16{
		unsigned short _1, _2, _3;
	}INDEX16;

	typedef struct tagIndex32{
		unsigned long _1, _2, _3;
	}INDEX32;

	/////////////////////////////////////////////////////
	/* 폴리곤을 이루는 각 정점의 위치를 보관하는 구조체*/
	typedef struct tagPolygonPosition{
		_vec3 _1, _2, _3;
	}POLYGON_POSITION;

	/////////////////////////////////////////////////////
	/* 메쉬의 텍스쳐를 보관하는 구조체*/
	typedef struct tagMeshTexture{
		enum TEXTURE_TYPE{
			TYPE_DIFFUSE, TYPE_NORMAL, TYPE_SPECULAR, TYPE_EMISSIVE, TYPE_END
		};
		LPDIRECT3DTEXTURE9 pMeshTexture[TYPE_END];

		LPDIRECT3DTEXTURE9 operator[](TEXTURE_TYPE eType){
			return pMeshTexture[eType];
		}
	}MESH_TEXTURE;

	///////////////////////////////////////////////////////
	/* 다이나믹 메쉬의 뼈 정보를 보관하는 구조체 */
	/* 원래 있는 D3DXFRAME를 상속받아서 행렬을 보관 할 수 있도록 만듬. */
	typedef struct tagD3DXFrame_Derived : public D3DXFRAME
	{
		D3DXMATRIX CombinedTransformationMatrix;
	}D3DXFRAME_DERIVED;

	///////////////////////////////////////////////////////
	/* 다이나믹 메쉬의 메쉬 정보를 보관하는 구조체 */
	/* 원래 있는 D3DXMESHCONTAINER를 상속받아서 메쉬의 텍스쳐를 보관 할 수 있도록 만듬. */
	typedef struct tagMeshContainer_Derived : public D3DXMESHCONTAINER{
		MESH_TEXTURE* pMeshTexture;
		LPD3DXMESH pOriginalMesh;
		_ulong dwNumFrames;
		D3DXMATRIX* pOffsetMatrices;
		D3DXMATRIX** ppCombinedTransformationMatrices;
		D3DXMATRIX* pRenderingMatrices;
	}D3DXMESHCONTAINER_DERIVED;

};

#endif // !__ENGINE_STRUCT_H__
