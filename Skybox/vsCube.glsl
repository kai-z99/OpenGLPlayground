#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 TexCoord;  //cubemap direction       

void main()
{
    TexCoord = aPos;   //cubemap direction  
    vec4 pos = projection * view * model * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
                                  
}