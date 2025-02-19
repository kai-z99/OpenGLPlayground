#version 330 core
out vec4 FragColor;
  
in vec2 TexCoord;

uniform sampler2D hdrScene;
uniform sampler2D blurScene;

uniform float exposure;

void main()
{             
    const float gamma = 2.2;
    vec3 hdrColor = texture(hdrScene, TexCoord).rgb;
    vec3 blurColor = texture(blurScene, TexCoord).rgb;
    hdrColor += blurColor;

    // exposure tone mapping
    vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);
    // gamma correction 
    mapped = pow(mapped, vec3(1.0 / gamma));
  
    FragColor = vec4(mapped, 1.0);

} 