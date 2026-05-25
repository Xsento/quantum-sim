#include "point.hpp"

// definitions for static members declared in PointCloud
GLuint PointCloud::VBO = 0;
GLuint PointCloud::VAO = 0;

double associatedLaguerrePolynomial(int l1, int l2, double x){
    // TODO
}

void PointCloud::setupBuffers() {
    float vertices[]{
        0.0f, -0.5f, 0.0f,      // 0
        0.0f, 0.0f, 0.5f,       // 1
        -0.5f, 0.0f, 0.0f,      // 2

        -0.5f, 0.0f, 0.0f,      // 2
        0.0f, 0.0f, 0.5f,       // 1
        0.0f, 0.5f, 0.0f,       // 3
        
        0.0f, 0.5f, 0.0f,       // 3
        0.0f, 0.0f, 0.5f,       // 1
        0.5f, 0.0f, 0.0f,       // 5

        0.5f, 0.0f, 0.0f,       // 5
        0.0f, 0.0f, 0.5f,       // 1
        0.0f, -0.5f, 0.0f,      // 0


        0.0f, -0.5f, 0.0f,      // 0
        0.0f, 0.0f, -0.5f,      // 4
        -0.5f, 0.0f, 0.0f,      // 2

        -0.5f, 0.0f, 0.0f,      // 2
        0.0f, 0.0f, -0.5f,      // 4
        0.0f, 0.5f, 0.0f,       // 3
        
        0.0f, 0.5f, 0.0f,       // 3
        0.0f, 0.0f, -0.5f,      // 4
        0.5f, 0.0f, 0.0f,       // 5

        0.5f, 0.0f, 0.0f,       // 5
        0.0f, 0.0f, -0.5f,      // 4
        0.0f, -0.5f, 0.0f,      // 0
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void PointCloud::draw(glm::mat4 model) {

    for (auto& point : points) {
        if (point.probability < 0.1f)
            continue;
        shaderProgram.use();
        shaderProgram.setVec3("color", POINT_COLOR);
        
        model = glm::mat4(1.0f);
        model = glm::translate(model, point.position);
        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
        shaderProgram.setMat4("model", model);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 24);    
    }

}

PointCloud::PointCloud(Shader& shaderProgram, unsigned int numPoints) : shaderProgram(shaderProgram), numPoints(numPoints) {
    std::mt19937 rng(time(0));
    std::uniform_real_distribution<float> dist(-5.0f, 5.0f);

    while (points.size() < numPoints) {
        Point point;
        point.position = glm::vec3(dist(rng), dist(rng), dist(rng));
        point.probability = 1.0f;

        points.push_back(point);
    }
    
    setupBuffers();
}