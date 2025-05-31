#include "KoopaEngine.h"

//TODO: When you go close to some lights at the right angle, the whole light doesnt produce any light
//TODO: Fix light radius problem, gpt possibly?
//TODO: Fix tiling artifact problem (mystery right now)
//TODO: Remove old forward lighting stuff...!!!...!!!!!!!!!!!!!!!!!!!!!!...!!!!

#include <random>

int main()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> disRange(1.0f, 80.0f);
	std::uniform_real_distribution<float> disCol(0.0f, 1.0f);
	std::uniform_real_distribution<float> disInt(0.1f, 4.0f);

	struct Light
	{
		Vec3 color;
		float intensity;
		float range;
	};

	std::vector<Light> lights;
	for (int i = 0; i < 400 * 400; i++)
	{
		Light l = Light();

		l.color = Vec3(disCol(gen), disCol(gen), disCol(gen));
		l.intensity = disInt(gen);
		l.range = disRange(gen);

		lights.push_back(l);
	}
	
	

	KoopaEngine ke = KoopaEngine();

	std::vector<const char*> faces =
	{
		"../ShareLib/Resources/interstellar/xneg.png",
		"../ShareLib/Resources/interstellar/xpos.png",
		"../ShareLib/Resources/interstellar/ypos.png",
		"../ShareLib/Resources/interstellar/yneg.png",
		"../ShareLib/Resources/interstellar/zneg.png",
		"../ShareLib/Resources/interstellar/zpos.png"
	};

	ke.SetSkybox(faces);
	ke.SetDrawLightsDebug(true);
	ke.SetCameraExposure(0.7f);
	ke.SetCameraSpeed(30.0f);
	ke.SetFogColor(Vec3(0.9f, 0.5f, 0.2f));
	ke.SetExpFogDensity(0.22f);
	ke.SetFogType(EXPONENTIAL_SQUARED);
	ke.SetAmbientLighting(0.05f);
	ke.SetLinearFogStart(70.4f);
	ke.SetBloomThreshold(3.0f);

	//
	//ke.CreateParticleEmitter(30, 1024 * 128, Vec3(-35.0f, 0.0f, 0.0f), Vec3(1.0f, 2.0f, 1.0f), Vec4(0.0f, 0.0f, 1.0f, 30.0f));
	//ke.CreateParticleEmitter(30, 1024 * 1024, Vec3(0.0f, 0.0f, 0.0f), Vec3(1.0f, 1.0f, 1.0f), Vec4(0.0f, 0.0f, 1.0f, 0.0f));
	//ke.CreateParticleEmitter(30, 1024 * 128, Vec3(35.0f, 0.0f, 0.0f), Vec3(1.0f, 2.0f, 1.0f), Vec4(0.0f, 0.0f, 1.0f, -30.0f));

	while (!ke.shouldCloseWindow())
	{
		ke.BeginFrame();
		float t = ke.GetCurrentFrame();	

		ke.ClearScreen(Vec4(0.0f, 0.0f, 0.0f, 0.4f));
		
		//PARTICLE
		//{
		//	ke.SetCurrentDiffuseTexture(Vec3(0.3f, 0.3f, 0.3f));
		//	ke.SetCurrentBaseSpecular(0.1f);
		//	//ke.SetCurrentNormalTexture("../ShareLib/Resources/marble_tile_normal.jpg");
		//	ke.DrawPlane(
		//		Vec3(0, -50.5f, 0),				//pos
		//		Vec2(200.0f, 200.0f),			//size
		//		Vec4(0.0f, 1.0f, 0.0f, 0.0f)	//rot
		//	);
		//	ke.ResetMaterial();

		//	ke.SetCurrentDiffuseTexture(Vec3(0.1f, 0.1f, 0.1f));
		//	ke.SetCurrentBaseSpecular(0.1f);
		//	//ke.SetCurrentNormalTexture("../ShareLib/Resources/brickwall_normal.jpg");
		//	ke.DrawPlane(
		//		Vec3(0, 7.5f, -10.0f),			//pos //7.5 not 9.5
		//		Vec2(200.0f, 200.0f),				//size
		//		Vec4(1.0f, 0.0f, 0.0f, 90.0f)	//rot
		//	);
		//	ke.ResetMaterial();

		//	ke.DrawDirLight(Vec3(-1, -1.0f, -1), Vec3(1.0f, 1.0f, 1.0f), 2.0f, 1);
		//}
		
		//FWD+
		//static int index = 0;
		//{
		//	

		//	for (int i = 0; i < 300; i += 10)
		//	{
		//		for (int j = 0; j < 300; j += 10)
		//		{
		//			ke.DrawPointLight(Vec3(i + 5 * cos(t) - 100.f, 1.0f, j + 5 * sin(t) - 100.f), lights[index].color, lights[index].range, lights[index].intensity, false); //1.8f
		//			index++;
		//		}

		//	}

		//	index = 0;

		//	//LOWER PLANE
		//	//ke.SetCurrentDiffuseTexture("../ShareLib/Resources/marble_tile.jpg");
		//	ke.SetCurrentDiffuseTexture(Vec3(1.0f, 1.0f, 1.0f));
		//	ke.SetCurrentBaseSpecular(0.1f);
		//	//ke.SetCurrentNormalTexture("../ShareLib/Resources/marble_tile_normal.jpg");
		//	ke.DrawPlane(
		//		Vec3(0, -0.5f, 0),				//pos
		//		Vec2(2000.0f, 2000.0f),			//size
		//		Vec4(0.0f, 1.0f, 0.0f, 0.0f)	//rot
		//	);
		//	ke.ResetMaterial();
		//}

		//GENERIC
		{
			ke.DrawPointLight(
				Vec3(0.0f, 5 + 5 * sin(t), -1.0f),	//position
				Vec3(1.0f, 1.0f, 1.0f),												//col
				30.0f,																//range
				3.5f,																//intensity
				1																	//cast shadows
			);

			ke.DrawPointLight(Vec3(1.0f, 1 + sinf(t), -5.0f), Vec3(0.1f, 0.8f, 1.0f), 50.0f, 1.0f, 1);
			ke.DrawPointLight(Vec3(-4.0f, 1 + cosf(t), 1.0f), Vec3(0.1f, 1.8f, 0.5f), 40.0f, 1.0f, 0);



			//TERRAIN
			ke.SetCurrentDiffuseTexture("../ShareLib/Resources/rock.png");
			ke.SetCurrentBaseSpecular(0.0f);
			ke.DrawTerrain("../ShareLib/Resources/iceland_heightmap.png", Vec3(0, -20, 0), Vec3(0.3, 0.3, 0.3));
			ke.ResetMaterial();

			//LOWER PLANE
			ke.SetCurrentDiffuseTexture("../ShareLib/Resources/marble_tile.jpg");
			ke.SetCurrentDiffuseTexture(Vec3(1.0f, 1.0f, 1.0f));
			ke.SetCurrentBaseSpecular(0.1f);
			ke.SetCurrentNormalTexture("../ShareLib/Resources/marble_tile_normal.jpg");
			ke.DrawPlane(
				Vec3(0, -0.5f, 0),				//pos
				Vec2(20.0f, 20.0f),			//size
				Vec4(0.0f, 1.0f, 0.0f, 0.0f)	//rot
			);
			ke.ResetMaterial();


			//UPPER PLANE
			ke.SetCurrentDiffuseTexture(Vec3(1.0f, 1.0f, 1.0f));
			ke.SetCurrentBaseSpecular(0.1f);
			ke.SetCurrentNormalTexture("../ShareLib/Resources/brickwall_normal.jpg");
			ke.DrawPlane(
				Vec3(0, 7.5f, -10.0f),			//pos //7.5 not 9.5
				Vec2(20.0f, 20.0f),				//size
				Vec4(1.0f, 0.0f, 0.0f, 90.0f)	//rot
			);
			ke.ResetMaterial();

			//CUBE 1
			ke.SetCurrentDiffuseTexture("../ShareLib/Resources/marble_tile.jpg");
			ke.SetCurrentNormalTexture("../ShareLib/Resources/brickwall_normal.jpg");
			//ke.SetCurrentSpecularTexture("../ShareLib/Resources/brickwall_specular.jpg");
			ke.DrawCube(
				Vec3(5.0f, 2.0f, 0.0f),
				Vec3(1.0f, 2.0f, 1.0f),
				Vec4(0.0f, 1.0f, 1.0f, 150.0f * t)
			);

			//CUBE 2 (using prev material)
			ke.DrawCube(
				Vec3(0.0f, 0.0f, -4.0f),
				Vec3(0.3f, 6.0f, 0.3f),
				Vec4(0.0f, 1.0f, 0.0f, 20.0f)
			);
			ke.ResetMaterial();

			//SPHERE

			ke.SetCurrentDiffuseTexture("../ShareLib/Resources/rock.png");
			ke.SetCurrentNormalTexture("../ShareLib/Resources/rock_normal.png");
			ke.DrawSphere(
				Vec3(0, 2, -4),
				Vec3(1, 1, 1),
				Vec4(0, 1, 0, 30 * t)
			);
			ke.ResetMaterial();


			ke.DrawModel("../ShareLib/Resources/woodDragon/dragon.obj", true, Vec3(0.0f, 0.45f, 0.0f),
				{ 1.0f, 1.0f, 1.0f }, Vec4(0, 1, 0, 10 * t));


			ke.DrawModel("../ShareLib/Resources/animeChar/animeChar.obj", true, Vec3(-2.5f, -0.5f, 0.0f),
				{ 2.0f, 2.0f, 2.0f }, Vec4(0, 1, 0, 10 * t));


			/*
			ke.DrawModel("../ShareLib/Resources/vokselia_spawn/vokselia_spawn.obj", false, Vec3(0, 1.4, 0),
				{ 100.014, 100.014,100.014});
			*/
		}

		//PBR
		//{
		//	ke.SetCurrentPBRMaterial(
		//		"../ShareLib/Resources/mortar_stone_wall/albedo.png",
		//		"../ShareLib/Resources/mortar_stone_wall/normal.png",
		//		"../ShareLib/Resources/mortar_stone_wall/height.png",
		//		"../ShareLib/Resources/mortar_stone_wall/metallic.png",
		//		"../ShareLib/Resources/mortar_stone_wall/roughness.png",
		//		"../ShareLib/Resources/mortar_stone_wall/ao.png"
		//	);

		//	ke.DrawSphere(
		//		Vec3(4, 2, -4),
		//		Vec3(1, 1, 1),
		//		Vec4(0, 1, 0, 30 * t)
		//	);

		//	ke.SetCurrentPBRMaterial(
		//		"../ShareLib/Resources/alien_metal/albedo.png",
		//		"../ShareLib/Resources/alien_metal/normal.png",
		//		"../ShareLib/Resources/alien_metal/height.png",
		//		"../ShareLib/Resources/alien_metal/metallic.png",
		//		"../ShareLib/Resources/alien_metal/roughness.png",
		//		"../ShareLib/Resources/alien_metal/ao.png"
		//	);



		//	ke.SetCurrentPBRMaterial(
		//		"../ShareLib/Resources/pine_bark/albedo.png",
		//		"../ShareLib/Resources/pine_bark/normal.png",
		//		"../ShareLib/Resources/pine_bark/height.png",
		//		"../ShareLib/Resources/pine_bark/metallic.png",
		//		"../ShareLib/Resources/pine_bark/roughness.png",
		//		"../ShareLib/Resources/pine_bark/ao.png"
		//	);

		//	ke.DrawSphere(
		//		Vec3(12, 2, -4),
		//		Vec3(1, 1, 1),
		//		Vec4(0, 1, 0, 30 * t)
		//	);

		//	ke.SetCurrentPBRMaterial(
		//		"../ShareLib/Resources/red_rock/albedo.png",
		//		"../ShareLib/Resources/red_rock/normal.png",
		//		"../ShareLib/Resources/red_rock/height.png",
		//		"../ShareLib/Resources/red_rock/metallic.png",
		//		"../ShareLib/Resources/red_rock/roughness.png",
		//		"../ShareLib/Resources/red_rock/ao.png"
		//	);

		//	ke.DrawSphere(
		//		Vec3(14, 4, -4),
		//		Vec3(1, 1, 1),
		//		Vec4(0, 1, 0, 30 * t)
		//	);

		//	ke.SetCurrentPBRMaterial(
		//		"../ShareLib/Resources/fancy_gold/albedo.png",
		//		"../ShareLib/Resources/fancy_gold/normal.png",
		//		"../ShareLib/Resources/fancy_gold/height.png",
		//		"../ShareLib/Resources/fancy_gold/metallic.png",
		//		"../ShareLib/Resources/fancy_gold/roughness.png",
		//		"../ShareLib/Resources/fancy_gold/ao.png"
		//	);

		//	ke.DrawSphere(
		//		Vec3(18, 2, -4),
		//		Vec3(1, 1, 1),
		//		Vec4(0, 1, 0, 30 * t)
		//	);

		//	ke.SetCurrentPBRMaterial(
		//		"../ShareLib/Resources/white_marble/albedo.png",
		//		"../ShareLib/Resources/white_marble/normal.png",
		//		"../ShareLib/Resources/white_marble/height.png",
		//		"../ShareLib/Resources/white_marble/metallic.png",
		//		"../ShareLib/Resources/white_marble/roughness.png",
		//		"../ShareLib/Resources/white_marble/ao.png"
		//	);

		//	ke.DrawSphere(
		//		Vec3(20, 2, -4),
		//		Vec3(1, 1, 1),
		//		Vec4(0, 1, 0, 30 * t)
		//	);
		//}


		ke.EndFrame();
	}

	return 0;
}

