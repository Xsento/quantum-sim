#pragma once

#include "main.hpp"
#include "shader.h"

class PointCloud {
    struct Point {
        glm::vec3 position;         // [x, y, z]
        double probability;         // probability of finding an electron in a given point arising from the wave function
    };

    std::vector<std::vector<Point>> pointMasterVector;  // master vector containing all points, divided into subvectors for multithreading, each subvector is processed by a separate thread to avoid thread racing

    //static constexpr glm::vec3 POINT_COLOR = glm::vec3(0.2f, 0.7f, 1.0f);   // light blue 

    static constexpr glm::vec3 COLOR_MIN = glm::vec3(0.2f, 0.f, 0.4f);      // dark purple
    static constexpr glm::vec3 COLOR_MAX = glm::vec3(1.f, 0.5f, 0.f);       // orange
    static constexpr glm::vec3 COLOR_WHITE = glm::vec3(1.f, 1.f, 1.f);      // white

    static GLuint VBO, VAO;         // OpenGL buffer objects

    double maxProb = 0.0;           // global maximum probability, used for normalization and sampling

    Shader shaderProgram;
    unsigned int numPoints;         // number of points to generate, also the number of points left after rejection sampling, used for rendering
    unsigned short int maxThreads;  // maximum number of thread determined by std::hardware_concurrency, used for multithreading
    int pointsPerThread;            // integer division numPoints/maxThreads

    int n, l, m;                    // quantum numbers

    double range;                   // dimensions of the cube in which the points are generated (from -range to range in each axis)
    
public:
    PointCloud(Shader& shaderProgram, unsigned int numPoints, int n, int l, int m);
    
    void calculateProbability(Point& point);                                            // calculates probability for a point
    
    void calculateAllProbabilities();                                                   // calculates probabilities for all points
    
    void draw();                                                                        // renders the points using OpenGL, assumes the shader program is already in use and has the necessary uniforms set up
    
    void generatePointVector(double range, unsigned int seed,                           // generates a vector of points with random positions in a given range and adds it to the point master vector
         unsigned int amount, int threadId);                                            

    void probabilityWorker(int threadId, double& outLocalMax);                          // calculates probabilities for a subset of points and returns the local maximum probability
    
    void filterWorker(int threadId, double globalMaxProb,                               // filters a subset of points, rejecting points with probability lower than a random 
         unsigned int& outRejectedCount, unsigned int seed);                            // threshold based on the global maximum probability, and returns the count of rejected points

    void setupBuffers();                                                                // sets up the OpenGL buffers for rendering points, frees RAM by removing point vectors
};
