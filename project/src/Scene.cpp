#include "Scene.h"
#include "Utils.h"
#include "Material.h"

namespace dae {

#pragma region Base Scene
	//Initialize Scene with Default Solid Color Material (RED)
	Scene::Scene() :
		m_Materials({ new Material_SolidColor({1,0,0}) })
	{
		m_SphereGeometries.reserve(32);
		m_PlaneGeometries.reserve(32);
		m_TriangleMeshGeometries.reserve(32);
		m_Lights.reserve(32);
	}

	Scene::~Scene()
	{
		for (auto& pMaterial : m_Materials)
		{
			delete pMaterial;
			pMaterial = nullptr;
		}

		m_Materials.clear();
	}

	void dae::Scene::GetClosestHit(const Ray& ray, HitRecord& closestHit) const
	{
		HitRecord tempHit {};

		//Checks through all the sphere
		for (int idx{0};m_SphereGeometries.size() > idx;idx++)
		{

			if (GeometryUtils::HitTest_Sphere(m_SphereGeometries[idx], ray, tempHit))  // Check if there's a hit from spheres
			{
				if (tempHit.t < closestHit.t)  
				{
					closestHit = tempHit;

				}
			}
			
		}

		//Checks through all the planes
		for (int idx{ 0 }; m_PlaneGeometries.size() > idx; idx++)
		{
			if (GeometryUtils::HitTest_Plane(m_PlaneGeometries[idx],ray,tempHit))  // Check if there's a hit from planes
			{
				if (tempHit.t < closestHit.t)
				{
					closestHit = tempHit;
				}
			}
		}

		//Checks through all the Triangles
		for (int idx{ 0 }; m_Triangles.size() > idx; idx++)
		{
			if (GeometryUtils::HitTest_Triangle(m_Triangles[idx], ray, tempHit))  // Check if there's a hit from planes
			{
				if (tempHit.t < closestHit.t)
				{
					closestHit = tempHit;
				}
			}
		}

		
		//Checks through all the Triangles Meshes
		for (int idx{ 0 }; m_TriangleMeshGeometries.size() > idx; idx++)
		{
			if (GeometryUtils::HitTest_TriangleMesh(m_TriangleMeshGeometries[idx], ray, tempHit))  // Check if there's a hit from planes
			{
				if (tempHit.t < closestHit.t)
				{
					closestHit = tempHit;
				}
			}
		}

		tempHit = closestHit;
	}

	bool Scene::DoesHit(const Ray& ray) const
	{
		//Checks through all the sphere
		for (int idx{ 0 }; idx < m_SphereGeometries.size() ; ++idx)
		{
			if (GeometryUtils::HitTest_Sphere(m_SphereGeometries[idx], ray))  // Check if there's a hit from spheres
			{
				return true;
			}
		}


		for (int idx{ 0 }; idx < m_PlaneGeometries.size(); ++idx)
		{
			if (GeometryUtils::HitTest_Plane(m_PlaneGeometries[idx], ray)) // Check if there's a hit from planes
			{
				return true;
			}

		}

		//Checks through all the Triangles
		for (int idx{ 0 }; idx < m_Triangles.size(); ++idx)
		{
			if (GeometryUtils::HitTest_Triangle(m_Triangles[idx], ray))  
			{
				return true;
			}
		}

		//Checks through all the Triangles Meshes
		for (int idx{ 0 };idx < m_TriangleMeshGeometries.size(); idx++)
		{
			if (GeometryUtils::HitTest_TriangleMesh(m_TriangleMeshGeometries[idx], ray))  // Check if there's a hit from planes
			{
				return true;
			}
		}
	
		return false;
	}

#pragma region Scene Helpers
	Sphere* Scene::AddSphere(const Vector3& origin, float radius, unsigned char materialIndex)
	{
		Sphere s;
		s.origin = origin;
		s.radius = radius;
		s.materialIndex = materialIndex;

		m_SphereGeometries.emplace_back(s);
		return &m_SphereGeometries.back();
	}

