#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h> //in shareLIB

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h" //sharelib is a refrence to this project, no need for path
#include "Camera.h"

#include <iostream>
#include <vector>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 0.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

glm::vec3 lightPos = glm::vec3(-2.0f, 4.0f, -1.0f);
glm::vec3 lightCol = glm::vec3(1.0f, 1.0f, 1.0f);
glm::vec3 lightDir = glm::vec3(1.0f, -1.0f, 1.0f);

bool blinn = false;
bool blinnKeyPressed = false;


int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_DEPTH_BITS, 32); // Example using GLFW

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
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwWindowHint(GLFW_SAMPLES, 4);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_STENCIL_TEST);
    //glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    //glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //source, destination
    glEnable(GL_CULL_FACE);
    glEnable(GL_FRAMEBUFFER_SRGB);


    // build and compile shaders
    // -------------------------
    Shader shader("vs.glsl", "fs.glsl");
    Shader lightShader("vs.glsl", "fsLight.glsl");
    Shader depthShader("vsShadow.glsl","fsShadow.glsl");


    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float cubeVertices[] = { // CCW WINDING ORDER
        // Back face
        // Positions          // Normals         // TexCoords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f, // Bottom-left
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f, // Top-right
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f, // Bottom-right         
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f, // Top-right
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f, // Bottom-left
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f, // Top-left

        // Front face
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f, // Bottom-left
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f, // Bottom-right
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f, // Top-right
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f, // Top-right
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f, // Top-left
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f, // Bottom-left

        // Left face
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f, // Top-right
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f, // Top-left
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f, // Bottom-left
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f, // Bottom-left
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f, // Bottom-right
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f, // Top-right

        // Right face
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, // Top-left
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f, // Bottom-right
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, // Top-right         
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f, // Bottom-right
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, // Top-left
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f, // Bottom-left     

         // Bottom face
         -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f, // Top-right
          0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f, // Top-left
          0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f, // Bottom-left
          0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f, // Bottom-left
         -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f, // Bottom-right
         -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f, // Top-right

         // Top face
         -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f, // Top-left
          0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f, // Bottom-right
          0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f, // Top-right     
          0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f, // Bottom-right
         -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f, // Top-left
         -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f  // Bottom-left  
    };
    float planeVertices[] = {
        // positions             // normals          // texture Coords
         15.0f, -0.5f,  15.0f,      0.0f, 1.0f, 0.0f,    2.0f, 0.0f,
        -15.0f, -0.5f,  15.0f,      0.0f, 1.0f, 0.0f,    0.0f, 0.0f,
        -15.0f, -0.5f, -15.0f,      0.0f, 1.0f, 0.0f,    0.0f, 2.0f,

         15.0f, -0.5f,  15.0f,      0.0f, 1.0f, 0.0f,    2.0f, 0.0f,
        -15.0f, -0.5f, -15.0f,      0.0f, 1.0f, 0.0f,    0.0f, 2.0f,
         15.0f, -0.5f, -15.0f,      0.0f, 1.0f, 0.0f,    2.0f, 2.0f
    };
    // cube VAO
    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);

    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glBindVertexArray(0);
    // plane VAO
    unsigned int planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);

    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1); //noramls
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2); 
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    glBindVertexArray(0);
    //grass 
    
    //FRAME BUFFER FOR SHADOWMAP-----------------------------------------------------------------------------
    // ------------------------------------------------------------------------------------------------------
    //Note, the shadowmap is based on the lights pov
    //create the framebuffer
    unsigned int depthMapFB;
    glGenFramebuffers(1, &depthMapFB);

    //Create texture to attach to framebuffer
    unsigned int depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 2048, 2048, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); //DONT LET THE TEXTURE MAP REPEAT
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER); //DONT LET THE TEXTURE MAP REPEAT
    float borderCol[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderCol); // Instead, outside its range should just be 1.0f (furthest away, so no shadow)

    //Attach texture to framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFB);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE); //No need for color buffer
    glReadBuffer(GL_NONE); //No need for color buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // load textures
    // -------------
    unsigned int woodTexture = loadTexture("../ShareLib/Resources/dirt.jpg");

    // shader configuration
    // --------------------
    shader.use();
    //shader.setInt("woodText", 0);
    glUniform1i(glGetUniformLocation(shader.ID, "shadowMap"), 1);

    //POINT LIGHT
    /*
    glUniform3fv(glGetUniformLocation(shader.ID, "pointLight.position"), 1, glm::value_ptr(lightPos));  //world space
    glUniform3fv(glGetUniformLocation(shader.ID, "pointLight.ambient"), 1, glm::value_ptr(glm::vec3(0.025, 0.025, 0.025)));
    glUniform3fv(glGetUniformLocation(shader.ID, "pointLight.diffuse"), 1, glm::value_ptr(lightCol));      //what color does diffuse paint? I choose lightColor
    glUniform3fv(glGetUniformLocation(shader.ID, "pointLight.specular"), 1, glm::value_ptr(lightCol));     //what color does specular paint? I choose lightColor
    glUniform1f(glGetUniformLocation(shader.ID, "pointLight.intensity"), 1.0f);
    glUniform1f(glGetUniformLocation(shader.ID, "pointLight.constant"), 1.0f); // Constant attenuation
    glUniform1f(glGetUniformLocation(shader.ID, "pointLight.linear"), 0.09f);  // Linear attenuation
    glUniform1f(glGetUniformLocation(shader.ID, "pointLight.quadratic"), 0.032f); // Quadratic attenuation
    */
    //DIRECTIONAL LIGHT
    glUniform3fv(glGetUniformLocation(shader.ID, "directionalLight.direction"), 1, glm::value_ptr(lightDir));  //world space
    glUniform3fv(glGetUniformLocation(shader.ID, "directionalLight.ambient"), 1, glm::value_ptr(glm::vec3(0.025, 0.025, 0.025)));
    glUniform3fv(glGetUniformLocation(shader.ID, "directionalLight.diffuse"), 1, glm::value_ptr(lightCol));      //what color does diffuse paint? I choose lightColor
    glUniform3fv(glGetUniformLocation(shader.ID, "directionalLight.specular"), 1, glm::value_ptr(lightCol));     //what color does specular paint? I choose lightColor
    glUniform1f(glGetUniformLocation(shader.ID, "directionalLight.intensity"), 1.0f);
    glUniform1f(glGetUniformLocation(shader.ID, "directionalLight.constant"), 1.0f); // Constant attenuation
    glUniform1f(glGetUniformLocation(shader.ID, "directionalLight.linear"), 0.09f);  // Linear attenuation
    glUniform1f(glGetUniformLocation(shader.ID, "directionalLight.quadratic"), 0.032f); // Quadratic attenuation

    //MATERIAL
    glUniform3fv(glGetUniformLocation(shader.ID, "material.ambient"), 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f))); //the color of the ambient on material
    glUniform1i(glGetUniformLocation(shader.ID, "material.diffuse"), 0); // GL_TEXTURE0, (wood)
    //glUniform1i(glGetUniformLocation(shader.ID, "material.specular"), 1); // GL_TEXTURE1, (wood specular map)
    glUniform1f(glGetUniformLocation(shader.ID, "material.shininess"), 16.0f); //pow!

    lightShader.use();
    glUniform3fv(glGetUniformLocation(lightShader.ID, "lightColor"), 1, glm::value_ptr(lightCol));


    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.00, 0.0f, 0.0f, 0.4f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        //move the light around
        lightDir.x = 1.0f * cosf(currentFrame * 0.25f);
        lightPos = -lightDir * 3.0f;

        //FIRST PASS: RENDER DEPTHMAP------------------------------------------------------------------------------------
        //--------------------------------------------------------------------------------------------------------------
        depthShader.use();

        float nearPlane = 1.0f;
        float farPlane = 8.5f;
        glm::mat4 lightProjectionMatrix = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, nearPlane, farPlane);
        glm::mat4 lightViewMatrix = glm::lookAt(glm::vec3(lightPos), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 lightSpaceMatrix = lightProjectionMatrix * lightViewMatrix; //Transforms a fragment to the pov of the directional light
        glUniformMatrix4fv(glGetUniformLocation(depthShader.ID, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

        glViewport(0, 0, 2048, 2048); //make sure the window rectangle is the shadowmap size
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFB); //Note, the shadowmap is based on the lights pov
        glClear(GL_DEPTH_BUFFER_BIT); //clear the depth buffer
        //glCullFace(GL_FRONT);

        //At this point, the depth map will be filled with the closest values from the lights pov. (After rendering)

        //RENDER SCENE------------------------------------------------------------------------------------------------------------
        //PLANE----------------------------------------------------
        glDisable(GL_CULL_FACE); //Cant cull non-closed shapes like plane
        glBindVertexArray(planeVAO);

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        glUniformMatrix4fv(glGetUniformLocation(depthShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
       // glUniformMatrix4fv(glGetUniformLocation(depthShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
       // glUniformMatrix4fv(glGetUniformLocation(depthShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        //CUBES----------------------------------------------------
        glEnable(GL_CULL_FACE);
        glBindVertexArray(cubeVAO);

        //Cube 1
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-2.0f, 0.0f, -2.0f));
        model = glm::rotate(model, glm::radians(20.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(depthShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //Cube 2
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, (1.0f + sinf(currentFrame)) / 2.0f, 0.0f));
        model = glm::rotate(model, glm::radians(currentFrame * 20.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(depthShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        //END CUBES-------------------------------------------------

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        //glCullFace(GL_BACK);

        //END RENDER SCENE-------------------------------------------------------------------------------------------
        //-----------------------------------------------------------------------------------------------------------


        //------------------------------------------------------------------------------------------------------------
        //SECOND PASS: RENDER SCENE NORMALY---------------------------------------------------------------------------
        //---------------------------------------------------------------------------------------------------------
        shader.use();

        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, woodTexture); //We set the uniform for material.diffuse to id 0
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthMap);    //We set the uniform for depthMap to id 1
        
        //Send important stuffs to shader...
        glUniform3fv(glGetUniformLocation(shader.ID, "viewPos"), 1, glm::value_ptr(camera.position));
        glUniform1i(glGetUniformLocation(shader.ID, "blinn"), blinn);
        if (blinn) glUniform1f(glGetUniformLocation(shader.ID, "material.shininess"), 64.0f); //pow!
        else glUniform1f(glGetUniformLocation(shader.ID, "material.shininess"), 32.0f);
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix)); //for comparison to depthMap
        glUniform3fv(glGetUniformLocation(shader.ID, "directionalLight.direction"), 1, glm::value_ptr(lightDir));  //world space
        

        //RENDER SCENE------------------------------------------------------------------------------------------------------------
        //PLANE----------------------------------------------------
        glDisable(GL_CULL_FACE); //Cant cull non-closed shapes like plane
        glBindVertexArray(planeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, woodTexture);

        model = glm::mat4(1.0f);
        view = camera.GetViewMatrix();
        projection = glm::perspective(glm::radians(camera.zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        //CUBES----------------------------------------------------
        glEnable(GL_CULL_FACE);
        glBindVertexArray(cubeVAO);

        //Cube 1
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-2.0f, 0.0f, -2.0f));
        model = glm::rotate(model, glm::radians(20.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //Cube 2
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, (1.0f + sinf(currentFrame)) / 2.0f, 0.0f));
        model = glm::rotate(model, glm::radians(currentFrame * 20.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        //----------------------------------------------------------
        //END CUBES-------------------------------------------------
        // 
        //LIGHT (debug)---------------------------------------------------------------------------------------------------
        lightShader.use();
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(lightPos));
        model = glm::scale(model, glm::vec3(0.2f,0.2f, 0.2f));
        glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        glBindVertexArray(0);
        //END LIGHT-------------------------------------------------------------------------------------------------------

        //END SECOND PASS---------------------------------------------------------------------------------------------------
        //------------------------------------------------------------------------------------------------------------------

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &planeVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &planeVBO);

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(UP, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(DOWN, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && !blinnKeyPressed)
    {
        blinn = !blinn;
        blinnKeyPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE)
    {
        blinnKeyPressed = false;
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

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMove(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
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
        GLenum internalFormat;
        if (nrComponents == 1)
        {
            format = GL_RED;
            internalFormat = GL_RED;
        }
        else if (nrComponents == 3)
        {
            format = GL_SRGB;
            internalFormat = GL_RGB;
        }
            
        else if (nrComponents == 4)
        {
            format = GL_RGBA;
            internalFormat = GL_SRGB_ALPHA;
        }
            


        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, internalFormat, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        if (format == GL_RGBA)
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }
        else
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        }

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

//Note for sharelib: originally you have to have the folder glad/glad.c in the same directory as main. We move that to
//shareLib effectively simulating that but not having to copy the folder to every project. Saem goes for stb_image and custom headers like Shader or Mesh etc.