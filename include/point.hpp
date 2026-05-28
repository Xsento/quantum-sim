#include "main.hpp"
#include "shader.h"

class PointCloud {
    struct Point {
        glm::vec3 position;     // [x, y, z]
        double probability;       // probability
    };
    std::vector<Point> points;

    static constexpr glm::vec3 POINT_COLOR = glm::vec3(0.2f, 0.7f, 1.0f); // light blue

    static constexpr glm::vec3 COLOR_MIN = glm::vec3(0.2f, 0.f, 0.4f);  // dark purple
    static constexpr glm::vec3 COLOR_MAX = glm::vec3(1.f, 0.5f, 0.f);   // orange
    static constexpr glm::vec3 COLOR_WHITE = glm::vec3(1.f, 1.f, 1.f);   // white

    static GLuint VBO, VAO;
    static void setupBuffers();

    double maxProb = 0.0;

    Shader shaderProgram;
    unsigned int numPoints;

    int n, l, m;
    
public:
    PointCloud(Shader& shaderProgram, unsigned int numPoints, int n, int l, int m);
    
    void calculateProbability(int n, int l, int m, Point& point);  // for multithreading purposes
    void calculateAllProbabilities();
    void draw(glm::mat4 model);
};
