#version 330 core
out float FragColor;
  
in vec2 TexCoords;

uniform sampler2D gPosition;	//texture
uniform sampler2D gNormal;		//texture
uniform sampler2D texNoise;		//texture

uniform vec3 samples[64];
uniform mat4 projection;

// tile noise texture over screen, based on screen dimensions divided by noise size
const vec2 noiseScale = vec2(800.0/4.0, 600.0/4.0); // screen = 800x600 , 4x4 kernel

int kernelSize = 64;
float radius = 0.5;
float bias = 0.025;

void main()
{	
	//inputs
	vec3 fragPos = texture(gPosition, TexCoords).xyz;
	vec3 normal = normalize(texture(gNormal, TexCoords).rgb);
	vec3 randomVec = normalize(texture(texNoise, TexCoords * noiseScale).xyz);

	//TBN
	vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal)); //using the random rotation vector, create a tangent
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal); // tangent -> view

	float occlusion = 0.0f;
	for (int i = 0; i < kernelSize; i++)
	{
		vec3 samplePos = TBN * samples[i]; //convert hemisphere sample from tangent to view space
		samplePos = fragPos + samplePos * radius; //radius changes hemisphere size

		//project sample position to NDC so we can smaple the depth texture
		vec4 offset = vec4(samplePos, 1.0f);
		offset = projection * offset; //clip space
		offset.xyz /= offset.w; //perspective divide to [-1,1]
		offset.xyz = (offset.xyz * 0.5f) + 0.5f; //bring to [0,1] ready to sample depth texture

		float sampleDepth = texture(gPosition, offset.xy).z; //view-space depth value of the sampled fragment
		//range check
		float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck; 
	}

	occlusion = 1.0f - (occlusion / kernelSize);
	FragColor = occlusion;

}