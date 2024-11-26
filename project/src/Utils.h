#pragma once
#include <fstream>
#include "Maths.h"
#include "DataTypes.h"

namespace dae
{
	struct Triangle;

	namespace GeometryUtils
	{
#pragma region Sphere HitTest
		//SPHERE HIT-TESTS
		inline bool HitTest_Sphere(const Sphere& sphere, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			Vector3 lRay(ray.origin, sphere.origin);

			float tca = Vector3::Dot(lRay, ray.direction);

			float od{ Vector3::Reject(lRay,ray.direction).SqrMagnitude() }; 

			if (od > sphere.radius * sphere.radius)
			{
				hitRecord.didHit = false;
				return false;
			}

			hitRecord.didHit = true;
			hitRecord.materialIndex = sphere.materialIndex;

			float thc{ float(sqrt(sphere.radius*sphere.radius - od)) };

			float t = tca - thc;

			if (t < ray.min || t > ray.max)
			{
				t = tca + thc;
				if (t < ray.min || t > ray.max) return false;
			}

			if (!ignoreHitRecord)
			{
				hitRecord.t = t;
				hitRecord.didHit = true;
				hitRecord.origin = ray.origin + hitRecord.t * ray.direction;
				hitRecord.normal = (hitRecord.origin - sphere.origin).Normalized();
				hitRecord.materialIndex = sphere.materialIndex;
			}

			return true;
				
			
		}

		inline bool HitTest_Sphere(const Sphere& sphere, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Sphere(sphere, ray, temp, true);
		}

#pragma endregion

#pragma region Plane HitTest
		//PLANE HIT-TESTS
		inline bool HitTest_Plane(const Plane& plane, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			float t = Vector3::Dot(plane.origin - ray.origin, plane.normal) / Vector3::Dot(ray.direction ,plane.normal);

			if (t >= ray.min && t <= ray.max)
			{
				Vector3 p{ ray.origin + t * ray.direction };
				hitRecord.didHit = true;
				hitRecord.t = t;
				hitRecord.materialIndex = plane.materialIndex;
				hitRecord.normal = plane.normal;

				
				hitRecord.origin = p;
				return true;
			}

			hitRecord.didHit = false;
			return false;
			
		}

		inline bool HitTest_Plane(const Plane& plane, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Plane(plane, ray, temp, true);
		}

#pragma endregion

#pragma region Triangle HitTest
		//TRIANGLE HIT-TESTS
		inline bool HitTest_Triangle(const Triangle& triangle, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
				const Vector3 edge1 = triangle.v1 - triangle.v0;
				const Vector3 edge2 = triangle.v2 - triangle.v0;

				const Vector3 h = Vector3::Cross(ray.direction, edge2);
				const float a = Vector3::Dot(edge1, h);

				if (a > -FLT_EPSILON && a < FLT_EPSILON)  // ray parallel to triangle
				{
					hitRecord.didHit = false;
					return false;
				}

				const float f = 1 / a;
				const Vector3 s = ray.origin - triangle.v0;
				const float u = f * Vector3::Dot(s, h);

				if (u < 0 || u > 1) {
					hitRecord.didHit = false;
					return false;
				}
				const Vector3 q = Vector3::Cross(s, edge1);
				const float v = f * Vector3::Dot(ray.direction, q);

				if (v < 0 || u + v > 1)
				{
					hitRecord.didHit = false;
					return false;
				}

				const float t = f * Vector3::Dot(edge2, q);

				auto fill_hitRecord = [&hitRecord,triangle,ray,t]()
				{
						hitRecord.t = std::min(hitRecord.t, t);
						hitRecord.normal = triangle.normal;
						hitRecord.origin = ray.origin + ray.direction * t;
						hitRecord.materialIndex = triangle.materialIndex;
						hitRecord.didHit = true;
				};

				
				if (t > ray.min && t < ray.max)
				{
				

					switch (triangle.cullMode)
					{
					case TriangleCullMode::NoCulling:
						if (!ignoreHitRecord)
						{
							fill_hitRecord();
						}
						return true;
					case TriangleCullMode::BackFaceCulling:
						if (!ignoreHitRecord)
						{
							if (a < 0.001)
							{
								return false;
							}

							fill_hitRecord();
							return true;
						}

						if (a < 0.001)
						{
							return true;
						}

						return false;
					case TriangleCullMode::FrontFaceCulling:
						if (ignoreHitRecord == false)
						{
							if (a > 0.001)
							{
								return false;
							}

							fill_hitRecord();

							return true;
						}

						if (a > 0.001)
						{
							return true;
						}

						return false;
					}
				}

				return false;
		}

		inline bool HitTest_Triangle(const Triangle& triangle, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Triangle(triangle, ray, temp, true);
		}
#pragma endregion


#pragma region  BVH HitTest

		inline bool IntersectAABB(const Ray& ray, const Vector3 bmin, const Vector3 bmax,HitRecord& hitRecord)
		{
			float tx1 = (bmin.x - ray.origin.x) / ray.direction.x, tx2 = (bmax.x - ray.origin.x) / ray.direction.x;
			float tmin = std::min(tx1, tx2), tmax = std::max(tx1, tx2);

			float ty1 = (bmin.y - ray.origin.y) / ray.direction.y, ty2 = (bmax.y - ray.origin.y) / ray.direction.y;
			tmin = std::max(tmin, std::min(ty1, ty2)), tmax = std::min(tmax, std::max(ty1, ty2));

			float tz1 = (bmin.z - ray.origin.z) / ray.direction.z, tz2 = (bmax.z - ray.origin.z) / ray.direction.z;
			tmin = std::max(tmin, std::min(tz1, tz2)), tmax = std::min(tmax, std::max(tz1, tz2));

			return tmax >= tmin && tmin < ray.max && tmax > 0;
		}

#pragma endregion

#pragma region TriangeMesh HitTest

