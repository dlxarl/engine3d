#pragma once
#include "Scene.h"
#include "Shape.h"
#include <vector>
#include <memory>
#include <glm/glm.hpp>

class DemoScene : public Scene {
public:
    void load() override;
    void update(float deltaTime) override;
    void draw(Shader& lightingShader, Shader& lampShader) override;

private:
    std::vector<std::unique_ptr<Shape>> shapes;
    std::unique_ptr<Shape> lightCube;
    glm::vec3 lightPos;
};