#pragma once
#include "Shader.h"

class Scene {
public:
    virtual ~Scene() = default;
    virtual void load() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void draw(Shader& lightingShader, Shader& lampShader) = 0;
};