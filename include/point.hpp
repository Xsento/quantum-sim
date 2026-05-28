#include "main.hpp"
#include "shader.h"

class PointCloud {
    struct Point {
        glm::vec3 position;         // [x, y, z]
        double probability;         // probability of finding an electron in a given point arising from the wave function
    };
    std::vector<std::vector<Point>> pointMasterVector;

    //static constexpr glm::vec3 POINT_COLOR = glm::vec3(0.2f, 0.7f, 1.0f);   // light blue 

    static constexpr glm::vec3 COLOR_MIN = glm::vec3(0.2f, 0.f, 0.4f);      // dark purple
    static constexpr glm::vec3 COLOR_MAX = glm::vec3(1.f, 0.5f, 0.f);       // orange
    static constexpr glm::vec3 COLOR_WHITE = glm::vec3(1.f, 1.f, 1.f);      // white

    static GLuint VBO, VAO;
    static void setupBuffers();     // setup VBO and VAO

    double maxProb = 0.0;

    Shader shaderProgram;
    unsigned int numPoints;
    unsigned short int maxThreads;
    int pointsPerThread;

    int n, l, m;
    
public:
    PointCloud(Shader& shaderProgram, unsigned int numPoints, int n, int l, int m);
    
    void calculateProbability(Point& point);                                            // calculates probability for a point
    
    void calculateAllProbabilities();                                                   // calculates probabilities for all points
    
    void draw(glm::mat4 model);                                                         // calculates model matrices and draws each point
    
    void generatePointVector(double range, unsigned int seed,                      
         unsigned int amount, int threadId);                                            // generates a vector of randomly generated points and adds it to the point vectors vector
    
    void probabilityWorker(int threadId, double& outLocalMax);                          // calculates probabilities for a subset of points and returns the local maximum probability
    
    void filterWorker(int threadId, double globalMaxProb,                               // filters a subset of points, rejecting points with probability lower than a random 
         unsigned int& outRejectedCount, unsigned int seed);                            // threshold based on the global maximum probability, and returns the count of rejected points
};
