#include "KoopaEngine.h"
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

int main()
{
	KoopaEngine ke = KoopaEngine();

	/*
	ke.AO(true);
	he.bloom(false);
	*/

	//Model m;
	//TODO: Handle cubemap loading and drawing. Probbaly want it to be perframe draw to be consistant.
	//TODO: make a thing where you can choose to stbi_set_flip_vertically_on_load for each face.
	// 
	//TODO: make it so expanding a the scale changes so that it repeats the texture instead of stretches it.
	// 
	//TODO: enable/disable bloom
	//TODO: Add a roughness param for each drawcall
	//TODO: work on camera stuff
	//TODO: Terrain
	//TODO: Fix bug where dir light doesnt work with dor 0,-1,0
	
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
	while (!ke.shouldCloseWindow())
	{
		ke.BeginFrame();

		float t = ke.GetCurrentFrame();

		ke.ClearScreen(Vec4(0.2f, 0.2f, 0.2f, 0.4f));

		ke.DrawPointLight(
			Vec3(0.0f, 3.0f, -1.0f),			//pos
			Vec3(1.0f, 1.0f, 1.0f),				//col
			8.0f,						//intensity
			true						//cast shadows
		);
		ke.DrawPointLight(Vec3(4.0f, 1.0f, cosf(t)), Vec3(0.5f, 0.2f, 1.0f), 15.0f, false);
		ke.DrawPointLight(Vec3(1.0f, 1 + sinf(t), 1.0f), Vec3(0.1f, 0.8f, 1.0f), 2.0f, false);
		ke.DrawPointLight(Vec3(-3.0f, 1 + cosf(t), 1.0f), Vec3(0.1f, 1.8f, 0.5f), 2.0f, false);
		ke.DrawDirLight(Vec3(-1.0f, -1.0f, -1.0f), Vec3(1.0f, 1.0f, 1.0f), 0.8f, true);

		ke.DrawPlane(
			Vec3(0, -0.5f, 0),				//pos
			Vec2(20.0f, 20.0f),				//size
			Vec4(0.0f, 1.0f, 0.0f, 0.0f)			//rot
		);
		ke.SetCurrentDiffuseTexture("../ShareLib/Resources/marble_tile.jpg");
		ke.SetCurrentNormalTexture("../ShareLib/Resources/marble_tile_normal.jpg");

		ke.DrawPlane(
			Vec3(0, 9.5f, -10.0f),				//pos
			Vec2(20.0f, 20.0f),				//size
			Vec4(1.0f, 0.0f, 0.0f, 90.0f)			//rot
		);
		ke.SetCurrentDiffuseTexture(Vec3(1.0f, 0.5f, 0.24f));
		ke.SetCurrentNormalTexture("../ShareLib/Resources/brickwall_normal.jpg");

		ke.DrawCube(
			Vec3(2.0f, 2.0f, 0.0f),
			Vec3(1.0f, 2.0f, 1.0f),
			Vec4(0.0f, 1.0f, 1.0f, 150.0f * t)
		);
		ke.SetCurrentDiffuseTexture("../ShareLib/Resources/brickwall.jpg");
		ke.SetCurrentNormalTexture("../ShareLib/Resources/brickwall_normal.jpg");

		ke.DrawCube(
			Vec3(0.0f, 0.0f, 0.0f),
			Vec3(1.0f, 1.0f, 1.0f),
			Vec4(0.0f, 1.0f, 0.0f, 20.0f)
		);
		ke.SetCurrentDiffuseTexture("../ShareLib/Resources/brickwall.jpg");
		ke.SetCurrentNormalTexture("../ShareLib/Resources/brickwall_normal.jpg");

		ke.DrawCube(
			Vec3(0.0f, 0.0f, 3.0f),
			Vec3(2.0f, 1.0f, 1.0f),
			Vec4(0.0f, 1.0f, 0.0f, 40.0f)
		);
		ke.SetCurrentDiffuseTexture("../ShareLib/Resources/fabric.jpg");
		ke.SetCurrentNormalTexture("../ShareLib/Resources/fabric_normal.jpg");

		ke.DrawSphere(
			Vec3(0, 2, -3),
			Vec3(0.5, 1, 1),
			Vec4(0, 1, 0, 30 * t)
		);
		ke.SetCurrentDiffuseTexture("../ShareLib/Resources/rock.png");
		ke.SetCurrentNormalTexture("../ShareLib/Resources/rock_normal.png");

		ke.EndFrame();
	}

	return 0;
}