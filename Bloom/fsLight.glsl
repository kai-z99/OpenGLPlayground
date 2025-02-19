#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

uniform vec3 lightColor;

void main()
{
    FragColor = vec4(lightColor, 1.0f); // a cube, that doesnt acutally emit light remember its purely a visual

    float brightness = dot(FragColor.rgb, vec3(0.2126f, 0.7152f, 0.0722f)); //Luminance formula, based on human vision
    if (brightness > 1.0) BrightColor = vec4(lightColor, 1.0f);
    else BrightColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);

}