	Plane* Scene::AddPlane(const Vector3& origin, const Vector3& normal, unsigned char materialIndex)
	{
		Plane p;
		p.origin = origin;
		p.normal = normal;
		p.materialIndex = materialIndex;

		m_PlaneGeometries.emplace_back(p);
		return &m_PlaneGeometries.back();
	}

	TriangleMesh* Scene::AddTriangleMesh(TriangleCullMode cullMode, unsigned char materialIndex)
	{
		TriangleMesh m{};
		m.cullMode = cullMode;
		m.materialIndex = materialIndex;

		m_TriangleMeshGeometries.emplace_back(m);
		return &m_TriangleMeshGeometries.back();
	}

	Light* Scene::AddPointLight(const Vector3& origin, float intensity, const ColorRGB& color)
	{
		Light l;
		l.origin = origin;
		l.intensity = intensity;
		l.color = color;
		l.type = LightType::Point;

		m_Lights.emplace_back(l);
		return &m_Lights.back();
	}

	Light* Scene::AddDirectionalLight(const Vector3& direction, float intensity, const ColorRGB& color)
	{
		Light l;
		l.direction = direction;
		l.intensity = intensity;
		l.color = color;
		l.type = LightType::Directional;

		m_Lights.emplace_back(l);
		return &m_Lights.back();
	}

	unsigned char Scene::AddMaterial(Material* pMaterial)
	{
		m_Materials.push_back(pMaterial);
		return static_cast<unsigned char>(m_Materials.size() - 1);
	}
#pragma endregion
#pragma endregion

#pragma region SCENE W1
	void Scene_W1::Initialize()
	{
		//default: Material id0 >> SolidColor Material (RED)
		constexpr unsigned char matId_Solid_Red = 0;
		const unsigned char matId_Solid_Blue = AddMaterial(new Material_SolidColor{ colors::Blue });

		const unsigned char matId_Solid_Yellow = AddMaterial(new Material_SolidColor{ colors::Yellow });
		const unsigned char matId_Solid_Green = AddMaterial(new Material_SolidColor{ colors::Green });
		const unsigned char matId_Solid_Magenta = AddMaterial(new Material_SolidColor{ colors::Magenta });

		//Spheres
		AddSphere({ -25.f, 0.f, 100.f }, 50.f, matId_Solid_Red);
		AddSphere({ 25.f, 0.f, 100.f }, 50.f, matId_Solid_Blue);

		//Plane
		AddPlane({ -75.f, 0.f, 0.f }, { 1.f, 0.f,0.f }, matId_Solid_Green);
		AddPlane({ 75.f, 0.f, 0.f }, { -1.f, 0.f,0.f }, matId_Solid_Green);
		AddPlane({ 0.f, -75.f, 0.f }, { 0.f, 1.f,0.f }, matId_Solid_Yellow);
		AddPlane({ 0.f, 75.f, 0.f }, { 0.f, -1.f,0.f }, matId_Solid_Yellow);
		AddPlane({ 0.f, 0.f, 125.f }, { 0.f, 0.f,-1.f }, matId_Solid_Magenta);
	}
#pragma endregion


