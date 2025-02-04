#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;

struct Material
{
    vec3 ambient;       //ambient color (grey for now)
    sampler2D diffuse;  //texture (diffuse map)
    sampler2D specular; //specular map
    float shininess;
};

uniform Material material;

struct PointLight {    
    vec3 position;

    float intensity;
    
    float constant;
    float linear;
    float quadratic;  

    vec3 ambient; 
    vec3 diffuse;
    vec3 specular;
};  

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
uniform PointLight pointLight;

in vec3 ourNormal;
in vec3 FragPos;

uniform vec3 viewPos;
uniform bool blinn;

void main()
{             

    vec3 norm = normalize(ourNormal);
    vec3 viewDir = normalize(viewPos - FragPos);


    vec3 result = CalcPointLight(pointLight, norm, FragPos, viewDir);

    FragColor = vec4(result, 1.0f);
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

    if (blinn)
    {
        spec = pow(max(dot(halfwayDir, normal), 0.0), material.shininess);
    }
    else
    {
        spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    }
    
    // attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance));    

    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TexCoord));
    vec3 diffuse  = light.intensity * light.diffuse  * diff * vec3(texture(material.diffuse, TexCoord));
    vec3 specular = light.intensity * light.specular * spec;// * vec3(texture(material.specular, TexCoord)); not using map rn

    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
} 