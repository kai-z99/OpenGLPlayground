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
unsigned int loadCubemap(std::vector<std::string> faces);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;


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
    glDepthFunc(GL_LEQUAL);


    // build and compile shaders
    // -------------------------
    Shader shader("vs1.glsl", "fs1.glsl");
    Shader skyShader("vsCube.glsl", "fsCube.glsl");


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


    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    float hexBipyramidVertices[] = {
        // --- Top half (apex T = (0,0,+0.5)) ---

        // Face 0 (v0, v1, T)
        0.5f,  0.0f,  0.0f,    0.6547f,  0.3780f,  0.6547f,
        0.25f, 0.4330f,0.0f,    0.6547f,  0.3780f,  0.6547f,
        0.0f,  0.0f,  0.5f,     0.6547f,  0.3780f,  0.6547f,

        // Face 1 (v1, v2, T)
        0.25f,  0.4330f, 0.0f,   0.0f,     0.7560f,  0.6547f,
       -0.25f,  0.4330f, 0.0f,   0.0f,     0.7560f,  0.6547f,
        0.0f,   0.0f,    0.5f,   0.0f,     0.7560f,  0.6547f,

        // Face 2 (v2, v3, T)
       -0.25f,  0.4330f, 0.0f,  -0.6547f,  0.3780f,  0.6547f,
       -0.5f,   0.0f,    0.0f,  -0.6547f,  0.3780f,  0.6547f,
        0.0f,    0.0f,   0.5f,  -0.6547f,  0.3780f,  0.6547f,

        // Face 3 (v3, v4, T)
       -0.5f,   0.0f,    0.0f,  -0.6547f, -0.3780f,  0.6547f,
       -0.25f, -0.4330f, 0.0f,  -0.6547f, -0.3780f,  0.6547f,
        0.0f,   0.0f,    0.5f,  -0.6547f, -0.3780f,  0.6547f,

        // Face 4 (v4, v5, T)
       -0.25f, -0.4330f, 0.0f,   0.0f,    -0.7560f,  0.6547f,
        0.25f, -0.4330f, 0.0f,   0.0f,    -0.7560f,  0.6547f,
        0.0f,   0.0f,    0.5f,   0.0f,    -0.7560f,  0.6547f,

        // Face 5 (v5, v0, T)
        0.25f, -0.4330f, 0.0f,   0.6547f, -0.3780f,  0.6547f,
        0.5f,   0.0f,    0.0f,   0.6547f, -0.3780f,  0.6547f,
        0.0f,   0.0f,    0.5f,   0.6547f, -0.3780f,  0.6547f,

        // --- Bottom half (apex B = (0,0,-0.5)) ---

        // Face 0 (v0, B, v1)
        0.5f,   0.0f,    0.0f,   0.6547f,  0.3780f, -0.6547f,
        0.0f,   0.0f,   -0.5f,   0.6547f,  0.3780f, -0.6547f,
        0.25f,  0.4330f, 0.0f,   0.6547f,  0.3780f, -0.6547f,

        // Face 1 (v1, B, v2)
        0.25f,  0.4330f, 0.0f,   0.0f,     0.7560f, -0.6547f,
        0.0f,   0.0f,   -0.5f,   0.0f,     0.7560f, -0.6547f,
       -0.25f,  0.4330f, 0.0f,   0.0f,     0.7560f, -0.6547f,

       // Face 2 (v2, B, v3)
          -0.25f,  0.4330f, 0.0f,  -0.6547f,  0.3780f, -0.6547f,
           0.0f,    0.0f,  -0.5f,  -0.6547f,  0.3780f, -0.6547f,
          -0.5f,   0.0f,   0.0f,   -0.6547f,  0.3780f, -0.6547f,

          // Face 3 (v3, B, v4)
         -0.5f,   0.0f,    0.0f,  -0.6547f, -0.3780f, -0.6547f,
          0.0f,    0.0f,   -0.5f, -0.6547f, -0.3780f, -0.6547f,
         -0.25f, -0.4330f, 0.0f,  -0.6547f, -0.3780f, -0.6547f,

         // Face 4 (v4, B, v5)
        -0.25f, -0.4330f, 0.0f,   0.0f,    -0.7560f, -0.6547f,
         0.0f,   0.0f,   -0.5f,   0.0f,    -0.7560f, -0.6547f,
         0.25f, -0.4330f, 0.0f,   0.0f,    -0.7560f, -0.6547f,

         // Face 5 (v5, B, v0)
         0.25f, -0.4330f, 0.0f,   0.6547f, -0.3780f, -0.6547f,
         0.0f,   0.0f,   -0.5f,   0.6547f, -0.3780f, -0.6547f,
         0.5f,   0.0f,    0.0f,   0.6547f, -0.3780f, -0.6547f
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

    // skybox VAO
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);

    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);

    // load textures
    // -------------
    unsigned int cubeTexture = loadTexture("../ShareLib/Resources/container2.png");
    unsigned int woodenCrateSpecularMapTexture = loadTexture("../ShareLib/Resources/container2_specular.png");

    //skybox
    
    
    stbi_set_flip_vertically_on_load(false);
    /*
    std::vector<std::string> faces =
    {
        "../ShareLib/Resources/yellowCloudSkybox/yellowcloud_rt.jpg",
        "../ShareLib/Resources/yellowCloudSkybox/yellowcloud_lf.jpg",
        
        
        "../ShareLib/Resources/yellowCloudSkybox/yellowcloud_up.jpg",
        "../ShareLib/Resources/yellowCloudSkybox/yellowcloud_dn.jpg",
        
        
        "../ShareLib/Resources/yellowCloudSkybox/yellowcloud_ft.jpg",
        "../ShareLib/Resources/yellowCloudSkybox/yellowcloud_bk.jpg", 
        
    };
    */
    
    
    
    std::vector<std::string> faces =
    {
        "../ShareLib/Resources/skybox/right.jpg",
        "../ShareLib/Resources/skybox/left.jpg",
        "../ShareLib/Resources/skybox/top.jpg",
        "../ShareLib/Resources/skybox/bottom.jpg",
        "../ShareLib/Resources/skybox/front.jpg",
        "../ShareLib/Resources/skybox/back.jpg"
    };
    
    unsigned int cubeMapTexture = loadCubemap(faces);
    shader.use();
    shader.setInt("skybox", 0);
    shader.setInt("woodenTexture", 1);
    shader.setInt("woodenSpecularMap", 2);
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
        glClearColor(0.71, 0.9f, 0.949f, 0.4f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);


        //CUBE----------------------------------------------------
        shader.use();
        glEnable(GL_CULL_FACE);
        

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, sinf(currentFrame), 0.0f));
        model = glm::rotate(model, glm::radians(currentFrame * 25.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniform3fv(glGetUniformLocation(shader.ID, "cameraPos"), 1, glm::value_ptr(camera.position) );

        glBindVertexArray(cubeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, cubeTexture);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, woodenCrateSpecularMapTexture);

        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        //----------------------------------------------------------
        //SKYBOX--------------------------------------------------
        skyShader.use();
        glDisable(GL_CULL_FACE);
        glDepthFunc(GL_LEQUAL);

        model = glm::mat4(1.0f);
        //model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
        view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
        projection = glm::perspective(glm::radians(camera.zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        glUniformMatrix4fv(glGetUniformLocation(skyShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(skyShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(skyShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glDepthFunc(GL_LESS);
        glBindVertexArray(0);



        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);

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
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;


        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
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

unsigned int loadCubemap(std::vector<std::string> faces)
{
    unsigned int textureID; 
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrComponents;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

//Note for sharelib: originally you have to have the folder glad/glad.c in the same directory as main. We move that to
//shareLib effectively simulating that but not having to copy the folder to every project. Saem goes for stb_image and custom headers like Shader or Mesh etc.