#include <smesh.h>
#include <TangentSpaceCalculation.h>

struct smesh_triangle_proxy : public ITriangleInputProxy
{
	node& m_node;

	smesh_triangle_proxy(node& node)
	:m_node(node)
	{

	}

	//! not virtual to save the call overhead
	//! /return 0..
	unsigned int GetTriangleCount() const
	{
		return m_node.m_index.size() / 3;
	}

	//! not virtual to save the call overhead
	//! /param indwTriNo 0..
	//! /param outdwPos
	//! /param outdwNorm
	//! /param outdwUV
	void GetTriangleIndices(
		const unsigned int indwTriNo,
		unsigned int outdwPos[3],
		unsigned int outdwNorm[3],
		unsigned int outdwUV[3]) const
	{
		for(unsigned char i = 0; i != 3; ++i)
			outdwPos[i] = outdwNorm[i] = outdwUV[i] = m_node.m_index[indwTriNo * 3 + i];
	}

	//! not virtual to save the call overhead
	//! /param indwPos 0..
	//! /param outfPos
	void GetPos(const unsigned int indwPos, float outfPos[3]) const
	{
		outfPos[0] = m_node.m_vertex[indwPos].m_position.x;
		outfPos[1] = m_node.m_vertex[indwPos].m_position.y;
		outfPos[2] = m_node.m_vertex[indwPos].m_position.z;
	}

	//! not virtual to save the call overhead
	//! /param indwPos 0..
	//! /param outfUV 
	void GetUV(const unsigned int indwPos, float outfUV[2]) const
	{
		outfUV[0] = m_node.m_vertex[indwPos].m_uvmap.x;
		outfUV[1] = m_node.m_vertex[indwPos].m_uvmap.y;
	}
};


bool compute_tangent_space(model& ml)
{
	bool success = true;
	//compute
	for (node& nd : ml.m_nodes)
	{
		//proxy
		smesh_triangle_proxy proxy(nd);
		//compute
		CTangentSpaceCalculation<smesh_triangle_proxy> c_tan_space;
		success &= c_tan_space.CalculateTangentSpace(proxy) == 0;
		//get...
		for (unsigned int i = 0; i != c_tan_space.GetBaseCount(); ++i)
		{
			//get
			float tangent[3];
			float bitangent[3];
			float normal[3];
			c_tan_space.GetBase(i, tangent, bitangent, normal);
#if 0
			//put
			nd.m_vertex[i].m_normal.x = n[0];
			nd.m_vertex[i].m_normal.y = n[1];
			nd.m_vertex[i].m_normal.z = n[2];
#endif
			nd.m_vertex[i].m_tangent.x = tangent[0];
			nd.m_vertex[i].m_tangent.y = tangent[1];
			nd.m_vertex[i].m_tangent.z = tangent[2];

			nd.m_vertex[i].m_bitangent.x = bitangent[0];
			nd.m_vertex[i].m_bitangent.y = bitangent[1];
			nd.m_vertex[i].m_bitangent.z = bitangent[2];
		}
	}
	return success;
}