#include <vector>
#include <glm.hpp>

static const std::vector<glm::vec3> heartPoints = {
        glm::vec3(0.36-0.1, -0.6+0.1, 0.0),
        glm::vec3(0.96-0.1, 0.0+0.1, 0.0),
        glm::vec3(0.46-0.1, 0.76+0.1, 0.0),
        glm::vec3(0.06-0.1, 0.14+0.1, 0.0),
        glm::vec3(0.36-0.1, -0.95+0.1, 0.0),
        glm::vec3(0.66-0.1, 0.14+0.1, 0.0),
        glm::vec3(0.26-0.1, 0.76+0.1, 0.0),
        glm::vec3(-0.24-0.1, 0.0+0.1, 0.0),
        glm::vec3(0.36-0.1, -0.6+0.1, 0.0)
};

static const std::vector<glm::vec3> wavePoints = {
        glm::vec3(-0.35, -0.75, 0.0),
        glm::vec3(-0.35,  0.75, 0.0),
        glm::vec3(0.65, -0.75, 0.0),
        glm::vec3(0.65,  0.75, 0.0)
};

static const std::vector<glm::vec3> moonPoints = {
        glm::vec3(0.26, 0.77, 0.0f),
        glm::vec3(-0.35, 0.66, 0.0f),
        glm::vec3(-0.08, -0.83, 0.0f),
        glm::vec3(0.41, 0.04, 0.0f),
        glm::vec3(0.62, 0.17, 0.0f),
        glm::vec3(0.44, -0.60, 0.0f),
        glm::vec3(-0.12, 0.29, 0.0f),
        glm::vec3(0.26, 0.77, 0.0f)
};

static const std::vector<glm::vec3> eightPoints = {
        glm::vec3(0.00f,0.80f,0.00f),
        glm::vec3(-0.50f,0.60f,0.00f),
        glm::vec3(0.50f,0.20f,0.00f),
        glm::vec3(0.80f,-0.60f,0.00f),
        glm::vec3(-0.80f,-0.60f,0.00f),
        glm::vec3(-0.50f,0.20f,0.00f),
        glm::vec3(0.50f,0.60f,0.00f),
        glm::vec3(0.00f,0.80f,0.00f)
};

static const std::vector<glm::vec3> circlePoints = {
        glm::vec3(0.80f, 0.00f, 0.00f),
        glm::vec3(0.80f, 0.90f, 0.00f),
        glm::vec3(0.00f, 0.90f, 0.00f),
        glm::vec3(0.00f, 0.00f, 0.00f),
        glm::vec3(0.00f, -0.90f, 0.00f),
        glm::vec3(0.80f, -0.90f, 0.00f),
        glm::vec3(0.80f, 0.00f, 0.00f)
};