#include "main.hpp"
#include "point.hpp"
#include "shader.h"

// definitions for globals declared in main.hpp
GLfloat cameraAnglePhi = 0.f; // in degrees
GLfloat cameraAngleTheta = 90.f; // in degrees
GLfloat camedaDistance = 25.f;

const GLfloat CAMERA_ROTATION_SPEED = 9000000.f;
const GLfloat CAMERA_ZOOM_SPEED = 3000000.f;

int main()
{
    // glfw: initialize and configure
    std::cout << "Initializing GLFW..." << std::endl;
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    std::cout << "GLFW initialized properly" << std::endl;

    // glfw window creation
    glfwWindowHint(GLFW_MAXIMIZED , GL_TRUE);
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Please wait...", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // build and compile the shader program
    std::cout << "Initializing shader program..." << std::endl;
    Shader shaderProgram("../assets/shaders/vertex.vs", "../assets/shaders/fragment.fs");
    std::cout << "Shader program initialized" << std::endl;

    //alpha blend
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

    // depth test and point size
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_PROGRAM_POINT_SIZE);

    // SIMULATION PARAMETERS
    // --------------------------------------------------------------------------------
    const int n = 3;
    const int l = 2;
    const int m = 0;
    double start = glfwGetTime();
    std::cout << "Generating point array... " << std::endl;
    PointCloud pointCloud(shaderProgram, 6000000, n, l, m);
    std::cout << "Point generation time: " << glfwGetTime()-start << "s" << std::endl;

    start = glfwGetTime();
    std::cout << "Calculating probabilities... " << std::endl;
    pointCloud.calculateAllProbabilities();
    std::cout << "Probability calculation time: " << glfwGetTime()-start << "s" << std::endl;
    // --------------------------------------------------------------------------------

    double lastTime = glfwGetTime();
    int frames = 0;

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // Measure speed
        double currentTime = glfwGetTime();
        frames++;
        if ( currentTime - lastTime >= 1.0 ){
            glfwSetWindowTitle(window, (std::string("FPS: ") + std::to_string(frames)).c_str()); 
            frames = 0;
            lastTime += 1.0;
        }
        // input    
        processInput(window);

        // render
        glClearColor(0.f, 0.f, 0.f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaderProgram.use();

        // create transformations
        glm::mat4 model         = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        glm::mat4 view          = glm::mat4(1.0f);
        glm::mat4 projection    = glm::mat4(1.0f);
        //view  = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        
        GLfloat thetaRad = glm::radians(cameraAngleTheta);
        GLfloat phiRad = glm::radians(cameraAnglePhi);

        glm::vec3 cameraPos = glm::vec3(camedaDistance * sin(thetaRad) * cos(phiRad),      
                                  camedaDistance * cos(thetaRad),                    
                                  camedaDistance * sin(thetaRad) * sin(phiRad));        
                                  
        glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);

        cameraPos += target; // move the camera to the position relative to the target

        view = glm::lookAt(
            cameraPos,
            target,                               // what the camera is lookin at
            glm::vec3(0.0f, 1.0f, 0.0f)
        );

        // prevent distortion by setting the aspect ratio based on the framebuffer size, not the window size
        int currentWidth = SCR_WIDTH;
        int currentHeight = SCR_HEIGHT;
        glfwGetFramebufferSize(window, &currentWidth, &currentHeight);
        projection = glm::perspective(glm::radians(45.0f), static_cast<float>(currentWidth) / static_cast<float>(currentHeight), 0.1f, 100.0f);
        
        shaderProgram.setMat4("view", view);        
        shaderProgram.setMat4("projection", projection);

        pointCloud.draw(model);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
 
    std::cout << "Window closed. Exiting..." << std::endl;
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window)
{
    auto lastTime = glfwGetTime();
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        cameraAngleTheta -= CAMERA_ROTATION_SPEED*(glfwGetTime() - lastTime);
        if (cameraAngleTheta <= 0)
            cameraAngleTheta = 0.1f;
        //std::cout << "Camera angle Y: " << cameraAngleTheta << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        cameraAngleTheta += CAMERA_ROTATION_SPEED*(glfwGetTime() - lastTime);
        if (cameraAngleTheta >= 180)
            cameraAngleTheta = 179.9f;
        //std::cout << "Camera angle Y: " << cameraAngleTheta << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        cameraAnglePhi -= CAMERA_ROTATION_SPEED*(glfwGetTime() - lastTime);
        if (cameraAnglePhi <= 0)
            cameraAnglePhi += 360;
        //std::cout << "Camera angle X: " << cameraAnglePhi << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        cameraAnglePhi += CAMERA_ROTATION_SPEED*(glfwGetTime() - lastTime);
        if (cameraAnglePhi >= 360)
            cameraAnglePhi -= 360;
        //std::cout << "Camera angle X: " << cameraAnglePhi << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS){
        camedaDistance -= CAMERA_ZOOM_SPEED*(glfwGetTime() - lastTime);
        if (camedaDistance <= 10.f)
            camedaDistance = 10.f;
        //std::cout << "Camera distance: " << camedaDistance << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS){
        camedaDistance += CAMERA_ZOOM_SPEED*(glfwGetTime() - lastTime);
        //std::cout << "Camera distance: " << camedaDistance << std::endl;
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