	void Scene_W2::Initialize()
	{
		m_Camera.origin = { 0.f,3.f,-9.f };
		m_Camera.fovAngle = 45.f;

		//default: Material id0 >> SolidColorMaterial (RED)
		constexpr unsigned char matId_Solid_Red = 0;
		const unsigned char matId_Solid_Blue = AddMaterial(new Material_SolidColor{ colors::Blue });

		const unsigned char matId_Solid_Yellow = AddMaterial(new Material_SolidColor{ colors::Yellow });
		const unsigned char matId_Solid_Green = AddMaterial(new Material_SolidColor{ colors::Green });
		const unsigned char matId_Solid_Magenta = AddMaterial(new Material_SolidColor{ colors::Magenta });
		const unsigned char matId_Solid_Porple = AddMaterial(new Material_SolidColor{ ColorRGB(207, 159, 255)});

		//Plane
		AddPlane({ -5.f,0.f,0.f }, { 1.f,0.f,0.f }, matId_Solid_Green);
		AddPlane({ 5.f,0.f,0.f }, { -1.f,0.f,0.f }, matId_Solid_Green);
		AddPlane({ 0.f,0.f,0.f }, { 0.f,1.f,0.f }, matId_Solid_Yellow);
		AddPlane({ 0.f,10.f,0.f }, { 0.f,-1.f,0.f }, matId_Solid_Yellow);
		AddPlane({ 0.f,0.f,10.f }, { 0.f,0.f,-1.f }, matId_Solid_Magenta);

		//Spheres
		AddSphere({-1.75f,1.f,0.f}, .75f, matId_Solid_Red);
		AddSphere({0.f,1.f,0.f}, .75f, matId_Solid_Blue);
		AddSphere({ 1.75f,1.f,0.f }, .75f, matId_Solid_Red);
		AddSphere({ -1.75f,3.f,0.f }, .75f, matId_Solid_Blue);
		AddSphere({ 0.f,3.f,0.f }, .75f, matId_Solid_Red);
		AddSphere({ 1.75f,3.f,0.f }, .75f, matId_Solid_Blue);



		/*AddSphere({ 0.f,5.f,0.f }, .75f, matId_Solid_Porple);*/

		//Light
		AddPointLight({ 0.f,5.f,-5.f }, 70.f, colors::White);

	}


