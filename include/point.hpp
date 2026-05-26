#include "main.hpp"
#include "shader.h"

class PointCloud {
    struct Point {
        glm::vec3 position;     // [x, y, z]
        double probability;       // probability
    };
    std::vector<Point> points;

    static constexpr glm::vec3 POINT_COLOR = glm::vec3(0.2f, 0.7f, 1.0f); // light blue

    static GLuint VBO, VAO;
    static void setupBuffers();

    Shader shaderProgram;
    unsigned int numPoints;

    int n, l, m;
    
public:
    PointCloud(Shader& shaderProgram, unsigned int numPoints, int n, int l, int m);
    
    void calculateProbability(int n, int l, int m, Point& point);  // for multithreading purposes
    void calculateAllProbabilities();
    void draw(glm::mat4 model);
};
