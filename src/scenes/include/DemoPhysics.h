#pragma once
#include "Scene.h"
#include "Shape.h"
#include "Skybox.h"
#include "Player.h"
#include <vector>
#include <memory>
#include <glm/glm.hpp>

class DemoPhysics : public Scene {
public:
    void load() override;
    void update(float deltaTime) override;
    void draw(Shader& lightingShader, Shader& lampShader, const glm::mat4& view, const glm::mat4& proj) override;

private:
    std::vector<std::unique_ptr<Shape>> shapes;
    std::unique_ptr<Shape> lightCube;
    std::unique_ptr<Skybox> skybox;
    std::shared_ptr<Player> player;
    glm::vec3 lightPos;
};