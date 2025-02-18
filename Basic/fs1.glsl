#version 330 core
out vec4 FragColor;
in vec3 ourColor;
in vec2 TexCoord;
in vec3 ourNormal;
in vec3 FragPos;

uniform sampler2D minionTexture;
uniform sampler2D matrixTexture; //emission map

uniform float mixStrength;
uniform int useTexture;
uniform vec3 viewPos;

uniform float t;

uniform vec3 globalAmbient;

struct Material
{
    vec3 ambient;       //ambient color (grey for now)
    sampler2D diffuse;  //texture (diffuse map)
    sampler2D specular; //specular map
    float shininess;
};

uniform Material material;

struct DirLight {
    vec3 direction;
    
    float intensity;

    vec3 diffuse;
    vec3 specular;
};  
uniform DirLight dirLight;
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);

struct PointLight {    
    vec3 position;

    float intensity;
    
    float constant;
    float linear;
    float quadratic;  

    vec3 ambient; //attenuated
    vec3 diffuse;
    vec3 specular;
};  
#define NR_POINT_LIGHTS 1
uniform PointLight pointLights[NR_POINT_LIGHTS];
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);


struct SpotLight {
    vec3 position;
    vec3 direction;

    float intensity;

    float cutOff;
    float outerCutOff;

    vec3 diffuse;
    vec3 specular;

};
uniform SpotLight spotlight;
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);


void main()
{
    // properties
    vec3 norm = normalize(ourNormal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // phase 1: Directional lighting
    vec3 result = CalcDirLight(dirLight, norm, viewDir);
    // phase 2: Point lights
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);    
    // phase 3: Spot light
    result += CalcSpotLight(spotlight, norm, FragPos, viewDir); 
    
    //phase 4: global ambient
    result += globalAmbient * material.ambient * vec3(texture(material.diffuse, TexCoord)); //ambientfactor * ambientcolor * texture(optional)

    //phase 5: emission
    vec3 emission = vec3(0.0f);

    vec3 specularMapFragmentColor = vec3(texture(material.specular, TexCoord));
    if (specularMapFragmentColor == vec3(0.0f)) //if its not the metal edge part
    {
        emission = vec3(texture(matrixTexture, vec2(TexCoord.x, TexCoord.y + t)));
        emission *= max(sin(t + 3.141) , 0.0f);
    }


    result += emission;
    
    FragColor = vec4(result, 1.0);
}


vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    //vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TexCoord));
    vec3 diffuse  = light.intensity * light.diffuse  * diff * vec3(texture(material.diffuse, TexCoord));
    vec3 specular = light.intensity * light.specular * spec * vec3(texture(material.specular, TexCoord));
    return (diffuse + specular);    
}  


vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TexCoord));
    vec3 diffuse  = light.intensity * light.diffuse  * diff * vec3(texture(material.diffuse, TexCoord));
    vec3 specular = light.intensity * light.specular * spec * vec3(texture(material.specular, TexCoord));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
    //return (diffuse + specular);
} 

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess); 
    // combine results
    vec3 diffuse  = light.intensity * light.diffuse  * diff * vec3(texture(material.diffuse, TexCoord));
    vec3 specular = light.intensity * light.specular * spec * vec3(texture(material.specular, TexCoord));
    
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon   = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0); 

    diffuse  *= intensity;
    specular *= intensity;


    //return (ambient + diffuse + specular);
    return diffuse + specular;
}