	void Scene_W3::Initialize()
	{

		m_Camera.origin = { 0.f,3.f,-9.f };
		m_Camera.fovAngle = 45.f;

		const auto matCT_GrayRoughMetal		= AddMaterial(new Material_CookTorrence({.972f,.960f,.915f},1.f,1.f));
		const auto matCT_GrayMediumMetal		= AddMaterial(new Material_CookTorrence({ .972f,.960f,.915f }, 1.f, .6f));
		const auto matCT_GraySmoothMetal		= AddMaterial(new Material_CookTorrence({ .972f,.960f,.915f }, 1.f, .1f));
		const auto matCT_GrayRoughPlastic	= AddMaterial(new Material_CookTorrence({ .75f,.75f,.75f },0.f, 1.f));
		const auto matCT_GrayMediumPlastic	= AddMaterial(new Material_CookTorrence({ .75f,.75f,.75f },0.f, .6f));
		const auto matCT_GraySmoothPlastic	= AddMaterial(new Material_CookTorrence({ .75f,.75f,.75f },0.f, .1f));

		const auto matLambert_GrayBlue = AddMaterial(new Material_Lambert({ .49f,.57f,.57f }, 1.f));

		//Plane
		AddPlane(Vector3{0.f,0.f,10.f}, Vector3{0.f,0.f,-1.f},matLambert_GrayBlue);//Back
		AddPlane(Vector3{0.f,0.f,0.f},  Vector3{0.f,1.f,0.f},matLambert_GrayBlue);//Bottom
		AddPlane(Vector3{0.f,10.f,0.f}, Vector3{0.f,-1.f,0.f},matLambert_GrayBlue);//Top
		AddPlane(Vector3{ 5.f,0.f,0.f}, Vector3{-1.f,0.f,0.f},matLambert_GrayBlue);//Right
		AddPlane(Vector3{-5.f,0.f,0.f}, Vector3{ 1.f,0.f,0.f},matLambert_GrayBlue);//Left

		//Temporary Lambert-Phong Spheres & Materials
		//const auto matLambertPhong1 = AddMaterial(new Material_LambertPhong(colors::Blue, 0.5, 0.5, 3.f));
		//const auto matLambertPhong2 = AddMaterial(new Material_LambertPhong(colors::Blue, 0.5, 0.5, 15.f));
		//const auto matLambertPhong3 = AddMaterial(new Material_LambertPhong(colors::Blue, 0.5, 0.5, 50.f));

		//AddSphere({ -1.75f,1.f,0.f }, .75f, matLambertPhong1);
		//AddSphere({ 0.f,1.f,0.f }, .75f,    matLambertPhong2);
		//AddSphere({ 1.75f,1.f,0.f }, .75f,  matLambertPhong3);

		//Spheres
		AddSphere({ -1.75f,1.f,0.f },.75f,  matCT_GrayRoughMetal);
		AddSphere({ 0.f,1.f,0.f },   .75f,	 matCT_GrayMediumMetal);
		AddSphere({ 1.75f,1.f,0.f }, .75f,  matCT_GraySmoothMetal);
		AddSphere({ -1.75f,3.f,0.f },.75f,	 matCT_GrayRoughPlastic);
		AddSphere({ 0.f,3.f,0.f },   .75f,	 matCT_GrayMediumPlastic);
		AddSphere({ 1.75f,3.f,0.f }, .75f,  matCT_GraySmoothPlastic);

		//Light
		AddPointLight(Vector3{ 0.f,5.f,5.f },   50.f, ColorRGB{ 1.f,.61f,.45f }); //Backlight
		AddPointLight(Vector3{-2.5f,5.f,-5.f }, 70.f, ColorRGB{ 1.f,.8f,.45f }); //Front Light Left
		AddPointLight(Vector3{2.5f,2.5f,-5.f }, 50.f, ColorRGB{ .34f,.47f,.68f }); //Front Light Right



		//PHONG TEST SCENE
		//m_Camera.origin = { 0.f,1.f,-5.f };
		//m_Camera.fovAngle = 45.f;

		////Materials
		//const auto matLambert_Red = AddMaterial(new Material_Lambert(colors::Red,1.f));
		//const auto matLambert_Blue = AddMaterial(new Material_LambertPhong(colors::Blue,1.f,1.f,60.f));
		//const auto matLambert_Yellow = AddMaterial(new Material_Lambert(colors::Yellow, 1.f));

		////Spheres
		//AddSphere({ -.75f,1.f,.0f }, 1.f, matLambert_Red);
		//AddSphere({ .75f,1.f,.0f }, 1.f, matLambert_Blue);

		////Plane
		//AddPlane({ 0.f,0.f,0.f }, { 0.f,1.f,0.f }, matLambert_Yellow);

		////Light
		//AddPointLight({ 0.f,5.f,5.f }, 25.f,colors::White);

		//AddPointLight({ 0.f,2.5f,-5.f },25.f,colors::White);
	}


