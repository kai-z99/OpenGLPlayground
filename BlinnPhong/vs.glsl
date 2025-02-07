#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoord;
out vec3 ourNormal;
out vec3 FragPos;
out vec4 LightSpaceFragPos;

//shadows
uniform mat4 lightSpaceMatrix;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);  //frag position in screen space
    ourNormal = mat3(transpose(inverse(model))) * aNormal;     //Normal, taking into account non-linear scaling
    TexCoord = aTexCoord;                                      // texture coords
    FragPos = vec3(model * vec4(aPos, 1.0));                   //fragment position in world space
    LightSpaceFragPos = lightSpaceMatrix * model * vec4(aPos, 1.0); //fragment postiion in light space
}