/*
void main()
{      
    //--//WORKING IN WORLD SPACE//--//

            

    //AMBIENT--------------------------------------------------
    vec3 ambient =  vec3(mix(texture(material.diffuse, TexCoord), texture(minionTexture, TexCoord), mixStrength)) * light.ambient ;
    //light.ambient is (0.2, 0.2, 0.2) so it adds some slight white/grey tinted texture to everything.

    //DIFFUSE--------------------------------------------------
    vec3 norm = normalize(ourNormal);
    vec3 lightDir = normalize(light.position - FragPos); 
    float diff = max(dot(norm, lightDir), 0.0);  // The scaling param based on angle. A dot B = cos(angle between AB) when A and B are normalized. 
                                                 // cos(angle between normal and light dir) = [0,1] for 0deg -> 90deg. eg: for 0 degrees, 
                                                    // light is dir == norm, so should have the highest value therefore simluting direct lighting.
                                                 // same ideas for [1,0] -90deg -> 0deg.
                                                 // cos() is only positive 0-1 between -90 and 90, so just set 0 if its not in that range. 
                                                  
    vec3 diffuse = diff * vec3(mix(texture(material.diffuse, TexCoord), texture(minionTexture, TexCoord), mixStrength)) * light.diffuse;
    //light.diffuse is the color of the light, so it adds the texture tinted the color of the light based on light direction and normal direction.

    //SPECULAR-------------------------------------------------
    //float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos); //direction from frag pos to camera pos (eye look direction)
    vec3 reflectDir = reflect(-lightDir, norm);  //direction thats light reflects off normal (light ray direction)
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess); // see how close those 2 directions are aligned, if theyre closer, make that fragment brighter.
                                                                              // It makes sense since when you look at a light ray directly towards you its the strongest.
                                                                              // Pow by some value called the shinyness.
    vec3 specular = spec * vec3(texture(material.specular, TexCoord)) * light.specular; 
    // light.specular is the light color, so it adds the light color to areas based on the view and light reflect direction and specular map.


    //EMISSION-------------------------------------------------
    vec3 emission = vec3(0.0f);

    vec3 specularMapFragmentColor = vec3(texture(material.specular, TexCoord));
    if (specularMapFragmentColor == vec3(0.0f)) //if its not the metal edge part
    {
        emission = vec3(texture(matrixTexture, vec2(TexCoord.x, TexCoord.y + t)));
        emission *= max(sin(t + 3.141) , 0.0f);
    }
    //Note: the emission is added to the color that phong calcaulted, but its generally way brighter (0,1,0) for the matrix green compared to
    //to the value that color that phong genreated like (0.3 0.1, 0.2), meaning it easiliy stands out and looks almost pure green.


    //ATTENUATION----------------------------------------------(Point light)
    float distance = length(light.position - FragPos);
    float attenuation = 1.0f / (light.constant + (light.linear * distance) + (light.quadratic * distance * distance));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    //DIRECTIONAL LIGHTING
    //----------------------------------------------------------
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon   = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0); 
    diffuse  *= intensity;
    specular *= intensity;

    //OUTPUT---------------------------------------------------
    //result = ambient + diffuse + specular + emission;
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0f);
}
*/












/*
if (useTexture == 1) 
    {
        FragColor = vec4(ourColor, 1.0f) * mix(texture(t1, TexCoord), texture(t2, TexCoord), mixStrength );
    }

    else 
    {
        FragColor = vec4(ourColor, 1.0f); // use this for the right triangle
    } 

*/

// texture(t2, vec2(TexCoord.x * cos(2.0f * 3.141f * mixStrength), TexCoord.y * sin(2.0f * 3.141f * mixStrength))), mixStrength )


/*
    float phase = mod(ourFrame, 10000.0) / 10000.0;

    if (phase < 1.0 / 3.0) 
    {
        float t = phase * 3.0; // Normalize to [0, 1]
        FragColor = vec4(mix(ourColor.r, ourColor.g, t),
                         mix(ourColor.g, ourColor.b, t),
                         mix(ourColor.b, ourColor.r, t),
                         1.0);
    }

    else if (phase < 2.0 / 3.0) 
    {
        float t = (phase - 1.0 / 3.0) * 3.0; 
        FragColor = vec4(mix(ourColor.g, ourColor.b, t),
                         mix(ourColor.b, ourColor.r, t),
                         mix(ourColor.r, ourColor.g, t),
                         1.0);
    }

    else 
    {
        float t = (phase - 2.0 / 3.0) * 3.0; 
        FragColor = vec4(mix(ourColor.b, ourColor.r, t),
                         mix(ourColor.r, ourColor.g, t),
                         mix(ourColor.g, ourColor.b, t),
                         1.0);
    }
*/