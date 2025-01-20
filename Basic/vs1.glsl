#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aNormal;

out vec3 ourColor;
out vec2 TexCoord;
out vec3 ourNormal;
out vec3 FragPos;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0); // vertex position
    ourColor = aColor;                                         // color
    TexCoord = aTexCoord;                                      // texture coords
    ourNormal = mat3(transpose(inverse(model))) * aNormal;     //Normal, taking into account non-linear scaling

    FragPos = vec3(model * vec4(aPos, 1.0)); //fragment position in world space
}   