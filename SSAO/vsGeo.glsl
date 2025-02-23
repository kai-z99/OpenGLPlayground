#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	//Frag pos in view space
	vec4 viewSpaceFragPos = view * model * vec4(aPos, 1.0f);
	FragPos = viewSpaceFragPos.xyz; //view space

	//View space normal matrix
	mat3 normalMatrix = transpose(inverse(mat3(view * model)));
	Normal = normalMatrix * aNormal; //view space

	gl_Position = projection * viewSpaceFragPos; //clip space
}
