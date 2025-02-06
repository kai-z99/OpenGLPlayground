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


struct DirectionalLight {
    vec3 direction;

    float intensity;
    
    float constant;
    float linear;
    float quadratic;  

    vec3 ambient; 
    vec3 diffuse;
    vec3 specular;

};

vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir, float shadow );
uniform DirectionalLight directionalLight;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir);

in vec3 ourNormal;
in vec3 FragPos;
in vec4 LightSpaceFragPos;

uniform vec3 viewPos;
uniform bool blinn;


void main()
{             

    vec3 norm = normalize(ourNormal);
    vec3 viewDir = normalize(viewPos - FragPos);


    //vec3 result = CalcPointLight(pointLight, norm, FragPos, viewDir);
    float shadow = ShadowCalculation(LightSpaceFragPos, norm, normalize(-directionalLight.direction));
    vec3 result = CalcDirectionalLight(directionalLight, norm, viewDir, shadow);


    FragColor = vec4(result, 1.0f);

} 

uniform sampler2D shadowMap;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir )
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w; //perspective divide to convert to [-1,1]
    projCoords = projCoords * 0.5 + 0.5;  //convert to NDC [0,1]

    float closestDepth = texture(shadowMap, projCoords.xy).r;   
    float currentDepth = projCoords.z;

    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);  
    float shadow = currentDepth - bias > closestDepth ? 1.0f : 0.0f;

    return shadow;
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
    /*
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance));    
    */

    float attenuation = 1.0 / (distance * distance); //quadratic attenuation

    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TexCoord));
    vec3 diffuse  = light.intensity * light.diffuse  * diff * vec3(texture(material.diffuse, TexCoord));
    vec3 specular = light.intensity * light.specular * spec;// * vec3(texture(material.specular, TexCoord)); not using map rn

    //ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
} 

vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir, float shadow)
{
    vec3 lightDir = normalize(-light.direction);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading

    float spec;

    if (blinn)
    {
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(halfwayDir, normal), 0.0), material.shininess);
    }
    else
    {
        vec3 reflectDir = reflect(-lightDir, normal);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    }

    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TexCoord));
    vec3 diffuse  = light.intensity * light.diffuse  * diff * vec3(texture(material.diffuse, TexCoord));
    vec3 specular = light.intensity * light.specular * spec; //* vec3(texture(material.specular, TexCoord));
    return (ambient + (1.0f - shadow) * (diffuse + specular));
}  