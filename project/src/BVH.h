#pragma once

#include <cstdint>
#include <vector>
#include "Vector3.h"

namespace dae
{
	struct TriangleMesh;
}


struct aabb
{
	dae::Vector3 bmin{ 1e30f, 1e30f, 1e30f };
	dae::Vector3 bmax{ -1e30f, -1e30f, -1e30f };
	void grow(dae::Vector3 p) { bmin = dae::Vector3::Min(bmin, p), bmax = dae::Vector3::Max(bmax, p); }
	float halfArea()
	{
		dae::Vector3 e = bmax - bmin; // box extent
		return e.x * e.y + e.y * e.z + e.z * e.x;
	}
};

struct BVHNode
{
	aabb aabb;
	uint32_t leftNode, firstTriIdx, triCount;

	bool IsLeaf() const
	{
		return (triCount > 0);
	};
};


struct Tri
{
	dae::Vector3 vertex0, vertex1, vertex2;
	dae::Vector3 centroid;
	dae::Vector3 normals;
};

class BVH
{
public:
	BVH(dae::TriangleMesh* triangleMesh);
	~BVH() = default;
	void BuildBVH();
	void UpdateNodeBounds(uint32_t const nodeIdx);
	void Subdivide(uint32_t const nodeIdx);
	BVHNode& GetBvhNodes(int nodeIdx);
	Tri& GetTriAtIdx(int idx);
	float EvaluateSAH(BVHNode& node, int axis, float pos);

	dae::TriangleMesh* mesh;
private:
	int NrOfTriangles{ 0 };
	std::vector<Tri>tri{};
	std::vector <BVHNode> bvhNodes;
	uint32_t rootNodeIdx = 0, nodesUsed = 1;

};
