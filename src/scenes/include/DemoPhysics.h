#pragma once
#include "Scene.h"
#include "Shape.h"
#include "Skybox.h"
#include "PostProcessor.h"
#include "ShadowMap.h"
#include "Player.h"
#include <vector>
#include <memory>
#include <glm/glm.hpp>

class DemoPhysics : public Scene {
public:
    void load() override;
    void update(float deltaTime) override;
    void draw(Shader& lightingShader, Shader& lampShader, const glm::mat4& view, const glm::mat4& proj) override;

    void drawShadow(Shader& shadowShader) override;
    glm::vec3 getLightPos() const override;
    void drawDepth(Shader& depthShader) override;

private:
    std::vector<std::shared_ptr<Shape>> shapes;

    std::unique_ptr<Shape> lightCube;
    glm::vec3 lightPos;
    std::unique_ptr<Skybox> skybox;

    std::unique_ptr<PostProcessor> postProcessor;
    std::unique_ptr<ShadowMap> shadowMap;
    std::unique_ptr<Shader> depthShader;
    std::shared_ptr<Player> player;





    void renderScene(Shader& shader);
};