#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D brightScene;

uniform bool horizontal;

float weight[5] = float[] (0.227027f, 0.1945946f, 0.1216216f, 0.054054f, 0.016216f); //gaussian

void main()
{
	vec2 texOffset = 1.0f / textureSize(brightScene, 0); //size of one texel
	vec3 result = vec3(0.0f); //Final fragement color

	//1.)  current fragments contribution
	result += texture(brightScene, TexCoord).rgb * weight[0];

	//2.) h/v texel contributions
	if (horizontal)
	{
		for (int i = 1; i < 5; i++)
		{
			vec2 currentSampleOffset = vec2(texOffset.x * i, 0.0f);
			result += texture(brightScene, TexCoord + currentSampleOffset).rgb * weight[i];
			result += texture(brightScene, TexCoord - currentSampleOffset).rgb * weight[i];
		}
	}
	else
	{
		for (int i = 1; i < 5; i++)
		{
			vec2 currentSampleOffset = vec2(0.0f, texOffset.y * i);
			result += texture(brightScene, TexCoord + currentSampleOffset).rgb * weight[i];
			result += texture(brightScene, TexCoord - currentSampleOffset).rgb * weight[i];
		}
	}

	FragColor = vec4(result, 1.0f);
}