/*
for (float x = -10.0f; x < 10.0f; x += 5.0f)
{
	for (float z = -10.0f; z < 10.0f; z += 5.0f)
	{
		ke.DrawModel("../ShareLib/Resources/knight/knight.obj", true, Vec3(x, -0.5f, z),
			{ 2.0f, 2.0f, 2.0f }, Vec4(0, 1, 0, 10 * t));
	}
}
*/
/*
ke.DrawModel("../ShareLib/Resources/knight/knight.obj", true, Vec3(-1.5f, -0.5f, 3.0f),
	{ 2.0f, 2.0f, 2.0f }, Vec4(0, 1, 0, 10 * t));
*/

/*
ke.DrawModel("../ShareLib/Resources/vokselia_spawn/vokselia_spawn.obj", false, Vec3(0, 1.4, 0),
	{ 100.014, 100.014,100.014});
*/

/*
ke.DrawModel("../ShareLib/Resources/dragon.obj", false, Vec3(0, 1.5, 0),
	{ 5.014, 5.014,5.014 }, {0, 1, 0, 10 * t});
*/

/*
		ke.DrawModel("../ShareLib/Resources/backpack/backpack.obj", true, Vec3(8, 3, 1),
			Vec3(1,1,1), Vec4(0, 1, 0, 90.0f));
		//ke.DrawModel("../ShareLib/Resources/Toad/Toad.obj", false, Vec3(-6, -0.5, 1),
		//	{0.02, 0.02, 0.02}, Vec4(0, 1, 0, 10 * t));

		ke.DrawModel("../ShareLib/Resources/hammer/model.dae", false, Vec3(4, 3.5 + 0.3 * cosf(t), -3),
			{ 1.02, 1.02, 1.02 }, Vec4(1, 0, 0, 90));

		/*ke.DrawModel("../ShareLib/Resources/minion2/minion2.obj", false, Vec3(0.5, (t < 5)? -0.5 : -0.5 + (t - 5), 1),
			{0.08, 0.08, 0.08 }, Vec4(0, 1, 0, 300 * t));*/




			//Invert both colors 10 level

		//CUBE 3

		//ke.SetCurrentDiffuseTexture("../ShareLib/Resources/fabric.jpg");
		//ke.SetCurrentNormalTexture("../ShareLib/Resources/fabric_normal.jpg");
		//ke.DrawCube(
		//	Vec3(0.0f, 40.0f, 3.0f),
		//	Vec3(10.0f, 10.0f, 10.0f),
		//	Vec4(0.0f, 1.0f, 0.0f, 100 * t)
		//);

		/*
		//ke.SetCurrentDiffuseTexture("../ShareLib/Resources/brickwall.jpg");
		//ke.SetCurrentNormalTexture("../ShareLib/Resources/brickwall_normal.jpg");
		//for (const auto& b : buildings)
		//	ke.DrawCube(b.centre, b.scale, Vec4(0.f, 1.f, 0.f, 0.f));
		//ke.ResetMaterial();

		*/

		//TODO: Handle cubemap loading and drawing. Probbaly want it to be perframe draw to be consistant.
		//TODO: make a thing where you can choose to stbi_set_flip_vertically_on_load for each face.
		// 
		//TODO: work on camera stuff
		//TODO: Add a smooth curve to bloom calcs
		//TODO: Area Lights
		//TODO: PCSS
		//TODO: Auto LODmodels especially for shadowmaps.
		//TODO: SSS
		//TODO: VSM
		//TODO: toggle msaa

