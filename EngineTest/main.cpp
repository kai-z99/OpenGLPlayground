#include "KoopaEngine.h"

int main()
{
	KoopaEngine ke = KoopaEngine();	

	/*
	ke.AO(true);
	he.bloom(false);
	he.gamma();
	*/


	//Model m;
	//TODO: Draw calls shouldnt be attached to a shader.
	//TODO: maybe have a shader parameter in SendUniforms()
	//TODO: point shadows
	//TODO: handle if dhadowmap logic if theres no dirLight.
	//TODO: Handle cubemap loading and drawing. Probbaly want it to be perframe draw to be consistant.
	//TODO: make a thing where you can choose to stbi_set_flip_vertically_on_load for each face.

	const char* normalPath = "";
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
	std::vector<const char*> faces =
	{
		"../ShareLib/Resources/interstellar/xneg.png",
		"../ShareLib/Resources/interstellar/xpos.png",
		"../ShareLib/Resources/interstellar/ypos.png",
		"../ShareLib/Resources/interstellar/yneg.png",
		"../ShareLib/Resources/interstellar/zneg.png",
		"../ShareLib/Resources/interstellar/zpos.png"
	};

	//ke.SetSkybox(faces);
	ke.SetDrawLightsDebug(true);

	while (!ke.shouldCloseWindow())
	{
		ke.BeginFrame();

		ke.ClearScreen(Vec4(0.2f, 0.2f, 0.2f, 0.4f));


		ke.DrawPointLight(Vec3(0.0f, 3.0f, 0.0f), Vec3(0.2f, 0.2f, 1.0f), 25.0f);
		ke.DrawPointLight(Vec3(4.0f, 1.0f, cosf(ke.GetCurrentFrame())), Vec3(1.0f, 1.0f, 0.5f), 2.0f);
		ke.DrawDirLight(Vec3(-1.0f, -1.0f, -0.5f), Vec3(1.0f, 1.0f, 1.0f), 0.5, true);

		ke.DrawPlane(
			Vec3(0.0f, -0.5f, 0.0f),		//pos
			Vec2(20.0f, 20.0f),				//size
			Vec4(0.0f, 1.0f, 0.0f, 0.0f)	//rot
		);
		ke.SetCurrentDiffuseTexture(Vec3(1.0f, 0.5f, 0.24f));
		ke.SetCurrentNormalTexture("../ShareLib/Resources/brickwall_normal.jpg");
		
		ke.DrawCube(
			Vec3(2.0f, 2.0f, 0.0f),
			Vec3(1.0f, 2.0f, 1.0f),
			Vec4(0.0f, 1.0f, 1.0f, 150.0f * ke.GetCurrentFrame())
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

		ke.EndFrame();
	}

	return 0;
}