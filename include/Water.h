#pragma once
#include "Shape.h"
#include "Shader.h"
#include <glm/glm.hpp>
#include <memory>
#include <array>

struct Ripple {
    glm::vec3 position;
    float startTime;
    bool active;
};

class Water : public Shape {
public:
    Water(float width = 10.0f, float depth = 10.0f, int resolution = 64);
    ~Water();

    void draw(Shader& shader) override;
    void update(float deltaTime);
    
    // Add ripple at world position (e.g., when object touches water)
    void addRipple(glm::vec3 worldPos);
    
    // Check if a position is in the water and add ripple if so
    void checkObjectInteraction(glm::vec3 objectPos, float objectRadius = 0.5f);
    
    // Get water height at world position
    float getHeightAt(float worldX, float worldZ);
    
    // Check if point is within water bounds
    bool isInBounds(float worldX, float worldZ);

    float waveSpeed = 1.0f;
    float waveHeight = 0.1f;
    float waveFrequency = 2.0f;
    glm::vec3 waterColor = glm::vec3(0.02f, 0.06f, 0.1f); // Dark water color
    float transparency = 0.85f; // Less transparent
    float waterDepth = 3.0f; // Visual depth of water

private:
    static const int MAX_RIPPLES = 8;
    
    int gridX, gridZ;
    float width, depth;
    float time;
    
    unsigned int EBO;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    
    std::array<Ripple, MAX_RIPPLES> ripples;
    int nextRippleIndex = 0;
    
    void generateMesh();
    void updateWaves();
    float getWaveHeight(float x, float z);
    float getRippleHeight(float x, float z);
    glm::vec3 calculateNormal(float x, float z);
};
