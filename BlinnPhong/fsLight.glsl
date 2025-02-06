#version 330 core
out vec4 FragColor;

uniform vec3 lightColor;

void main()
{
    FragColor = vec4(lightColor, 1.0); // a cube, that doesnt acutally emit light remember its purely a visual
    
}