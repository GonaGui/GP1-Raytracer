#include "BVH.h"

#include <iostream>

#include "DataTypes.h"


BVH::BVH(dae::TriangleMesh* triangleMesh) :mesh{ triangleMesh }
{
	NrOfTriangles = triangleMesh->normals.size();
	tri.resize(NrOfTriangles);
	bvhNodes.resize(NrOfTriangles * 2 - 1);
}

void BVH::BuildBVH()
{
	nodesUsed = 1;
	for (int i = 0; i < NrOfTriangles; i++)
	{
		tri[i].vertex0 = mesh->transformedPositions[mesh->indices[(i * 3)]];
		tri[i].vertex1 = mesh->transformedPositions[mesh->indices[(i * 3) + 1]];
		tri[i].vertex2 = mesh->transformedPositions[mesh->indices[(i * 3) + 2]];
		tri[i].normals = mesh->transformedNormals[i];
		tri[i].centroid = (tri[i].vertex0 + tri[i].vertex1 + tri[i].vertex2) * 0.3333f;
	}

	// assign all triangles to root node
	BVHNode& root = bvhNodes[rootNodeIdx];
	root.leftNode = 0;
	root.firstTriIdx = 0, root.triCount = NrOfTriangles;
	UpdateNodeBounds(rootNodeIdx);
 
	Subdivide(rootNodeIdx);

	//std::cout << nodesUsed << std::endl;
	//uint32_t maxTriangles{ 0 };
	//uint32_t minTriangles{ 999 };

	//for (int i{0}; i < nodesUsed; i++)
	//{
	//	std::cout << "Nr of node: " << i << std::endl;
	//	std::cout << "Nr of triangles: " << bvhNodes[i].triCount << std::endl;
	//	std::cout << "Left Child Idx: " << bvhNodes[i].leftNode << std::endl<< std::endl;
	//	maxTriangles = std::max(maxTriangles, bvhNodes[i].triCount);
	//	minTriangles = std::min(minTriangles, bvhNodes[i].triCount);
	//}
	//std::cout << "Max Nr of triangles: " << maxTriangles << std::endl;
	//std::cout << "Min Nr of triangles: " << minTriangles << std::endl;

}

void BVH::UpdateNodeBounds(uint32_t const nodeIdx)
{
	BVHNode& node = bvhNodes[nodeIdx];
	uint32_t first = node.firstTriIdx;
	node.aabb.bmin = dae::Vector3(1e30f, 1e30f, 1e30f);
	node.aabb.bmax = dae::Vector3(-1e30f, -1e30f, -1e30f);
	for (int i{ 0 }; i < node.triCount; i++)
	{
		Tri& leafTri = tri[first + i];
		node.aabb.bmin = dae::Vector3::Min(node.aabb.bmin, leafTri.vertex0);
		node.aabb.bmin = dae::Vector3::Min(node.aabb.bmin, leafTri.vertex1);
		node.aabb.bmin = dae::Vector3::Min(node.aabb.bmin, leafTri.vertex2);
		node.aabb.bmax = dae::Vector3::Max(node.aabb.bmax, leafTri.vertex0);
		node.aabb.bmax = dae::Vector3::Max(node.aabb.bmax, leafTri.vertex1);
		node.aabb.bmax = dae::Vector3::Max(node.aabb.bmax, leafTri.vertex2);
	}
}

void BVH::Subdivide(uint32_t const nodeIdx)
{
	// terminate recursion
	BVHNode& node = bvhNodes[nodeIdx];
	if (node.triCount <= 3) return;

	// determine split axis using SAH
	int bestAxis = -1;
	float bestPos = 0, bestCost = 1e30f;
	const int nrOfTestsPerAxis = 5;
	for (int axis = 0; axis < 3; axis++)
	{
		for (uint32_t i = 0; i < nrOfTestsPerAxis; i++)
		{
			float IntervalJump = (i + 1) / (nrOfTestsPerAxis + 1.f);
			float possibleSplitPos = (node.aabb.bmin + (node.aabb.bmax - node.aabb.bmin) * IntervalJump)[axis];

			float cost = EvaluateSAH(node, axis, possibleSplitPos);
			if (cost < bestCost) bestPos = possibleSplitPos, bestAxis = axis, bestCost = cost;
		}
	}

	dae::Vector3 e = node.aabb.bmax - node.aabb.bmin; // extent of parent
	float parentArea = node.aabb.halfArea();
	float parentCost = node.triCount * parentArea;
	int axis = bestAxis;
	float splitPos = bestPos;

	if (parentCost <= bestCost) return;

	// in-place partition
	int i = node.firstTriIdx;
	int j = i + node.triCount - 1;
	while (i <= j)
	{
		if (tri[i].centroid[axis] < splitPos)
		{
			i++;
		}
		else
		{
			std::swap(tri[i], tri[j]);
			--j;
		}
	}
	// abort split if one of the sides is empty
	int leftCount = i - node.firstTriIdx;
	if (leftCount == 0 || leftCount == node.triCount) return;
	// create child nodes
	int leftChildIdx = nodesUsed;
	nodesUsed++;
	int rightChildIdx = nodesUsed;
	nodesUsed++;
	bvhNodes[leftChildIdx].firstTriIdx = node.firstTriIdx;
	bvhNodes[leftChildIdx].triCount = leftCount;
	bvhNodes[rightChildIdx].firstTriIdx = i;
	bvhNodes[rightChildIdx].triCount = node.triCount - leftCount;
	node.leftNode = leftChildIdx;
	node.triCount = 0;
	UpdateNodeBounds(leftChildIdx);
	UpdateNodeBounds(rightChildIdx);
	// recurse
	Subdivide(leftChildIdx);
	Subdivide(rightChildIdx);
}

BVHNode& BVH::GetBvhNodes(int nodeIdx)
{
	return bvhNodes[nodeIdx];
}

Tri& BVH::GetTriAtIdx(int idx)
{
	return tri[idx];
}

float BVH::EvaluateSAH(BVHNode& node, int axis, float pos)
{
	// determine triangle counts and bounds for this split candidate
	aabb leftBox, rightBox;
	int leftCount = 0, rightCount = 0;
	for (uint32_t i = 0; i < node.triCount; i++)
	{
		Tri& triangle = tri[node.firstTriIdx + i];
		if (triangle.centroid[axis] < pos)
		{
			leftCount++;
			leftBox.grow(triangle.vertex0);
			leftBox.grow(triangle.vertex1);
			leftBox.grow(triangle.vertex2);
		}
		else
		{
			rightCount++;
			rightBox.grow(triangle.vertex0);
			rightBox.grow(triangle.vertex1);
			rightBox.grow(triangle.vertex2);
		}
	}
	float cost = leftCount * leftBox.halfArea() + rightCount * rightBox.halfArea();
	return cost > 0 ? cost : 1e30f;
}
