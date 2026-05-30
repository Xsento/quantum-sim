#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>
#include <random>
#include <ctime>
#include <cmath>
#include <complex>
#include <thread>
#include <functional>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

// settings
constexpr int SCR_WIDTH = 800;
constexpr int SCR_HEIGHT = 600;

// camera parameters (defined in one .cpp file)
extern GLfloat cameraAnglePhi; // in degrees
extern GLfloat cameraAngleTheta; // in degrees
extern GLfloat cameraDistance;

// callback for window resizing, also called once at the beginning to set the viewport
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

// process keyboard input
void processInput(GLFWwindow *window);
