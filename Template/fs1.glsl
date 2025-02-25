#version 330 core
out vec4 FragColor;


uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

struct PointLight {    
    vec3 position;   
    vec3 color; 
};  

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
uniform PointLight pointLight;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 ourNormal;
uniform vec3 viewPos;


void main()
{          
    vec3 normal = normalize(ourNormal);
    vec3 color = vec3(0.0f);

    vec3 viewDir = normalize(viewPos - FragPos);

    color += CalcPointLight(pointLight, normal, FragPos, viewDir);

    float gamma = 2.2;
    vec3 sceneAmbient = vec3(0.015f, 0.015f, 0.015f) * pow(texture(texture_diffuse1, TexCoords).rgb, vec3(gamma)); //degamma
    color += sceneAmbient;

    color = pow(color, vec3(1.0 / gamma));
    FragColor = vec4(color, 1.0f);
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
    vec3 diffuseColor = pow(texture(texture_diffuse1, TexCoords).rgb, vec3(gamma)); //degamma

    vec3 diffuse  = 3 * light.color  * diff * diffuseColor;
    vec3 specular = 3 * light.color  * spec * vec3(texture(texture_specular1, TexCoords));

    //ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    //return diffuse;
    return (diffuse + specular);
} 