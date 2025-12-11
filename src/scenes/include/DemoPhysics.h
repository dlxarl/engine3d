#pragma once
#include "Scene.h"
#include "Shape.h"
#include "Skybox.h"
#include "PostProcessor.h"
#include "ShadowMap.h"
#include "Player.h"
#include "HUD.h"
#include "Audio.h"
#include "Entity.h"
#include "FriendlyEntity.h"
#include "EnemyEntity.h"
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

private:
    std::vector<std::shared_ptr<Shape>> shapes;
    std::vector<std::unique_ptr<Entity>> entities;
    std::unique_ptr<Shape> lightCube;
    glm::vec3 lightPos;
    std::unique_ptr<Skybox> skybox;

    std::unique_ptr<PostProcessor> postProcessor;
    std::unique_ptr<ShadowMap> shadowMap;
    std::unique_ptr<Shader> depthShader;
    std::shared_ptr<Player> player;

    std::unique_ptr<HUD> hud;
    std::shared_ptr<Texture> crosshairTexture;

    // Audio
    std::shared_ptr<AudioSource3D> speakerAudio;
    glm::vec3 speakerPosition;

    void renderScene(Shader& shader);
};