//const auto buildings = MakeBuildings(10);

/*
std::vector<const char*> faces =
{
	"../ShareLib/Resources/skybox/right.jpg",
	"../ShareLib/Resources/skybox/left.jpg",
	"../ShareLib/Resources/skybox/top.jpg",
	"../ShareLib/Resources/skybox/bottom.jpg",
	"../ShareLib/Resources/skybox/front.jpg",
	"../ShareLib/Resources/skybox/back.jpg"
};
*/
/*
std::vector<const char*> faces =
{
	"../ShareLib/Resources/spacebox/skybox_left.png",
	"../ShareLib/Resources/spacebox/skybox_right.png",
	"../ShareLib/Resources/spacebox/skybox_up.png",
	"../ShareLib/Resources/spacebox/skybox_down.png",
	"../ShareLib/Resources/spacebox/skybox_front.png",
	"../ShareLib/Resources/spacebox/skybox_back.png"
};
*/
/*
ke.SetCurrentPBRMaterial(
	"../ShareLib/Resources/scuffed_metal/albedo.png",
	"../ShareLib/Resources/scuffed_metal/normal.png",
	"../ShareLib/Resources/scuffed_metal/metallic.png",
	"../ShareLib/Resources/scuffed_metal/roughness.png",
	"../ShareLib/Resources/scuffed_metal/ao.png"
);

ke.DrawSphere(
	Vec3(0, 2, -4),
	Vec3(1, 1, 1),
	Vec4(0, 1, 0, 30 * t)
);

ke.SetCurrentPBRMaterial(
	"../ShareLib/Resources/rusted_iron/albedo.png",
	"../ShareLib/Resources/rusted_iron/normal.png",
	"../ShareLib/Resources/rusted_iron/metallic.png",
	"../ShareLib/Resources/rusted_iron/roughness.png",
	"../ShareLib/Resources/rusted_iron/ao.png"
);

ke.DrawSphere(
	Vec3(2, 2, -4),
	Vec3(1, 1, 1),
	Vec4(0, 1, 0, 30 * t)
);
*/

