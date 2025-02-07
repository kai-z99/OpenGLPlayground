#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/mesh.h>

//#include "../ShareLib/Shader.h"
#include "Shader.h"
#include "Camera.h"
#include "stb_image.h"

#include <iostream>
#include <random>


#define RAINBOW_LIGHT


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void drawRecursiveTransform(unsigned int transformLoc, glm::vec3 translate, glm::mat4 transform, int depth);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
unsigned int loadTexture(char const* path);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float mixValue = 0.4f;

Camera camera = Camera(glm::vec3(0.0f, 1.0f, 3.0f));

float lastX = 800/2;
float lastY = 600/2;
bool firstMouse = true;

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
glm::vec3 globalAmbient = glm::vec3(0.13f, 0.13f, 0.13f);

bool flashLightOn = true;

glm::vec3 testLightPosition = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 testLightColor = glm::vec3(1.0f, 1.0f, 1.0f);


int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    //Z BUFFER
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // build and compile our shader program
    // ------------------------------------
    Shader ourShader("vs1.glsl", "fs1.glsl"); 
    Shader lightCubeShader("vs1.glsl", "fsLight.glsl");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    // range: -1.0 to 1.0 for NDC, 0.0. to 1.0 for tex coords
    float cubeVertices[] = {
        // ————————————————  BACK FACE  ————————————————
        //   positions           colors            tex coords    normal (temp)               
        -0.5f, -0.5f, -0.5f,    1.0f, 0.0f, 0.0f,  0.0f, 0.0f,   0.0f, 0.0f, -1.0f,//back bottom left
         0.5f,  0.5f, -0.5f,    0.0f, 1.0f, 0.0f,  1.0f, 1.0f,   0.0f, 0.0f, -1.0f,//back top right
         0.5f, -0.5f, -0.5f,    0.0f, 0.0f, 1.0f,  1.0f, 0.0f,   0.0f, 0.0f, -1.0f,//back bottom right

         0.5f,  0.5f, -0.5f,    0.0f, 1.0f, 0.0f,  1.0f, 1.0f,   0.0f, 0.0f, -1.0f,//back top right
        -0.5f, -0.5f, -0.5f,    1.0f, 0.0f, 0.0f,  0.0f, 0.0f,   0.0f, 0.0f, -1.0f,//back bottom left
        -0.5f,  0.5f, -0.5f,    0.0f, 0.0f, 1.0f,  0.0f, 1.0f,   0.0f, 0.0f, -1.0f,//bacl top left

        // ————————————————  FRONT FACE  ————————————————
        -0.5f, -0.5f,  0.5f,    0.0f, 1.0f, 0.0f,  0.0f, 0.0f,  0.0f, 0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,    0.0f, 1.0f, 0.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,    0.0f, 1.0f, 0.0f,  1.0f, 1.0f,  0.0f, 0.0f, 1.0f,

         0.5f,  0.5f,  0.5f,    0.0f, 1.0f, 0.0f,  1.0f, 1.0f,  0.0f, 0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,    0.0f, 1.0f, 0.0f,  0.0f, 1.0f,  0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,    0.0f, 1.0f, 0.0f,  0.0f, 0.0f,  0.0f, 0.0f, 1.0f,

        // ————————————————  LEFT FACE   ————————————————
        -0.5f,  0.5f,  0.5f,    0.0f, 0.0f, 1.0f,  0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,    0.0f, 0.0f, 1.0f,  1.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,    0.0f, 0.0f, 1.0f,  1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,    0.0f, 0.0f, 1.0f,  1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,    0.0f, 0.0f, 1.0f,  0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,    0.0f, 0.0f, 1.0f,  0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,

        // ————————————————  RIGHT FACE  ————————————————
         0.5f,  0.5f,  0.5f,    1.0f, 0.0f, 1.0f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,    1.0f, 0.0f, 1.0f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,    1.0f, 0.0f, 1.0f,  1.0f, 1.0f,  1.0f, 0.0f, 0.0f,

         0.5f, -0.5f, -0.5f,    1.0f, 0.0f, 1.0f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,    1.0f, 0.0f, 1.0f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,    1.0f, 0.0f, 1.0f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f,

         // ————————————————   TOP FACE   ————————————————
         -0.5f,  0.5f, -0.5f,    1.0f, 1.0f, 0.0f,  0.0f, 1.0f,  0.0f, 1.0f, 0.0f,
          0.5f,  0.5f,  0.5f,    1.0f, 1.0f, 0.0f,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f,
          0.5f,  0.5f, -0.5f,    1.0f, 1.0f, 0.0f,  1.0f, 1.0f,  0.0f, 1.0f, 0.0f,

          0.5f,  0.5f,  0.5f,    1.0f, 1.0f, 0.0f,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f,
         -0.5f,  0.5f, -0.5f,    1.0f, 1.0f, 0.0f,  0.0f, 1.0f,  0.0f, 1.0f, 0.0f,
         -0.5f,  0.5f,  0.5f,    1.0f, 1.0f, 0.0f,  0.0f, 0.0f,  0.0f, 1.0f, 0.0f,

         // ———————————————— BOTTOM FACE ————————————————
         -0.5f, -0.5f, -0.5f,    1.0f, 1.0f, 1.0f,  0.0f, 1.0f,  0.0f, -1.0f, 0.0f,
          0.5f, -0.5f, -0.5f,    1.0f, 1.0f, 1.0f,  1.0f, 1.0f,  0.0f, -1.0f, 0.0f,
          0.5f, -0.5f,  0.5f,    1.0f, 1.0f, 1.0f,  1.0f, 0.0f,  0.0f, -1.0f, 0.0f,

          0.5f, -0.5f,  0.5f,    1.0f, 1.0f, 1.0f,  1.0f, 0.0f,  0.0f, -1.0f, 0.0f,
         -0.5f, -0.5f,  0.5f,    1.0f, 1.0f, 1.0f,  0.0f, 0.0f,  0.0f, -1.0f, 0.0f,
         -0.5f, -0.5f, -0.5f,    1.0f, 1.0f, 1.0f,  0.0f, 1.0f,  0.0f, -1.0f, 0.0f,
    };

    float planeVertices[] =
    {                       //tex           //norm
       -1.0f,-1.0f, 0.0f,    0.0f, 0.0f,    0.0f, 0.0f, 1.0f,   //bl
        1.0f,-1.0f, 0.0f,    1.0f, 0.0f,    0.0f, 0.0f, 1.0f,   //br
        1.0f, 1.0f, 0.0f,    1.0f, 1.0f,    0.0f, 0.0f, 1.0f,   //tr

        1.0f, 1.0f, 0.0f,    1.0f, 1.0f,    0.0f, 0.0f, 1.0f,   //tr
       -1.0f, 1.0f, 0.0f,    0.0f, 1.0f,    0.0f, 0.0f, 1.0f,   //tl
       -1.0f,-1.0f, 0.0f,    0.0f, 0.0f,    0.0f, 0.0f, 1.0f,   //bl

    };


    glm::vec3 cubePositions[10000];
    int ind = 0;


    for (float x = -50; x <= 50; x += 10)
    {
        for (float y = -50; y <= 50; y += 10)
        {
            for (float z = -50; z <= 50; z += 10)
            {
                cubePositions[ind++] = glm::vec3(x, y, z);
            }
        }
        
    }

    ind = 0;

    std::random_device rd; // Seed generator
    std::mt19937 gen(rd()); // Mersenne Twister engine seeded with rd()
    std::uniform_int_distribution<> dis(-60, 60); // Range [1, 100]

    glm::vec3 pointLightPositions[100];

    for (int i = 0; i < 100; i++)
    {
        pointLightPositions[ind++] = glm::vec3(dis(gen), dis(gen), dis(gen));
    }


    unsigned int VAO, VBO, EBO;
    //generation
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO); //were working on leftVAO

    //create vertex buffer AKA VBO stuff----------------------------------------------------
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW); //for VBO
   
    //NOW configure the vertex buffer attributes.-=---------------------------------------
    // position attribute (index 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0); //in vs1: layout (location = 0) in vec3 aPos;
    glEnableVertexAttribArray(0); //vao stores this
    // color attribute (index 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float))); //in vs1: layout (location = 1) in vec3 aColor;
    glEnableVertexAttribArray(1); //vao stores this
    // tex coords attribute (index 2)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float))); //in vs1: layout (location = 2) in vec2 aTexCoord;
    glEnableVertexAttribArray(2);  
    // normals attribute (index 3)
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float))); //in vs1: layout (location = 3) in vec2 aNormal;
    glEnableVertexAttribArray(3);

    //EBO stuff. (for glDrawElements)
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); 
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


    //LIGHT
    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO); //working on lightVAO
    // we only need to bind to the VBO, the container's VBO's data already contains the data.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // set the vertex attribute 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0); //in vs1: layout (location = 0) in vec3 aPos;
    glEnableVertexAttribArray(0); //vao stores this
    //Note: We did not configure: layout (location = 1) in vec3 aColor; and layout (location = 2) in vec2 aTexCoord; so they will get default values.


    //FLOOR
    unsigned int planeVAO;
    unsigned int planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glBindVertexArray(planeVAO); //working on planeVAO

    //VBO
    glGenBuffers(1, &planeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW); 
    //position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); //location = 0
    glEnableVertexAttribArray(0); 
    //texcoord
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); //location = 2
    glEnableVertexAttribArray(2);
    //norm
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float))); //location = 2
    glEnableVertexAttribArray(3);


    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    // glBindVertexArray(0);

    unsigned int woodenCrateTexture, woodenCrateSpecularMapTexture, minionTexture, matrixTexture;
    glGenTextures(1, &woodenCrateTexture);
    glGenTextures(1, &woodenCrateSpecularMapTexture);
    glGenTextures(1, &minionTexture);
    glGenTextures(1, &matrixTexture);

    //WOODEN CRATE TEXTURE-----------------------------------------------------------------------------------------------
    woodenCrateTexture = loadTexture("../ShareLib/Resources/container2.png");
    //-------------------------------------------------------------------------------------------------------------
    woodenCrateSpecularMapTexture = loadTexture("../ShareLib/Resources/container2_specular.png");
    stbi_set_flip_vertically_on_load(true);
    minionTexture = loadTexture("../ShareLib/Resources/minion.jpg");
    matrixTexture = loadTexture("../ShareLib/Resources/matrix.jpg");


    ourShader.use(); //need to use shader program atleast once to access glUniform
    

    glUniform3fv(glGetUniformLocation(ourShader.ID, "globalAmbient"), 1, glm::value_ptr(globalAmbient));

    //MATERIAL UNIFORM-------------------------------------------------------------------
    glUniform3fv(glGetUniformLocation(ourShader.ID, "material.ambient"), 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f))); //the color of the ambient on material
    glUniform1i(glGetUniformLocation(ourShader.ID, "material.diffuse"), 0); // GL_TEXTURE0, (wood)
    glUniform1i(glGetUniformLocation(ourShader.ID, "material.specular"), 1); // GL_TEXTURE1, (wood specular map)
    glUniform1f(glGetUniformLocation(ourShader.ID, "material.shininess"), 32.0f); //pow!

    //extra textures---------------------------------------------------------------------
    glUniform1i(glGetUniformLocation(ourShader.ID, "minionTexture"), 2); // GL_TEXTURE2, (minion texture) 
    glUniform1i(glGetUniformLocation(ourShader.ID, "matrixTexture"), 3); // GL_TEXTURE3, (emission map texture)
    
    // CONSTANT LIGHTING UNIFORMS---------------------------------------------------------
    // point lights //
    //for (int i = 0; i < 100; i++)
    //{
    //    // Generate uniform names for ambient and intensity
    //    std::string ambientStr = "pointLights[" + std::to_string(i) + "].ambient";
    //    const char* ambient = ambientStr.c_str();

    //    std::string diffuseStr = "pointLights[" + std::to_string(i) + "].diffuse";
    //    const char* diffuse = diffuseStr.c_str();

    //    std::string specularStr = "pointLights[" + std::to_string(i) + "].specular";
    //    const char* specular = specularStr.c_str();

    //    std::string intensityStr = "pointLights[" + std::to_string(i) + "].intensity";
    //    const char* intensity = intensityStr.c_str();

    //    // Generate uniform names for attenuation
    //    std::string constantStr = "pointLights[" + std::to_string(i) + "].constant";
    //    const char* constant = constantStr.c_str();

    //    std::string linearStr = "pointLights[" + std::to_string(i) + "].linear";
    //    const char* linear = linearStr.c_str();

    //    std::string quadraticStr = "pointLights[" + std::to_string(i) + "].quadratic";
    //    const char* quadratic = quadraticStr.c_str();

    //    std::string positionStr = "pointLights[" + std::to_string(i) + "].position";
    //    const char* position = positionStr.c_str();



    //    // Set every point light's ambient to grey and intensity to 2.0f
    //    glUniform3fv(glGetUniformLocation(ourShader.ID, ambient), 1, glm::value_ptr(glm::vec3(0.2f, 0.2f, 0.2f))); // Attenuated ambient
    //    glUniform3fv(glGetUniformLocation(ourShader.ID, diffuse), 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f))); //white color
    //    glUniform3fv(glGetUniformLocation(ourShader.ID, specular), 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f))); //white color 
    //    glUniform1f(glGetUniformLocation(ourShader.ID, intensity), 1.0f);

    //    // Set attenuation properties
    //    glUniform1f(glGetUniformLocation(ourShader.ID, constant), 1.0f); // Constant attenuation
    //    glUniform1f(glGetUniformLocation(ourShader.ID, linear), 0.09f);  // Linear attenuation
    //    glUniform1f(glGetUniformLocation(ourShader.ID, quadratic), 0.032f); // Quadratic attenuation

    //    //position
    //    glUniform3fv(glGetUniformLocation(ourShader.ID, position), 1, glm::value_ptr(pointLightPositions[i])); //

    //}

    // Set every point light's ambient to grey and intensity to 2.0f
    glUniform3fv(glGetUniformLocation(ourShader.ID, "pointLights[0].ambient"), 1, glm::value_ptr(glm::vec3(0.2f, 0.2f, 0.2f))); // Attenuated ambient
    //glUniform3fv(glGetUniformLocation(ourShader.ID, "pointLights[0].diffuse"), 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f))); //white color
    //glUniform3fv(glGetUniformLocation(ourShader.ID, "pointLights[0].specular"), 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f))); //white color 
    glUniform1f(glGetUniformLocation(ourShader.ID, "pointLights[0].intensity"), 2.0f);

    // Set attenuation properties
    glUniform1f(glGetUniformLocation(ourShader.ID, "pointLights[0].constant"), 1.0f); // Constant attenuation
    glUniform1f(glGetUniformLocation(ourShader.ID, "pointLights[0].linear"), 0.09f);  // Linear attenuation
    glUniform1f(glGetUniformLocation(ourShader.ID, "pointLights[0].quadratic"), 0.032f); // Quadratic attenuation

    //position
    //glUniform3fv(glGetUniformLocation(ourShader.ID, "pointLights[0].position"), 1, glm::value_ptr(pointLightPositions[0])); 
    
    //dir light// unused 
    //glUniform3fv(glGetUniformLocation(ourShader.ID, "dirLight.diffuse"), 1, glm::value_ptr(glm::vec3(1.0f))); //color of dirLight
    //glUniform3fv(glGetUniformLocation(ourShader.ID, "dirLight.specular"), 1, glm::value_ptr(glm::vec3(1.0f))); //color of dirLight
    glUniform3fv(glGetUniformLocation(ourShader.ID, "dirLight.direction"), 1, glm::value_ptr(glm::vec3(-1.0f, -1.0f, -1.0f))); 
    glUniform1f(glGetUniformLocation(ourShader.ID, "dirLight.intensity"), 1.0f);

    //flashLight
    glUniform3fv(glGetUniformLocation(ourShader.ID, "spotlight.diffuse"), 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f))); //color is white
    glUniform3fv(glGetUniformLocation(ourShader.ID, "spotlight.specular"), 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f))); //color is white
    glUniform1f(glGetUniformLocation(ourShader.ID, "spotlight.cutOff"), glm::cos(glm::radians(12.0f)));
    glUniform1f(glGetUniformLocation(ourShader.ID, "spotlight.outerCutOff"), glm::cos(glm::radians(17.0f)));
    //intensity is set in the loop



    //diffuse and specular are added in the rendering loop since theyre subject to change.

    
    lightCubeShader.use();
    //glUniform3fv(glGetUniformLocation(lightCubeShader.ID, "lightColor"), 1, glm::value_ptr(lightColor));

    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // render loop
    // -----------

    float frame = 0;

    while (!glfwWindowShouldClose(window))
    {
        frame++;
        // input
        // -----
        processInput(window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        //GL_DEPTH_BUFFER_BIT for z buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Associate woodenCrateTexture to GL_TEXTURE0, therefore associated with material.diffuse. (we set the uniform for material.diffuse to 0, corresponding to GL_TEXTURE0)
        glActiveTexture(GL_TEXTURE0); 
        glBindTexture(GL_TEXTURE_2D, woodenCrateTexture); 

        //same for woodenCrateSpecularMapTexture (specular map)
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, woodenCrateSpecularMapTexture);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, minionTexture);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, matrixTexture);

        //RENDER OBJECTS-----------------------------------------------------------------------------------------------------------------

        ourShader.use();

        float t = glfwGetTime();
        deltaTime = t - lastFrame;
        lastFrame = t;
        glUniform1f(glGetUniformLocation(ourShader.ID, "t"), t); //used for sin wave in shader

        glm::mat4 model      = glm::mat4(1.0f); //moves to world space
        glm::mat4 view       = glm::mat4(1.0f); //moves to view space (camera)
        glm::mat4 projection = glm::mat4(1.0f); //moves to clip space

        //model      = glm::rotate(model, glm::radians(t * 15.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        view       = camera.GetKaisViewMatrix();
        projection = glm::perspective(glm::radians(camera.zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        
        //TEXTURED CUBE WITH COLOR AND NORMALS
        glBindVertexArray(VAO);

        //send the matrix uniforms to vertex shader
        glUniformMatrix4fv(glGetUniformLocation(ourShader.ID, "view"), 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(ourShader.ID, "projection"), 1, GL_FALSE, &projection[0][0]);

        for (int i = 0; i < 1000; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::rotate(model, glm::radians(t * 15.0f), glm::vec3(0.0f, 1.0f, 0.0f));   
            model = glm::translate(model, cubePositions[i]);
            
            glUniformMatrix4fv(glGetUniformLocation(ourShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

            //Draw container
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }



        //useful shading uniforms for the fragment shader
        glUniform3fv(glGetUniformLocation(ourShader.ID, "viewPos"), 1, glm::value_ptr(camera.position));       //world space

        testLightPosition.x = sinf(t * 3.5f);
        testLightPosition.y = 2.0f * cosf(t * 3.5f);

        //POINT LIGHTS
        glUniform3fv(glGetUniformLocation(ourShader.ID, "pointLights[0].position"), 1, glm::value_ptr(testLightPosition));  //world space
        glUniform3fv(glGetUniformLocation(ourShader.ID, "pointLights[0].diffuse"), 1, glm::value_ptr(testLightColor));      //what color does diffuse paint? I choose lightColor
        glUniform3fv(glGetUniformLocation(ourShader.ID, "pointLights[0].specular"), 1, glm::value_ptr(testLightColor));     //what color does specular paint? I choose lightColor
        /*
        glUniform3fv(glGetUniformLocation(ourShader.ID, "pointLights[1].position"), 1, glm::value_ptr(pointLightPositions[1]));  //world space
        glUniform3fv(glGetUniformLocation(ourShader.ID, "pointLights[1].diffuse"), 1, glm::value_ptr(glm::vec3(1.0f)));      //what color does diffuse paint? I choose lightColor
        glUniform3fv(glGetUniformLocation(ourShader.ID, "pointLights[1].specular"), 1, glm::value_ptr(glm::vec3(1.0f)));     //what color does specular paint? I choose lightColor
        */

        //FLASHLIGHT
        if (flashLightOn)
        {
            glUniform1f(glGetUniformLocation(ourShader.ID, "spotlight.intensity"), 1.0f);
        }
        else
        {
            glUniform1f(glGetUniformLocation(ourShader.ID, "spotlight.intensity"), 0.0f);
        }

        glUniform3fv(glGetUniformLocation(ourShader.ID, "spotlight.position"), 1, glm::value_ptr(camera.position));  //world space
        glUniform3fv(glGetUniformLocation(ourShader.ID, "spotlight.direction"), 1, glm::value_ptr(camera.front));  //world space


        glUniform1f(glGetUniformLocation(ourShader.ID, "mixStrength"), mixValue);

        //second box is smaller and movin around
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(sinf(t), 1.0f, -1.0f));
        model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
        model = glm::rotate(model, glm::radians(90.0f * sinf(t * 3.0f)), glm::vec3(1.0f, 1.0f, 1.0f));
        glUniformMatrix4fv(glGetUniformLocation(ourShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model)); 
        //only the model matrix uniform needs to be sent again, camera and perspective are unchanged from og box.

        glDrawArrays(GL_TRIANGLES, 0, 36);

        //third box is smaller and movin around and far away
        model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
        model = glm::translate(model, glm::vec3(cosf(t), 5.0f, -5.0f));
        model = glm::rotate(model, t, glm::vec3(1.0f, 1.0f, 1.0f));
        glUniformMatrix4fv(glGetUniformLocation(ourShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
        //only the model matrix uniform needs to be sent again, camera and perspective are unchanged from og box.

        glDrawArrays(GL_TRIANGLES, 0, 36);

        std::cout << t << '\n';


        //Light cube
        lightCubeShader.use();

        //fun colors light
        #ifdef RAINBOW_LIGHT
            testLightColor = glm::vec3(
                0.5f + 0.5f * sinf(t * 0.7f),
                0.5f + 0.5f * cosf(t * 1.1f),
                0.5f + 0.5f * sinf(t * 1.3f + 1.0f)
            );

        #endif // !rainbow

        glUniform3fv(glGetUniformLocation(lightCubeShader.ID, "lightColor"), 1, glm::value_ptr(testLightColor));

        
        model = glm::mat4(1.0f);
        model = glm::translate(model, testLightPosition);
        model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));

        glUniformMatrix4fv(glGetUniformLocation(lightCubeShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(lightCubeShader.ID, "view"), 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(lightCubeShader.ID, "projection"), 1, GL_FALSE, &projection[0][0]);
        
        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);


        // 
        //for (int i =0; i < 100; i++)
        //{
        //    //Make it small and move it to where the light is, simulating a light bulb
        //    model = glm::mat4(1.0f);
        //    model = glm::translate(model, pointLightPositions[i]);
        //    model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));

        //    //set uniforms in lightCubeShader, most matrices are the same from before, just model is different again.
        //    glUniformMatrix4fv(glGetUniformLocation(lightCubeShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
        //    glUniformMatrix4fv(glGetUniformLocation(lightCubeShader.ID, "view"), 1, GL_FALSE, &view[0][0]);
        //    glUniformMatrix4fv(glGetUniformLocation(lightCubeShader.ID, "projection"), 1, GL_FALSE, &projection[0][0]);

        //    //dont need light pos because its only used here in cpu to create model matrix.

        //    glBindVertexArray(lightVAO);
        //    glDrawArrays(GL_TRIANGLES, 0, 36);
        //}





        /*
        //rotate light around cube
        for (int i = 0; i < 2; i++)
        {
           
            if (i == 0)
            {
                pointLightPositions[i].x = sinf(t * 3.5f);
                pointLightPositions[i].y = 2.0f * cosf(t * 3.5f);
                glUniform3fv(glGetUniformLocation(lightCubeShader.ID, "lightColor"), 1, glm::value_ptr(lightColor));
            }
            else
            {
                pointLightPositions[i].x = 2.0f * sinf(t * 1.5f);
                pointLightPositions[i].z = cosf(t * 1.5f);
                glUniform3fv(glGetUniformLocation(lightCubeShader.ID, "lightColor"), 1, glm::value_ptr(glm::vec3(1.0f)));
            }

            //Make it small and move it to where the light is, simulating a light bulb
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));

            //set uniforms in lightCubeShader, most matrices are the same from before, just model is different again.
            glUniformMatrix4fv(glGetUniformLocation(lightCubeShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
            glUniformMatrix4fv(glGetUniformLocation(lightCubeShader.ID, "view"), 1, GL_FALSE, &view[0][0]);
            glUniformMatrix4fv(glGetUniformLocation(lightCubeShader.ID, "projection"), 1, GL_FALSE, &projection[0][0]);
            
            //dont need light pos because its only used here in cpu to create model matrix.

            glBindVertexArray(lightVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        */

        //FLOOR
        
        ourShader.use();
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -2.6f, 0.0f)); //move it down
        model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f)); //big
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); //move it down
        

        glUniformMatrix4fv(glGetUniformLocation(lightCubeShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

        glBindVertexArray(planeVAO);
        
        //glDrawArrays(GL_TRIANGLES, 0, 6);
        

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        mixValue += 0.001f; // change this value accordingly (might be too slow or too fast based on system hardware)
        if (mixValue >= 1.0f)
            mixValue = 1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        mixValue -= 0.001f; // change this value accordingly (might be too slow or too fast based on system hardware)
        if (mixValue <= 0.0f)
            mixValue = 0.0f;
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) 
    {                                                  
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(UP, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(DOWN, deltaTime);
    }

    //turn on and off flashlight
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
    {
        flashLightOn = !flashLightOn;
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    //avoid jump
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMove((float)xoffset, (float)yoffset);

}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll((float)yoffset);
}


// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

void drawRecursiveTransform(unsigned int transformLoc, glm::vec3 translate, glm::mat4 transform, int depth)
{
    if (depth < 0)
    {
        return;
    }

    //note: translation is affected by scaling. Base translate is -0.5, but scaling is 0.5 so -0.5 * 0.5 = -0.25 down.
    //or you can think: the triangle is now half the size. Originally, the triangle was the size of the screen,
    //so -0.5f would move it down half its height so half of it peeks out at the bottom. Now that its half the size, it moves that much
    //relative to the new size, so also down half its height, which is actually 0.25 down.
    transform = glm::translate(transform, translate);
    transform = glm::scale(transform, glm::vec3(0.5f)); //half the size
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
    glDrawArrays(GL_TRIANGLES, 0, 3);

    depth--;

    //note: translation is affected by scaling. 
    drawRecursiveTransform(transformLoc, glm::vec3(0.0f, 1.5f, 0.0f), transform, depth); //top
    drawRecursiveTransform(transformLoc, glm::vec3(-1.0f, -0.5f, 0.0f), transform, depth); //left
    drawRecursiveTransform(transformLoc, glm::vec3(1.0f, -0.5f, 0.0f), transform, depth); //right

    //the reason its 0.5f is because when you scale down the triangle is effectivelly a half size and 
    // placed directly in the center of this big parent triangle. If you look at the geometrics, 0.5 down and 1 left or right makes sense.
    //1.5 also checks out, move 0.5 up to reach top of parent triangle, then 1.0 to move it out perfectly.


}