	void Scene_W4_Bunny::Initialize()
	{
		m_Camera.origin = { 0.f, 1.f, -5.f };
		m_Camera.fovAngle = 45.f;

		// Materials
		const auto matLambert_GrayBlue = AddMaterial(new Material_Lambert({ 0.49f, 0.57f, 0.57f }, 1.f));
		const auto matLambert_White = AddMaterial(new Material_Lambert(colors::White, 1.f));

		// Planes
		AddPlane({ 0.f, 0.f, 10.f },{ 0.f, 0.f, -1.f }, matLambert_GrayBlue); // BACK
		AddPlane({ 0.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }, matLambert_GrayBlue);   // BOTTOM
		AddPlane({ 0.f, 10.f, 0.f },{ 0.f, -1.f, 0.f }, matLambert_GrayBlue); // TOP
		AddPlane({ 5.f, 0.f, 0.f }, { -1.f, 0.f, 0.f }, matLambert_GrayBlue);  // RIGHT
		AddPlane({ -5.f, 0.f, 0.f },{ 1.f, 0.f, 0.f }, matLambert_GrayBlue);  // LEFT

		// Triangle (Temp)
		//auto triangle = Triangle{ { -.75f, .5f, 0.f }, { -.75f, 2.f, 0.f }, { .75f, .5f, 0.f } };
		//triangle.cullMode = TriangleCullMode::FrontFaceCulling;
		//triangle.materialIndex = matLambert_White;

		//m_Triangles.emplace_back(triangle);

		//const auto triangleMesh = AddTriangleMesh(TriangleCullMode::NoCulling, matLambert_White);
		//triangleMesh->positions = {	{ -0.75f, -1.f, 0.f}, { -0.75f, 1.f, 0.f},
		//								{ 0.75f, 1.f,  1.f}, { 0.75f, -1.f, 0.f}	};

		//triangleMesh->indices = {
		//	0, 1, 2,   // Triangle 1
		//	0, 2, 3    // Triangle 2
		//};

		//triangleMesh->CalculateNormals();

		//triangleMesh->Translate({ 0.f,1.5f,0.f });
		//triangleMesh->RotateY(45);

		//triangleMesh->UpdateTransforms();


		pMesh = AddTriangleMesh(TriangleCullMode::BackFaceCulling, matLambert_White);
		Utils::ParseOBJ("Resources/lowpoly_bunny.obj", 
						pMesh->positions,
						pMesh->normals,
						pMesh->indices);

		pMesh->RotateY(60);

		pMesh->UpdateAABB();
		pMesh->UpdateTransforms();

		//pMesh->positions = { { -0.75f, -1.f, 0.f}, //v0	
		//						{ -0.75f, 1.f, 0.f}, //v1
		//						{ 0.75f, 1.f,  1.f}, //v2
		//						{ 0.75f, -1.f, 0.f} };//v3

		//pMesh->indices = {
		//	0, 1, 2,   // Triangle 1
		//	0, 2, 3    // Triangle 2
		//};

		//pMesh->CalculateNormals();
		//pMesh->Translate({ 0.f,1.5f,0.f });

		// Light
		AddPointLight({ 0.f, 5.f, 5.f }, 50.f, ColorRGB{ 1.f, .61f, .45f });  // Backlight
		AddPointLight({ -2.5f, 5.f, -5.f }, 70.f, ColorRGB{ 1.f, .8f, .45f }); // Front Light Left
		AddPointLight({ 2.5f, 5.f, -5.f }, 50.f, ColorRGB{ .34f, .47f, .68f }); // Front Light Right
	}

	void Scene_W4_Bunny::Update(dae::Timer* pTimer)
	{
		Scene::Update(pTimer);
	}


