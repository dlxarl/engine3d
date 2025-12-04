#pragma once
#include "Shader.h"
#include <glm/glm.hpp>

class Scene {
public:
    virtual ~Scene() = default;
    virtual void load() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void draw(Shader& lightingShader, Shader& lampShader, const glm::mat4& view, const glm::mat4& proj) = 0;
};