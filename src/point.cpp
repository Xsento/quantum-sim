#include "point.hpp"

// OPENGL RENDERING
// --------------------------------------------------------------------------------

// definitions for static members declared in PointCloud
GLuint PointCloud::VBO = 0;
GLuint PointCloud::VAO = 0;

// i love how well it works
void PointCloud::setupBuffers() {
    // point
    float* vertices = new float[numPoints * 4];

    // i dont think we need multithreading here, there wont be many points left after the rejection sampling and it would be a nightmare to set up
    int i = 0;
    for (auto& pointSubvector : pointMasterVector) {
        for (auto& point : pointSubvector) {
            vertices[i * 4 + 0] = point.position.x/range;   // normalize the position because opengl accepts only between -1 and 1
            vertices[i * 4 + 1] = point.position.y/range;
            vertices[i * 4 + 2] = point.position.z/range;
            vertices[i * 4 + 3] = point.probability;        // normalization happens in the shader
            i++;
        }
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 4*numPoints*sizeof(float), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(3 * sizeof(float))); // probability attribute
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    delete[] vertices;
    pointMasterVector.clear(); // free memory, we dont need the points anymore after uploading them to the GPU
    pointMasterVector.shrink_to_fit();
}

void PointCloud::draw() {
    //glBindVertexArray(VAO);               // setupBuffers binds the VAO, and we never unbind it, so this is not necessary

    glDrawArrays(GL_POINTS, 0, numPoints);  // beautiful 
}
// --------------------------------------------------------------------------------


// PHYSICS CALCULATIONS
// --------------------------------------------------------------------------------

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
// --------------------------------------------------------------------------------

PointCloud::PointCloud(Shader& shaderProgram, unsigned int numPoints, int n, int l, int m) : shaderProgram(shaderProgram), numPoints(numPoints), n(n), l(l), m(m) {
    switch (n){
        case 1:
            range = 5.0;
            break;
        case 2:
            range = 15.0;
            break;
        case 3:
            range = 25.0;
            break;
    }

    maxThreads = std::thread::hardware_concurrency();
    pointMasterVector.resize(maxThreads);

    // divide the task, create and handle threads
    pointsPerThread = numPoints/maxThreads;
    std::vector<std::thread> threads; 
    for (int i = 0; i < maxThreads; i++){
        threads.emplace_back(&PointCloud::generatePointVector, this, static_cast<double>(range), static_cast<unsigned int>(time(0) + i), static_cast<unsigned int>(pointsPerThread), static_cast<int>(i));
    }
    for (auto& t : threads){
        t.join();
    }

    //shaderProgram.use();                          // I'd prefer its turned on in main
    shaderProgram.setVec3("colorMin", COLOR_MIN);
    shaderProgram.setVec3("colorMax", COLOR_MAX);
    shaderProgram.setVec3("white", COLOR_WHITE);
}

void PointCloud::generatePointVector(double range, unsigned int seed, unsigned int amount, int threadId){
    std::mt19937 rng(seed);             // seed is unsigned int to avoid precision issues with float seeds
    std::vector<Point> pointVector;
    std::uniform_real_distribution<double> dist(-range, range); // set rng range
    for (int i = 0; i < amount; ++i){
        Point point;
        point.position = glm::vec3(dist(rng), dist(rng), dist(rng));    // randomize position
        pointVector.push_back(point);        
    }
    pointMasterVector[threadId] = pointVector;
}

void PointCloud::calculateProbability(Point& point) {
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

void PointCloud::probabilityWorker(int threadId, double& outLocalMax) {
    double localMax = 0.0;
    for (auto& point : pointMasterVector[threadId]) {
        calculateProbability(point);
        if (point.probability > localMax) {
            localMax = point.probability;
        }
    }
    outLocalMax = localMax; 
}

void PointCloud::filterWorker(int threadId, double globalMaxProb, unsigned int& outRejectedCount, unsigned int seed) {
    std::mt19937 rng(seed);                                             // seed is unsigned int to avoid precision issues with float seeds
    std::uniform_real_distribution<double> dist(0.0, globalMaxProb);    // set rng range

    outRejectedCount = static_cast<unsigned int>(std::erase_if(pointMasterVector[threadId], [&](Point& point) {
        double target = dist(rng);
        return point.probability < target;
    }));
}

void PointCloud::calculateAllProbabilities() {;
    maxProb = 0.0;

    std::vector<std::thread> calcThreads;
    std::vector<double> localMaxes(maxThreads, 0.0);

    unsigned int startingPoints = maxThreads * pointMasterVector[0].size();

    // multithreaded probability calculation and local max finding
    for (int i = 0; i < maxThreads; ++i) {
        calcThreads.emplace_back(&PointCloud::probabilityWorker, this, i, std::ref(localMaxes[i]));
    }
    for (auto& t : calcThreads) t.join();

    // sum up local maxes to find the global maximum probability
    for (double m : localMaxes) {
        maxProb = std::max(maxProb, m);
    }

    std::vector<std::thread> filterThreads;
    std::vector<unsigned int> localRejected(maxThreads, 0);
    localRejected.resize(maxThreads);

    // multithreaded rejection sampling of points
    for (int i = 0; i < maxThreads; ++i) {
        filterThreads.emplace_back(&PointCloud::filterWorker, this, i, maxProb, std::ref(localRejected[i]), static_cast<unsigned int>(time(0) + i));
    }
    for (auto& t : filterThreads) t.join();

    // sum up rejected points and print stats
    unsigned int pointsRejected = 0;
    for (unsigned int r : localRejected){
        // std::cout << r << " points rejected by thread." << std::endl;
        pointsRejected += r;
    }

    numPoints -= pointsRejected;

    shaderProgram.setFloat("maxProb", maxProb);

    std::cout << "Starting points: " << startingPoints << " Points drawn: " << numPoints << " Rejection rate: " << 100.f * static_cast<float>(pointsRejected) / startingPoints << "%" << std::endl;
}
