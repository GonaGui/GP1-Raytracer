#pragma once
#include <fstream>
#include "Maths.h"
#include "DataTypes.h"

namespace dae
{
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

			float nv(Vector3::Dot(triangle.normal, ray.direction));

			if ( nv < 0.001 && nv > -0.001)
			{
				return false;
			}

			Vector3 a(triangle.v0, triangle.v1);
			Vector3 b(triangle.v1, triangle.v2);
			Vector3 c(triangle.v2, triangle.v0);

			float t = Vector3::Dot(triangle.v0 - ray.origin, triangle.normal) / Vector3::Dot(ray.direction, triangle.normal);

			if (t >= ray.min && t <= ray.max)
			{

				Vector3 P{ ray.origin + t * ray.direction };
				const Vector3* triangleV[3] = { &triangle.v0,&triangle.v1,&triangle.v2 };
				const Vector3* triangleEdges[3]{ &a,&b,&c };

				for (int idx{ 0 }; idx < 3; idx++)
				{
					Vector3 p(*triangleV[idx], P);

					if (Vector3::Dot(Vector3::Cross(*triangleEdges[idx], p), triangle.normal) < 0)
					{
						return false;
					}
				}

			
				
					if (triangle.cullMode == TriangleCullMode::NoCulling)
					{
						if (ignoreHitRecord == false)
						{
							hitRecord.normal = triangle.normal;
							hitRecord.didHit = true;
							hitRecord.materialIndex = triangle.materialIndex;
							hitRecord.origin = P;
							hitRecord.t = t;
						}
						return true;
						
					}

					else if (triangle.cullMode == TriangleCullMode::BackFaceCulling)
					{
						if (ignoreHitRecord == false)
						{
							if (nv > 0.001)
							{
								return false;
							}
						
							hitRecord.normal = triangle.normal;
							hitRecord.didHit = true;
							hitRecord.materialIndex = triangle.materialIndex;
							hitRecord.origin = P;
							hitRecord.t = t;

							return true;
						}

						if (nv > 0.001)
						{
							return true;
						}

						return false;
					}

					else if (triangle.cullMode == TriangleCullMode::FrontFaceCulling)
					{
						if (ignoreHitRecord == false)
						{
							if (nv < 0.001)
							{
								return false;
							}
						
							hitRecord.normal = triangle.normal;
							hitRecord.didHit = true;
							hitRecord.materialIndex = triangle.materialIndex;
							hitRecord.origin = P;
							hitRecord.t = t;

							return true;
						}

						if (nv < 0.001)
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

		inline bool HitTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			if (!SlabTest_TriangleMesh(mesh,ray)) return false;
			
			HitRecord tempHit = {};

			for (int idx{ 0 }; idx < mesh.indices.size(); idx += 3)
			{
				int i0 = mesh.indices[idx];
				int i1 = mesh.indices[idx + 1];
				int i2 = mesh.indices[idx + 2];

				Triangle triangle{ mesh.transformedPositions[i0],mesh.transformedPositions[i1],mesh.transformedPositions[i2],mesh.transformedNormals[idx / 3] };

				triangle.materialIndex = mesh.materialIndex;
				triangle.cullMode = mesh.cullMode;

				HitTest_Triangle(triangle, ray, tempHit);

				if (tempHit.didHit)
				{
					if (tempHit.t < hitRecord.t)
					{
						hitRecord = tempHit;
					}
				}
			}

			
			return hitRecord.didHit;
		}

		inline bool HitTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_TriangleMesh(mesh, ray, temp, true);
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