	void Scene_W4_ReferenceScene::Initialize()
	{
		sceneName = "Reference Scene";
		m_Camera.origin = { 0.f, 3.f, -9.f };
		m_Camera.fovAngle =  45.f;

		// Materials
		const auto matCT_GrayRoughMetal = AddMaterial(new Material_CookTorrence({ 0.972f, 0.960f, 0.915f }, 1.f, 1.0f));
		const auto matCT_GrayMediumMetal = AddMaterial(new Material_CookTorrence({ 0.972f, 0.960f, 0.915f }, 1.f, 0.6f));
		const auto matCT_GraySmoothMetal = AddMaterial(new Material_CookTorrence({ 0.972f, 0.960f, 0.915f }, 1.f, 0.1f));
		const auto matCT_GrayRoughPlastic = AddMaterial(new Material_CookTorrence({ 0.75f , 0.75f , 0.75f }, 0.f, 1.0f));
		const auto matCT_GrayMediumPlastic = AddMaterial(new Material_CookTorrence({ 0.75f , 0.75f , 0.75f }, 0.f, 0.6f));
		const auto matCT_GraySmoothPlastic = AddMaterial(new Material_CookTorrence({ 0.75f , 0.75f , 0.75f }, 0.f, 0.1f));

		const auto matLambert_GrayBlue = AddMaterial(new Material_Lambert({ 0.49f, 0.57f, 0.57f }, 1.f));
		const auto matLambert_White = AddMaterial(new Material_Lambert(colors::White, 1.f));

		// Plane
		AddPlane(Vector3{ 0.f,  0.f, 10.f }, Vector3{ 0.f,  0.f, -1.f }, matLambert_GrayBlue);    // BACK
		AddPlane(Vector3{ 0.f,  0.f,  0.f }, Vector3{ 0.f,  1.f,  0.f }, matLambert_GrayBlue);    // BOTTOM
		AddPlane(Vector3{ 0.f, 10.f,  0.f }, Vector3{ 0.f, -1.f,  0.f }, matLambert_GrayBlue);    // TOP
		AddPlane(Vector3{ 5.f,  0.f,  0.f }, Vector3{ -1.f,  0.f,  0.f }, matLambert_GrayBlue);    // RIGHT
		AddPlane(Vector3{ -5.f,  0.f,  0.f }, Vector3{ 1.f,  0.f,  0.f }, matLambert_GrayBlue);    // LEFT

		// Spheres
		AddSphere({ -1.75f, 1.f, 0.f, }, 0.75f, matCT_GrayRoughMetal);
		AddSphere({ 0.f  , 1.f, 0.f, }, 0.75f, matCT_GrayMediumMetal);
		AddSphere({ 1.75f, 1.f, 0.f, }, 0.75f, matCT_GraySmoothMetal);
		AddSphere({ -1.75f, 3.f, 0.f, }, 0.75f, matCT_GrayRoughPlastic);
		AddSphere({ 0.f  , 3.f, 0.f, }, 0.75f, matCT_GrayMediumPlastic);
		AddSphere({ 1.75f, 3.f, 0.f, }, 0.75f, matCT_GraySmoothPlastic);

		// Triangle
		const Triangle baseTriangle = { Vector3(-0.75f, 1.5f, 0.f)      //v0
									  , Vector3(0.75f, 0.0f, 0.f)      //v1
									  , Vector3(-0.75f, 0.0f, 0.f) };    //v2

		// Triangle Meshes
		m_Meshes[0] = AddTriangleMesh(TriangleCullMode::BackFaceCulling, matLambert_White);
		m_Meshes[0]->AppendTriangle(baseTriangle, true);
		m_Meshes[0]->Translate({ -1.75f, 4.5f, 0.f });
		m_Meshes[0]->UpdateAABB();
		m_Meshes[0]->UpdateTransforms();

		m_Meshes[1] = AddTriangleMesh(TriangleCullMode::FrontFaceCulling, matLambert_White);
		m_Meshes[1]->AppendTriangle(baseTriangle, true);
		m_Meshes[1]->Translate({ 0.f, 4.5f, 0.f });
		m_Meshes[1]->UpdateAABB();
		m_Meshes[1]->UpdateTransforms();

		m_Meshes[2] = AddTriangleMesh(TriangleCullMode::NoCulling, matLambert_White);
		m_Meshes[2]->AppendTriangle(baseTriangle, true);
		m_Meshes[2]->Translate({ 1.75f, 4.5f, 0.f });
		m_Meshes[2]->UpdateAABB();
		m_Meshes[2]->UpdateTransforms();



		// Light
		AddPointLight({ 0.f  , 5.f ,  5.f }, 50.f, ColorRGB{ 1.f  , 0.61f, 0.45f });   // BackLight
		AddPointLight({ -2.5f , 5.f , -5.f }, 70.f, ColorRGB{ 1.f  , 0.8f , 0.45f });   // Front Light Left
		AddPointLight({ 2.5f , 2.5f, -5.f }, 50.f, ColorRGB{ 0.34f, 0.47f, 0.68f });
	}

	void Scene_W4_ReferenceScene::Update(dae::Timer* pTimer)
	{
		Scene::Update(pTimer);

		const auto yawAngle = (cos(pTimer->GetTotal()) + 1.f) / 2.f * PI_2;
		for (const auto m : m_Meshes)
		{
			m->RotateY(yawAngle);
			m->UpdateTransforms();
		}
	}
}
