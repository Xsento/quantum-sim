#include "point.hpp"

// definitions for static members declared in PointCloud
GLuint PointCloud::VBO = 0;
GLuint PointCloud::VAO = 0;

// Bohr radius is set to 1 for simplicity and skipped in the calculations
double normalizedRadialFunction(int n, int l, double x){
    switch (n){
        case 1:
            if (l == 0) return 2*exp(-x);
            break;
        case 2:
            if (l == 0) return (1/sqrt(2))*(1-x/2)*exp(-x/2);
            if (l == 1) return (1/sqrt(24))*x*exp(-x/2);
            break;
        case 3:
            if (l == 0) return (2/sqrt(27))*(1 - 2*x/3 + 2*x*x/27)*exp(-x/3); 
            if (l == 1) return (8/(27*sqrt(6)))*(1-x/6)*x*exp(-x/3);
            if (l == 2) return (4/(81*sqrt(30)))*x*x*exp(-x/3);
            break;
    }
    std::cout << "Radial function parameter error" << std::endl;
    exit(1);
    return 0;
}

double associatedLegendrePolynomial(int l, int m, double x){
    switch (l) {
        case 0:
            return 1.0;
        case 1:
            if (m == 0) return x;
            if (m == 1) return -sqrt(1 - x*x);
            break;
        case 2:
            if (m == 0) return 0.5 * (3*x*x - 1);
            if (m == 1) return -3*x*sqrt(1 - x*x);
            if (m == 2) return 3*(1 - x*x);
            break;
    }
    std::cout << "Legendre polynomial parameter error" << std::endl;
    exit(2);
    return 0;
}

int factorial (int n){
    if (n>0){
        return n * factorial(n-1);
    }
    else return 1;
}

std::complex<double> sphericalHarmonic(int l, int m, double theta, double phi){
    double realPart = ((-1)^m) * sqrt((2*l + 1)/(4*M_PI) * factorial(l-m) / factorial(l+m)) * associatedLegendrePolynomial(l,m,cos(theta));
    std::complex<double> exponential =(sin(m*phi),cos(m*phi));

    return realPart * exponential;
}

void PointCloud::calculateProbability(int n, int l, int m, Point& point) {
    double r = sqrt(point.position.x*point.position.x + point.position.z*point.position.z + point.position.y*point.position.y);
    double theta = acos(point.position.y/r);
    double phi = atan(point.position.z/point.position.x);

    std::complex<double> waveFunctionValue;
    std::complex<double> waveFunctionValueConjugate;
    if (m<0) m = -m;
    waveFunctionValue = normalizedRadialFunction(n,l,r)*sphericalHarmonic(l,m,theta,phi);
    //waveFunctionValueConjugate = conj(waveFunctionValue);

    point.probability = abs(waveFunctionValue)*abs(waveFunctionValue);
}

void PointCloud::calculateAllProbabilities(){
    std::mt19937 rng(time(0));

    double maxProb = 0.0;

    for (auto& point : points){
        calculateProbability(n,l,m,point);
        //std::cout << point.position.x << " " << point.position.y << " " << point.position.z << "|" << point.probability << std::endl;
        maxProb = std::max(maxProb,point.probability);
    }

    std::uniform_real_distribution<double> dist(0.0, maxProb);

    for (auto& point : points){
        double target = dist(rng);
        if (point.probability < target) point.probability = -1;
    }
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
    std::mt19937 rng(time(0));
    std::uniform_real_distribution<float> dist(0.f, 1.f);

    for (auto& point : points) {
        if (point.probability <= 0) continue;
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

PointCloud::PointCloud(Shader& shaderProgram, unsigned int numPoints, int n, int l, int m) : shaderProgram(shaderProgram), numPoints(numPoints), n(n), l(l), m(m) {
    std::mt19937 rng(time(0));
    std::uniform_real_distribution<double> dist(-(6.0*n), (6.0*n));

    while (points.size() < numPoints) {
        Point point;
        point.position = glm::vec3(dist(rng), dist(rng), dist(rng));
        //point.probability = 1.0f;

        points.push_back(point);
    }

    //std::cout << factorial(5) << std::endl;
    
    setupBuffers();
}