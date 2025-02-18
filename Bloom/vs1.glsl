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

void main()
{
    mat3 normalMatrix = mat3(transpose(inverse(model)));
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    TexCoord = aTexCoord;                                      // texture coords
    ourNormal = normalMatrix * aNormal;
    FragPos = vec3(model * vec4(aPos, 1.0f));
}