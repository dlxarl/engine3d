#include "Water.h"
#include <cmath>

Water::Water(float width, float depth, int resolution) 
    : width(width), depth(depth), gridX(resolution), gridZ(resolution), time(0.0f), EBO(0)
{
    // Initialize ripples
    for (auto& ripple : ripples) {
        ripple.active = false;
        ripple.startTime = 0.0f;
        ripple.position = glm::vec3(0.0f);
    }
    generateMesh();
}

Water::~Water() {
    if (EBO != 0) glDeleteBuffers(1, &EBO);
}

void Water::generateMesh() {
    vertices.clear();
    indices.clear();

    float halfWidth = width / 2.0f;
    float halfDepth = depth / 2.0f;
    float stepX = width / (float)(gridX - 1);
    float stepZ = depth / (float)(gridZ - 1);

    for (int z = 0; z < gridZ; z++) {
        for (int x = 0; x < gridX; x++) {
            float posX = -halfWidth + x * stepX;
            float posZ = -halfDepth + z * stepZ;
            float posY = 0.0f;

            vertices.push_back(posX);
            vertices.push_back(posY);
            vertices.push_back(posZ);

            vertices.push_back(0.0f);
            vertices.push_back(1.0f);
            vertices.push_back(0.0f);

            vertices.push_back((float)x / (gridX - 1));
            vertices.push_back((float)z / (gridZ - 1));
        }
    }

    for (int z = 0; z < gridZ - 1; z++) {
        for (int x = 0; x < gridX - 1; x++) {
            int topLeft = z * gridX + x;
            int topRight = topLeft + 1;
            int bottomLeft = (z + 1) * gridX + x;
            int bottomRight = bottomLeft + 1;

            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

float Water::getRippleHeight(float x, float z) {
    float totalRipple = 0.0f;
    
    for (const auto& ripple : ripples) {
        if (!ripple.active) continue;
        
        float age = time - ripple.startTime;
        if (age < 0.0f || age > 5.0f) continue;
        
        float dx = x - ripple.position.x;
        float dz = z - ripple.position.z;
        float dist = std::sqrt(dx * dx + dz * dz);
        
        // Expanding ring wave
        float waveSpeedRipple = 3.0f;
        float waveRadius = age * waveSpeedRipple;
        float ringDist = std::abs(dist - waveRadius);
        
        // Decay based on distance and age
        float decay = std::exp(-age * 0.8f) * std::exp(-ringDist * 2.0f);
        float frequency = 8.0f;
        float rippleHeight = std::sin(dist * frequency - age * 10.0f) * decay;
        
        totalRipple += rippleHeight * 0.1f;
    }
    
    return totalRipple;
}

float Water::getWaveHeight(float x, float z) {
    // Base waves
    float wave1 = sin(x * waveFrequency + time * waveSpeed) * waveHeight;
    float wave2 = sin(z * waveFrequency * 0.8f + time * waveSpeed * 1.3f) * waveHeight * 0.5f;
    float wave3 = sin((x + z) * waveFrequency * 0.5f + time * waveSpeed * 0.7f) * waveHeight * 0.3f;
    float wave4 = sin(x * waveFrequency * 2.0f - time * waveSpeed * 1.5f) * waveHeight * 0.2f;
    float wave5 = cos(z * waveFrequency * 1.5f + time * waveSpeed * 0.9f) * waveHeight * 0.25f;
    
    // Add ripples from object interactions
    float rippleEffect = getRippleHeight(x, z);
    
    return wave1 + wave2 + wave3 + wave4 + wave5 + rippleEffect;
}

glm::vec3 Water::calculateNormal(float x, float z) {
    float delta = 0.1f;
    
    float hL = getWaveHeight(x - delta, z);
    float hR = getWaveHeight(x + delta, z);
    float hD = getWaveHeight(x, z - delta);
    float hU = getWaveHeight(x, z + delta);
    
    glm::vec3 normal;
    normal.x = hL - hR;
    normal.y = 2.0f * delta;
    normal.z = hD - hU;
    
    return glm::normalize(normal);
}

void Water::addRipple(glm::vec3 worldPos) {
    ripples[nextRippleIndex].position = worldPos;
    ripples[nextRippleIndex].startTime = time;
    ripples[nextRippleIndex].active = true;
    
    nextRippleIndex = (nextRippleIndex + 1) % MAX_RIPPLES;
}

bool Water::isInBounds(float worldX, float worldZ) {
    float halfWidth = (width * scale.x) / 2.0f;
    float halfDepth = (depth * scale.z) / 2.0f;
    
    float localX = worldX - position.x;
    float localZ = worldZ - position.z;
    
    return (localX >= -halfWidth && localX <= halfWidth &&
            localZ >= -halfDepth && localZ <= halfDepth);
}

float Water::getHeightAt(float worldX, float worldZ) {
    if (!isInBounds(worldX, worldZ)) {
        return -1000.0f; // Return very low value if outside bounds
    }
    return position.y + getWaveHeight(worldX, worldZ);
}

void Water::checkObjectInteraction(glm::vec3 objectPos, float objectRadius) {
    if (!isInBounds(objectPos.x, objectPos.z)) return;
    
    float waterHeight = getHeightAt(objectPos.x, objectPos.z);
    float objectBottom = objectPos.y - objectRadius;
    float objectTop = objectPos.y + objectRadius;
    
    // Check if object is touching or entering water
    if (objectBottom <= waterHeight && objectTop >= waterHeight - 0.5f) {
        // Object is at water surface - create ripple
        addRipple(glm::vec3(objectPos.x, waterHeight, objectPos.z));
    }
}

void Water::updateWaves() {
    float halfWidth = width / 2.0f;
    float halfDepth = depth / 2.0f;
    float stepX = width / (float)(gridX - 1);
    float stepZ = depth / (float)(gridZ - 1);

    for (int z = 0; z < gridZ; z++) {
        for (int x = 0; x < gridX; x++) {
            int index = (z * gridX + x) * 8;
            
            float posX = -halfWidth + x * stepX;
            float posZ = -halfDepth + z * stepZ;
            
            float worldX = posX * scale.x + position.x;
            float worldZ = posZ * scale.z + position.z;
            
            vertices[index + 1] = getWaveHeight(worldX, worldZ);
            
            glm::vec3 normal = calculateNormal(worldX, worldZ);
            vertices[index + 3] = normal.x;
            vertices[index + 4] = normal.y;
            vertices[index + 5] = normal.z;
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());
    
    // Deactivate old ripples
    for (auto& ripple : ripples) {
        if (ripple.active && (time - ripple.startTime) > 5.0f) {
            ripple.active = false;
        }
    }
}

void Water::update(float deltaTime) {
    time += deltaTime;
    updateWaves();
}

void Water::draw(Shader& shader) {
    shader.setMat4("model", model);
    shader.setVec3("objectColor", waterColor);
    shader.setFloat("waterDepth", waterDepth);
    
    // Send ripple data to shader
    int activeCount = 0;
    for (int i = 0; i < MAX_RIPPLES; i++) {
        std::string base = "ripplePositions[" + std::to_string(i) + "]";
        std::string timeBase = "rippleTimes[" + std::to_string(i) + "]";
        
        if (ripples[i].active) {
            shader.setVec3(base, ripples[i].position);
            shader.setFloat(timeBase, ripples[i].startTime);
            activeCount++;
        } else {
            shader.setVec3(base, glm::vec3(0.0f));
            shader.setFloat(timeBase, -100.0f);
        }
    }
    shader.setInt("activeRipples", activeCount);
    
    shader.setBool("material.hasAlbedo", false);
    shader.setBool("material.hasNormal", false);
    shader.setBool("material.hasMetallic", false);
    shader.setBool("material.hasRoughness", false);
    shader.setBool("material.hasAO", false);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glDisable(GL_BLEND);
}
