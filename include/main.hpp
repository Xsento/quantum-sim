#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>
#include <random>
#include <ctime>
#include <complex>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// settings
constexpr unsigned int SCR_WIDTH = 800;
constexpr unsigned int SCR_HEIGHT = 600;

// camera parameters (defined in one .cpp file)
extern GLfloat cameraAnglePhi; // in degrees
extern GLfloat cameraAngleTheta; // in degrees
extern GLfloat camedaDistance;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
