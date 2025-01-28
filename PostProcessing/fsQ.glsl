#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D screenTexture;

const float offset = 1.0f / 100.0f;

void main()
{ 
    //FragColor = texture(screenTexture, TexCoords);

    //inversion
    //FragColor = vec4(vec3(1.0f - texture(screenTexture, TexCoords)), 1.0f);

    //grayscale
    /*
    vec4 col = texture(screenTexture, TexCoords);
    float average = (col.r + col.g + col.b) / 3.0f;
    FragColor = vec4(average, average, average, 1.0f);
    */

    //kernel
    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // top-left
        vec2( 0.0f,    offset), // top-center
        vec2( offset,  offset), // top-right
        vec2(-offset,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset,  0.0f),   // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0f,   -offset), // bottom-center
        vec2( offset, -offset)  // bottom-right   
    );

    //blur matrix
    /*
    float convolutionMatrix[9] = float[](
        1.0/16, 2.0/16,  1.0/16,
        2.0/16,  4.0/16, 2.0/16,
        1.0/16, 2.0/16, 1.0/16
    );
    */

    //cool neon edge detection
    /*
    float convolutionMatrix[9] = float[](
        1, 0, 1,
        0, -4, 0,
        1, 0, 1
    );
    */

    // Slight blur convolution matrix
    float convolutionMatrix[9] = float[](
    0.0, 0.0625, 0.0,
    0.0625, 0.75, 0.0625,
    0.0, 0.0625, 0.0
);

    /*
    // sobel
    float convolutionMatrix[9] = float[](
        -2, -1, 0,
        -1,  1, 1,
        0, 1, 2
    );
    */

    vec3 sampleTex[9];

    for (int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
    }

    vec3 col = vec3(0.0f);
    for (int i = 0; i < 9; i++)
    {
        col += sampleTex[i] * convolutionMatrix[i];
    }

    //some cool vhs stuff
    vec2 redShift = vec2(offset, 0.0); // Shift red to the right
    vec2 blueShift = vec2(-offset, 0.0); // Shift blue to the left

    float r = texture(screenTexture, TexCoords.st + redShift).r;
    float g = col.g; // keep green channel from the blurred image
    float b = texture(screenTexture, TexCoords.st + blueShift).b;

    FragColor = vec4(vec3(r,g,b), 1.0f);





}