/*
		ke.DrawSphere(
			Vec3(6, 2, -4),
			Vec3(1, 1, 1),
			Vec4(0, 1, 0, 30 * t )
		);
		/*
		ke.SetCurrentPBRMaterial(
			"../ShareLib/Resources/vines/albedo.png",
			"../ShareLib/Resources/vines/normal.png",
			"../ShareLib/Resources/vines/height.png",
			"../ShareLib/Resources/vines/metallic.png",
			"../ShareLib/Resources/vines/roughness.png",
			"../ShareLib/Resources/vines/ao.png"
		);

		ke.DrawSphere(
			Vec3(8, 2, -4),
			Vec3(1, 1, 1),
			Vec4(0, 1, 0, 30 * t)
		);
		*/
		/*
		ke.SetCurrentPBRMaterial(
			"../ShareLib/Resources/quilted/albedo.png",
			"../ShareLib/Resources/quilted/normal.png",
			"../ShareLib/Resources/quilted/metallic.png",
			"../ShareLib/Resources/quilted/roughness.png",
			"../ShareLib/Resources/quilted/ao.png"
		);

		ke.DrawSphere(
			Vec3(16, 2, -4),
			Vec3(1, 1, 1),
			Vec4(0, 1, 0, 30 * t)
		);
		*/