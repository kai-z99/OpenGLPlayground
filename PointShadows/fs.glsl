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

vec3 CalcPointLight(PointLight light, mat3 TBN, vec3 fragPos, vec3 viewDir, float shadow);
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
in vec3 ourTangent;
in vec3 FragPos;
in mat3 TBN;

uniform vec3 viewPos;
uniform bool blinn;
uniform samplerCube shadowMap;
uniform sampler2D normalMap;
uniform vec3 lightPos;
uniform float farPlane;
uniform float currentFrame;
uniform bool scrollNormal;



void main()
{             

    vec3 norm = normalize(ourNormal);
    vec3 viewDir = normalize(viewPos - FragPos);

    float shadow = ShadowCalculation(FragPos);
    vec3 result = CalcPointLight(pointLight, TBN, FragPos, viewDir, shadow);

    float gamma = 2.2;
    FragColor.rgb = (pow(result.rgb, vec3(1.0/gamma)));
    FragColor.a = 1.0f;
    //FragColor = vec4(result, 1.0f);

} 


vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);   

float ShadowCalculation(vec3 fragPos)
{
    //vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

    vec3 lightToFrag = fragPos - lightPos;
    float currentDepth = length(lightToFrag); //[0, farPlane] good
      
    //float bias = 0.05;
    float bias = max(0.05 * (1.0 - dot(normalize(ourNormal), normalize(lightToFrag))), 0.005);  
    float shadow = 0.0f;
    int samples = 20;
    float viewDistance = length(viewPos - fragPos);
    float diskRadius = (1.0 + (viewDistance / farPlane)) / 25.0; 
    
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(shadowMap, lightToFrag + sampleOffsetDirections[i] * diskRadius).r;
        closestDepth *= farPlane;   // move to [0, farPlane]

        if (currentDepth - bias > closestDepth) shadow += 1.0;
            
    }

    shadow /= float(samples);  
    

    //float closestDepth = texture(shadowMap, lightToFrag).r;
    //closestDepth *= farPlane;
    //if (currentDepth - bias > closestDepth) shadow += 1.0;
    
    //Finish!
    return shadow;
}


vec3 CalcPointLight(PointLight light, mat3 TBN, vec3 fragPos, vec3 viewDir, float shadow)
{
    vec3 lightDir = normalize(light.position - fragPos);

    //normal = ourNormal;
    
    vec3 normal;
    
    if (scrollNormal)
    {
        normal = texture(normalMap, vec2(TexCoord.x, TexCoord.y + (currentFrame * 0.05))).rgb;
    }
    else
    {
        normal = texture(normalMap, TexCoord).rgb;
    }
    
    //vec3 normal = texture(normalMap, vec2(TexCoord.x, TexCoord.y + (currentFrame * 0.05))).rgb;
    //normal = texture(normalMap, TexCoord).rgb;
    normal = normal * 2.0 - 1.0; 
    
    normal = normalize(TBN * normal); 
    //normal = ourNormal;
    //normal = ourTangent;
    //if (normal.z > 0.5) return vec3(0.0f, 1.0f, 0.0f);
    //else return vec3(1.0f, 0.0f, 0.0f);

    //return vec3(normal);

    //normal = ourNormal;
    // diffuse shading

    float diff = max(dot(normal, lightDir), 0.0);

   // return lightDir;
    //return vec3(diff, diff, diff);
    
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
    
    
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    

    //float attenuation = 1.0 / (distance * distance); //quadratic attenuation

    // combine results

    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TexCoord));

    float gamma = 2.2;
    vec3 diffuseColor = pow(texture(material.diffuse, TexCoord).rgb, vec3(gamma));

    vec3 diffuse  = light.intensity * light.diffuse  * diff * diffuseColor;
   
    vec3 specular = light.intensity * light.specular * spec;// * vec3(texture(material.specular, TexCoord)); not using map rn
    //ambient  *= attenuation;
    //diffuse  *= attenuation;
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