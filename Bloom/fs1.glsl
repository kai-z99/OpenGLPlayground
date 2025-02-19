#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec2 TexCoord;

uniform sampler2D texture1;

struct PointLight {    
    vec3 position;   
    vec3 color; 
};  

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
uniform PointLight pointLights[4];


in vec3 FragPos;
in vec3 ourNormal;
uniform vec3 viewPos;

void main()
{       

    vec3 color = vec3(0.0f);

    vec3 viewDir = normalize(viewPos - FragPos);

    for (int i = 0; i < 4; i++)
    {
        color += CalcPointLight(pointLights[i], ourNormal, FragPos, viewDir);

    }
    
    float gamma = 2.2;
    vec3 sceneAmbient = vec3(0.015f, 0.015f, 0.015f) * pow(texture(texture1, TexCoord).rgb, vec3(gamma)); //degamma
    color += sceneAmbient;

    FragColor = vec4(color, 1.0f);

    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if (brightness > 1.0) BrightColor = vec4(color, 1.0);
    else BrightColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);

    //FragColor = vec4((pow(color.rgb, vec3(1.0/gamma))), 1.0f); //gamma correcttion
    
    
} 

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    float spec;

    spec = pow(max(dot(halfwayDir, normal), 0.0), 64.0f);
    
    // attenuation
    float distance    = length(light.position - fragPos);
    
    float attenuation = 1.0 / (distance * distance); //quadratic attenuation

    // combine results
    float gamma = 2.2;
    vec3 diffuseColor = pow(texture(texture1, TexCoord).rgb, vec3(gamma)); //degamma


    vec3 diffuse  = light.color  * diff * diffuseColor;
    vec3 specular = light.color  * spec;// * vec3(texture(material.specular, TexCoord)); not using map rn

    //ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;

    return (diffuse + specular);
} 