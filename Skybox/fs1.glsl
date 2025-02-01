#version 330 core
out vec4 FragColor;

in vec3 Normal; 
in vec3 Position;
in vec2 TexCoords;

uniform vec3 cameraPos;
uniform samplerCube skybox;
uniform sampler2D woodenTexture;
uniform sampler2D woodenSpecularMap;


void main()
{      
    vec3 wood = vec3(texture(woodenTexture, TexCoords));
    float ratio = 1.00 / 1.52;
    vec3 I = normalize(Position - cameraPos);
    vec3 R = refract(I, -normalize(Normal), ratio);
    if (texture(woodenSpecularMap, TexCoords).r == 0.0f) //not metal
    {
        FragColor = texture(woodenTexture, TexCoords);
    }
    else
    {
        FragColor = vec4(texture(skybox, R).rgb, 1.0);
    }

    /*
    vec3 I = normalize(Position - cameraPos);
    vec3 R = reflect(I, normalize(Normal));
    FragColor = vec4(texture(skybox, R).rgb, 1.0);
    */

} 