		inline bool SlabTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray)
		{
			float tx1 = (mesh.transformedMinAABB.x - ray.origin.x) / ray.direction.x;
			float tx2 = (mesh.transformedMaxAABB.x - ray.origin.x) / ray.direction.x;

			float tmin = std::min(tx1, tx2);
			float tmax = std::max(tx1, tx2);

			float ty1 = (mesh.transformedMinAABB.y - ray.origin.y) / ray.direction.y;
			float ty2 = (mesh.transformedMaxAABB.y - ray.origin.y) / ray.direction.y;

			tmin = std::max(tmin, std::min(ty1, ty2));
			tmax = std::min(tmax, std::max(ty1, ty2));

			float tz1 = (mesh.transformedMinAABB.z - ray.origin.z) / ray.direction.z;
			float tz2 = (mesh.transformedMaxAABB.z - ray.origin.z) / ray.direction.z;

			tmin = std::max(tmin, std::min(tz1, tz2));
			tmax = std::min(tmax, std::max(tz1, tz2));

			return tmax > 0 && tmax >= tmin;

		}

		inline bool HitTest_TriangleMesh(const TriangleMesh& mesh, const int nodeIdx, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			const BVHNode& node = mesh.bvh->GetBvhNodes(nodeIdx);

			
			if (!IntersectAABB(ray, node.aabb.bmin, node.aabb.bmax,hitRecord)) return false;


			if (node.IsLeaf())
			{
				HitRecord newClosestHit{};
					for (int triIdxOffset{}; triIdxOffset < node.triCount; ++triIdxOffset)
				{
					int triIdx = (node.firstTriIdx + triIdxOffset);

					Triangle triangle(mesh.bvh->GetTriAtIdx(triIdx).vertex0, mesh.bvh->GetTriAtIdx(triIdx).vertex1, mesh.bvh->GetTriAtIdx(triIdx).vertex2, mesh.bvh->GetTriAtIdx(triIdx).normals);
					triangle.cullMode = mesh.cullMode;
					triangle.materialIndex = mesh.materialIndex;

					HitTest_Triangle(triangle, ray, newClosestHit);
					if (newClosestHit.didHit)
					{
						if (newClosestHit.t < hitRecord.t) hitRecord = newClosestHit;
					}
				}
				return hitRecord.didHit;
			}

			else
			{
				bool hitLeft = false, hitRight = false;

				hitLeft = HitTest_TriangleMesh(mesh, node.leftNode, ray, hitRecord);
				hitRight = HitTest_TriangleMesh(mesh, node.leftNode +1, ray, hitRecord);

				return hitLeft || hitRight;
			}
		}
		

		inline bool HitTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_TriangleMesh(mesh, 0 ,ray, temp, true);
		}
#pragma endregion

	}

	namespace LightUtils
	{
		//Direction from target to light
		inline Vector3 GetDirectionToLight(const Light& light, const Vector3 origin)
		{
			Vector3 lightVec(origin, light.origin);
			return lightVec;
		}

		inline ColorRGB GetRadiance(const Light& light, const Vector3& target)
		{
			if (light.type == LightType::Directional)
			{
				return ColorRGB { light.color * light.intensity };
			}
			
			return ColorRGB{ light.color * (light.intensity / (Vector3(target,light.origin).SqrMagnitude()) ) };
			
		}
	}

	namespace Utils
	{
		//Just parses vertices and indices
#pragma warning(push)
#pragma warning(disable : 4505) //Warning unreferenced local function
		static bool ParseOBJ(const std::string& filename, std::vector<Vector3>& positions, std::vector<Vector3>& normals, std::vector<int>& indices)
		{
			std::ifstream file(filename);
			if (!file)
				return false;

			std::string sCommand;
			// start a while iteration ending when the end of file is reached (ios::eof)
			while (!file.eof())
			{
				//read the first word of the string, use the >> operator (istream::operator>>) 
				file >> sCommand;
				//use conditional statements to process the different commands	
				if (sCommand == "#")
				{
					// Ignore Comment
				}
				else if (sCommand == "v")
				{
					//Vertex
					float x, y, z;
					file >> x >> y >> z;
					positions.push_back({ x, y, z });
				}
				else if (sCommand == "f")
				{
					float i0, i1, i2;
					file >> i0 >> i1 >> i2;

					indices.push_back((int)i0 - 1);
					indices.push_back((int)i1 - 1);
					indices.push_back((int)i2 - 1);
				}
				//read till end of line and ignore all remaining chars
				file.ignore(1000, '\n');

				if (file.eof())
					break;
			}

			//Precompute normals
			for (uint64_t index = 0; index < indices.size(); index += 3)
			{
				uint32_t i0 = indices[index];
				uint32_t i1 = indices[index + 1];
				uint32_t i2 = indices[index + 2];

				Vector3 edgeV0V1 = positions[i1] - positions[i0];
				Vector3 edgeV0V2 = positions[i2] - positions[i0];
				Vector3 normal = Vector3::Cross(edgeV0V1, edgeV0V2);

				if (std::isnan(normal.x))
				{
					int k = 0;
				}

				normal.Normalize();
				if (std::isnan(normal.x))
				{
					int k = 0;
				}

				normals.push_back(normal);
			}

			return true;
		}

#pragma warning(pop)
	}
}
