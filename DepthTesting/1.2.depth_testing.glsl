#version 330 core
out vec4 FragColor;
in vec2 TexCoord;
float near = 0.1; 
float far = 100.0; 

uniform sampler2D texture1;

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
    return 1;
}

void main()
{             
    float depth = LinearizeDepth(gl_FragCoord.z) / far; // divide by far to get depth in range [0,1] for visualization purposes
    //FragColor = vec4(vec3(depth), 1.0);
    //FragColor = vec4(vec3(gl_FragCoord.z), 1.0);

    //FragColor = vec4(texture(texture1, TexCoord).rgb, 1.0);

    vec4 col = texture(texture1, TexCoord);
    if (col.a < 0.1) discard;

    FragColor = col;

}