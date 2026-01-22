#pragma once
#include "Shader.h"
#include <glm/glm.hpp>

class Scene {
public:
    virtual ~Scene() = default;
    virtual void load() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void draw(Shader&, Shader&, const glm::mat4& view, const glm::mat4& proj) = 0;


    virtual void drawShadow(Shader& shadowShader) = 0;
    virtual void drawDepth(Shader& depthShader) = 0;
    virtual glm::vec3 getLightPos() const = 0;
};