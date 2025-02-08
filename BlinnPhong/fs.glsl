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

in vec2 TexCoord;
in vec3 ourNormal;
in vec3 FragPos;
in vec4 LightSpaceFragPos;

uniform vec3 viewPos;
uniform bool blinn;


void main()
{             

    vec3 norm = normalize(ourNormal);
    vec3 viewDir = normalize(viewPos - FragPos);

    float shadow = ShadowCalculation(LightSpaceFragPos, norm, normalize(-directionalLight.direction));
    vec3 result = CalcDirectionalLight(directionalLight, norm, viewDir, shadow);


    FragColor = vec4(result, 1.0f);

} 

uniform sampler2D shadowMap;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir )
{
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

    //Firstly, we are working in a "snapshot" view. fragPosLightSpace is in clipspace, where the clip is 
    //the pov of our light. (A "snapshot") The reason why its in this view is because our lightMatrix has
    //a view and projection matrix (the one that sends to clip space)

    //Secondly, our shadow map is also in a "snapshot" of the SAME clipspace, since we just rendered it in our first pass
    //with the EXACT same lightMatrix.

    //THE GOAL: Compare the depth values of the fragment and shadowMap value of these 2 snapshots to determine if the fragment is in a shadow.

    //Currently, FragPosLightSpace is in clipspace and in the form (x,y,z,w) but each of those attributes are not in the range [-1,1].
    //The reason for this is that we did not assign this varible to gl_Position. When you assign something to gl_Position,
    //it automattically performs a persective divide (x,y,z,w)/w. But since we didnt, we should do it ourselves.
    //Once we move it to [-1,1] its now in the NDC range, that it would have been if we assigned it with gl_Position.

    //NOTE: This is all assuming that the fragpos is in the light's frustum.
    // Projected coordinates outside the light's frustum are higher than 1.0 and will thus sample the depth 
    // texture outside its default range of [0,1]. This is why we cant use GL_REPEAT for that texture.
    
    //Well, why cant we just say anything outside the frustum is not a shadow, aka check if any of the coordinates
    //of projecoords > 1.0f? Well thats just the wrong philosophy. Removing GL_REPEAT and setting value of shadowMap to 1.0f
    //anytime something outside of xy[0,1] is sampled is more consistant with the philosphy of design.
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w; //perspective divide to convert to [-1,1]

    //At this state its like we assinged it with gl_Position. But since we want to index with our shadowMap which is in the range
    // [0,1], lets do some basic algebra to convert out [-1,1] NDC coords to [0,1], suitible for indexing the shadowMap.

    projCoords = projCoords * 0.5 + 0.5;  

    //Now both our light POV "snapshot" and shadow map are in [0,1] we can compare the depth values to check if
    //the fragment is in a shadow.

    //IN CONCLUSION: The point of those 2 lines is to match both our 'snapshot' and shadowMap to [0,1] so theyre comparisons are valid.
    float closestDepth = texture(shadowMap, projCoords.xy).r;   
    float currentDepth = projCoords.z; 

    //"A light-space projected fragment z coordinate is further than the light's far plane when its z coordinate is larger than 1.0"
    //The formula to calculate the z cooridnate of the frag is: z = 0 = near and z = 1 = far. So anything past the far plane is >1.0.
    //Essentially this is out of bounds, so just dont cast a shadow as its not pasrt of the calculation anymore.
    if (currentDepth > 1.0f) return 0.0f; //Outside the far plane

    //SHADOW ACNE: Look at the floor in front of you, the resolution of that floor is perfect. But in the shadowMap's "snapshot" of that same view,
    //The resolution of the shadowMap is not perfect so there will be little shadow ridges. So why would you try to compare these 2 ? Even an extremely small closetDepth
    //discrepency will change the shadow from 0.0 to 1.0f (when currentDepth is underneath a shadow ridge). The solution is to drag currentDepth forward slightly so the curentDepth is
    //never underneath one of the shadow ridges.
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);  

    float shadow = 0.0;
    //Study this later
    
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
        float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
        shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    //shadow = currentDepth - bias > closestDepth ? 1.0f : 0.0f;

    
    //Finish!
    return shadow;
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
    vec3 diffuse  = light.intensity * light.diffuse  * diff * vec3(texture(material.diffuse, TexCoord)); //light.diffuse is the color is paints(lightCol?), the texture is the texture where its painted on top of (woodTexture)
    vec3 specular = light.intensity * light.specular * spec; // vec3(texture(material.specular, TexCoord)); light.specular is the color it paints (lightCol?)
    return (ambient + (1.0f - shadow) * (diffuse + specular));
}  






/*
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
*/