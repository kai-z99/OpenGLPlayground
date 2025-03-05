#include "KoopaEngine.h"

int main()
{
	KoopaEngine ke = KoopaEngine();	

	/*
	ke.AO(true);
	he.bloom(false);
	he.gamma();
	*/

	//Texture* t = newTexture(path);
	//Model m;
	//TODO: Draw calls shouldnt be attached to a shader.

	while (!ke.shouldCloseWindow())
	{
		ke.BeginFrame();

		ke.ClearScreen(Vec4(0.3f, 0.3f, 0.3f, 0.3f));

		//TODO: Change diffuse texture here
		//ke.setTexture(const char* path); //so this works
		ke.DrawPointLight(Vec3(0.0f, 2.0f, 0.0f), Vec3(1.0f, 1.0f, 1.0f), 5.0f);
		ke.DrawPointLight(Vec3(3.0f, -3.0f, cosf(ke.GetCurrentFrame())), Vec3(1.0f, 1.0f, 0.5f), 2.0f);
		//ke.DrawPointLight(Vec3(3.0f, 1.0f, 0.0f), Vec3(1.0f, 1.0f, 0.5f), 2.0f);
		//ke.DrawPointLight(Vec3(3.0f, 1.0f, 0.0f), Vec3(1.0f, 1.0f, 0.5f), 2.0f);
		//ke.DrawPointLight(Vec3(3.0f, 1.0f, 0.0f), Vec3(1.0f, 1.0f, 0.5f), 2.0f);

		ke.DrawLightsDebug();

		ke.SetCurrentDiffuseTexture("../ShareLib/Resources/wood.png");

		//Make it so size is vec2 so you dont collapse
		//also make iut so its actually 1x1
		ke.DrawPlane(
			Vec3(0.0f, -4.0f, 0.0f),		//pos
			Vec3(1.0f, 1.0f, 3.0f),			//size
			Vec4(0.0f, 1.0f, 0.0f, 0.0f)	//rot
		);
		
		//m.draw();

		/*
		ke.PointLIight(pos, bright, col, shadow=false)
		ke.DirLight(dir, shadow=true)
		*/

		ke.DrawTriangle(
			Vec3(0.0f, 0.0f, 0.0f),
			Vec4(0.0f, 1.0f, 0.0f, 0.0f)
		);
		
		ke.SetCurrentDiffuseTexture("../ShareLib/Resources/brickwall.jpg");
		
		ke.DrawCube(
			Vec3(2.0f, 0.0f, 0.0f),
			Vec3(1.0f, 1.0f, 1.0f),
			Vec4(0.0f, 1.0f, 1.0f, 150.0f * ke.GetCurrentFrame())
		);

		ke.EndFrame();

	}

	return 0;
}