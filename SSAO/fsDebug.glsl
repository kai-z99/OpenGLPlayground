#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D scene;

void main()
{          
    vec3 c = texture(scene, TexCoords).rgb;
    //float c1 = texture(scene, TexCoords).r;
    FragColor = vec4(c, 1.0f);
    //FragColor = vec4(c1, c1, c1, 1.0f);
} 