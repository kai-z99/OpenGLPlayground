#version 330 core
out vec4 FragColor;


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

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, float shadow);
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

float ShadowCalculation(vec3 lightPos);

in vec2 TexCoord;
in vec3 ourNormal;
in vec3 FragPos;

uniform vec3 viewPos;
uniform bool blinn;
uniform samplerCube shadowMap;
uniform vec3 lightPos;
uniform float farPlane;

void main()
{             

    vec3 norm = normalize(ourNormal);
    vec3 viewDir = normalize(viewPos - FragPos);

    float shadow = ShadowCalculation(FragPos);
    vec3 result = CalcPointLight(pointLight, norm, FragPos, viewDir, shadow);

    FragColor = vec4(result, 1.0f);

} 



float ShadowCalculation(vec3 fragPos)
{
    //vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

    vec3 lightToFrag = fragPos - lightPos;
    float closestDepth = texture(shadowMap, normalize(lightToFrag)).r;

    closestDepth *= farPlane;

    float currentDepth = length(lightToFrag);

    //if (currentDepth > 1.0f) return 0.0f; //Outside the far plane

    float bias = 0.05;
    //float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);  

    float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;
    
    //Finish!
    return shadow;
}


vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, float shadow)
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

    return (ambient + (1.0f - shadow) * (diffuse + specular));
} 

/*
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
    vec3 diffuse  = light.intensity * light.diffuse  * diff * vec3(texture(material.diffuse, TexCoord)); //light.diffuse is the color is paints(lightCol?), the texture is the texture where its painted on top of (woodTexture)
    vec3 specular = light.intensity * light.specular * spec; // vec3(texture(material.specular, TexCoord)); light.specular is the color it paints (lightCol?)
    return (ambient + (1.0f - shadow) * (diffuse + specular));
}  
*/