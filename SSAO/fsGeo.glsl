#version 330 core
layout (location = 0) out vec4 gNormal; //viewspace
layout (location = 1) out vec4 gPosition; //viewspace

in vec2 TexCoords;
in vec3 FragPos; //viewspace
in vec3 Normal; //viewspace

void main()
{    
    // store the per-fragment normals into the gbuffer
    gNormal = vec4(normalize(Normal),1.0f);
    //gNormal = vec4(0.0f, 1.0f, 0.0f, 1);
    // store position
    gPosition = vec4(FragPos,1);
   // gPosition = vec4(0.0f, 1.0f, 0.0f, 1);

}