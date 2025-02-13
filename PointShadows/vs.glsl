    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aNormal;
    layout (location = 2) in vec2 aTexCoord;
    layout (location = 3) in vec3 aTangent;


    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    out vec2 TexCoord;
    out vec3 FragPos;

    out mat3 TBN;
    out vec3 ourNormal;

    void main()
    {
        gl_Position = projection * view * model * vec4(aPos, 1.0);  //frag position in screen space

        mat3 normalMatrix = mat3(transpose(inverse(model)));
        vec3 N = normalMatrix * aNormal;                         //Normal, taking into account non-linear scaling
        vec3 T = normalMatrix * aTangent;
        vec3 B = cross(N,T);
        mat3 tbn = mat3(T, B, N);
   

        TexCoord = aTexCoord;                                      // texture coords
        FragPos = vec3(model * vec4(aPos, 1.0));                   //fragment position in world space
        ourNormal = N;
        TBN = tbn;
    }