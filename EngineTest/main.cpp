#include "KoopaEngine.h"

int main()
{
	KoopaEngine ke = KoopaEngine();	

	while (!ke.shouldCloseWindow())
	{
		ke.BeginFrame();
		//TODO: Change diffuse texture here
		//ke.setTexture(const char* path); //so this works

		ke.DrawPlane(
			Vec3(0.0f, -4.0f, 0.0f), //pos
			Vec3(5.0f, 0.0f, 3.0f), //size
			Vec4(0.0f, 1.0f, 0.0f, 0.0f) //rot
		);

		ke.DrawTriangle(Vec3(0.0f, 0.0f, 0.0f),
						Vec4(0.0f, 1.0f, 0.0f, 0.0f)
		);

		ke.DrawCube(Vec3(2.0f, 0.0f, 0.0f),
					Vec3(1.0f, 1.0f, 1.0f),
					Vec4(0.0f, 1.0f, 0.0f, 10.0f * ke.GetCurrentFrame())
		);
			
		ke.EndFrame();

	}

	return 0;
}