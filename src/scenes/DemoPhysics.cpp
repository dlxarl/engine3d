#include "include/DemoPhysics.h"
#include "Cube.h"
#include "Model.h"
#include "Plane.h"
#include "Texture.h"
#include "ShadowMap.h"
#include "PostProcessor.h"
#include "Player.h"
#include "HUD.h"
#include "FriendlyEntity.h"
#include "EnemyEntity.h"
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "Sphere.h"

extern glm::vec3 cameraFront;
extern glm::vec3 cameraUp;
extern glm::vec3 cameraPos;

void DemoPhysics::load() {
    shapes.clear();
    entities.clear();

    postProcessor = std::make_unique<PostProcessor>(1920, 1080);
    shadowMap = std::make_unique<ShadowMap>();
    depthShader = std::make_unique<Shader>("src/shadow_depth.vert", "src/shadow_depth.frag");

    hud = std::make_unique<HUD>(1920, 1080);
    crosshairTexture = std::make_shared<Texture>("assets/hud/crosshair.png", "texture_diffuse");

    skybox = std::make_unique<Skybox>("assets/skybox/night.hdr");

    auto grassTexture = std::make_shared<Texture>("assets/textures/grass/albedo.jpg", "texture_albedo");

    auto floor = std::make_shared<Plane>();
    floor->setPosition(glm::vec3(0.0f, -2.5f, 0.0f));
    floor->setScale(glm::vec3(40.0f, 0.1f, 40.0f));
    floor->setColor(glm::vec3(0.5f, 0.5f, 0.5f));
    floor->isStatic = true;
    floor->hasCollision = true;
    floor->addTexture(grassTexture);
    shapes.push_back(floor);

    auto fallingCube = std::make_shared<Cube>();
    fallingCube->setPosition(glm::vec3(0.5f, 5.0f, 0.0f));
    fallingCube->setColor(glm::vec3(1.0f, 0.5f, 0.0f));
    fallingCube->useGravity = true;
    fallingCube->hasCollision = true;
    auto woodTexture = std::make_shared<Texture>("assets/textures/wood.jpg", "texture_albedo");
    fallingCube->addTexture(woodTexture);
    shapes.push_back(fallingCube);

    auto floatingCube = std::make_shared<Cube>();
    floatingCube->setPosition(glm::vec3(-2.5f, 1.0f, 2.0f));
    floatingCube->setColor(glm::vec3(0.0f, 1.0f, 1.0f));
    floatingCube->useGravity = false;
    floatingCube->hasCollision = true;
    auto brickTexture = std::make_shared<Texture>("assets/textures/bricks.png", "texture_albedo");
    floatingCube->addTexture(brickTexture);
    shapes.push_back(floatingCube);

    auto floatingSphere = std::make_shared<Sphere>(1.0f);
    floatingSphere->setPosition(glm::vec3(2.5f, 1.0f, 2.0f));
    floatingSphere->setColor(glm::vec3(1.0f, 0.0f, 0.0f));
    floatingSphere->useGravity = false;
    floatingSphere->hasCollision = true;
    shapes.push_back(floatingSphere);

    // speakerPosition = glm::vec3(4.0f, -1.9f, 0.0f);
    // auto speaker = std::make_shared<Model>("assets/models/speaker.fbx");
    // speaker->setPosition(speakerPosition);
    // speaker->setScale(glm::vec3(0.5f));
    // speaker->rotate(-90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    // speaker->useGravity = false;
    // speaker->hasCollision = true;
    // auto albedo_speaker = std::make_shared<Texture>("assets/textures/speaker/Speaker_Base_Colour.png", "texture_albedo");
    // speaker->addTexture(albedo_speaker);
    // auto roughness_speaker = std::make_shared<Texture>("assets/textures/speaker/Speaker_Roughness.png", "texture_roughness");
    // speaker->addTexture(roughness_speaker);
    // shapes.push_back(speaker);
    //
    // auto& audio = AudioSystem::getInstance();
    // ALuint songBuffer = audio.loadSound("assets/audio/song.mp3");
    //
    // speakerAudio = audio.createSource3D();
    // speakerAudio->setBuffer(songBuffer);
    // speakerAudio->setPosition(speakerPosition);
    // speakerAudio->setLooping(true);
    // speakerAudio->setVolume(1.0f);
    // speakerAudio->setReferenceDistance(2.0f);
    // speakerAudio->setMaxDistance(30.0f);
    // speakerAudio->setRolloffFactor(1.0f);
    // speakerAudio->play();

    // Entities
    // Friendly entity
    // auto friendly = std::make_unique<FriendlyEntity>("assets/models/cat.obj", glm::vec3(-3.0f, -1.9f, 3.0f), glm::vec3(0.01f));
    // if (friendly->visualShape) {
    //     shapes.push_back(friendly->visualShape);
    // }
    // entities.push_back(std::move(friendly));
    //
    // // Enemy entity
    // auto enemy = std::make_unique<EnemyEntity>("assets/models/wolf.obj", glm::vec3(3.0f, -3.0f, -3.0f), glm::vec3(2.2f));
    // if (enemy->visualShape) {
    //     shapes.push_back(enemy->visualShape);
    // }
    // entities.push_back(std::move(enemy));

    lightPos = glm::vec3(40.0f, 5.0f, -5.0f);
    lightCube = std::make_unique<Cube>();
    lightCube->setPosition(lightPos);
    lightCube->setScale(glm::vec3(0.5f));

    player = std::make_shared<Player>(glm::vec3(0.0f, -1.5f, 2.0f));
    player->setGrounded(true);
}

void DemoPhysics::update(float deltaTime) {
    GLFWwindow* window = glfwGetCurrentContext();

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        load();
        return;
    }

    static bool mKeyPressed = false;
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS && !mKeyPressed) {
        postProcessor->enabled = !postProcessor->enabled;
        std::cout << "PostProcessing: " << (postProcessor->enabled ? "ON" : "OFF") << std::endl;
        mKeyPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_RELEASE) {
        mKeyPressed = false;
    }

    float gravity = -19.6f;

    for (auto& object : shapes) {
        if (object->isStatic) continue;

        if (object->useGravity) {
            object->velocity.y += gravity * deltaTime;
        }

        glm::vec3 oldPosition = object->position;
        object->setPosition(object->position + object->velocity * deltaTime);

        if (object->hasCollision) {
            for (auto& other : shapes) {
                if (object.get() == other.get()) continue;
                if (!other->hasCollision) continue;

                if (object->checkCollision(*other)) {
                    object->setPosition(oldPosition);
                    object->velocity = glm::vec3(0.0f);
                }
            }
        }

        if (object->position.y < -30.0f) {
            object->setPosition(glm::vec3(0.0f, 10.0f, 0.0f));
            object->velocity = glm::vec3(0.0f);
        }
    }

    glm::vec3 moveDir = glm::vec3(0.0f);
    glm::vec3 flatFront = glm::normalize(glm::vec3(cameraFront.x, 0.0f, cameraFront.z));
    glm::vec3 flatRight = glm::normalize(glm::cross(flatFront, glm::vec3(0.0f, 1.0f, 0.0f)));

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) moveDir += flatFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) moveDir -= flatFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) moveDir -= flatRight;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) moveDir += flatRight;

    if (glm::length(moveDir) > 0) moveDir = glm::normalize(moveDir);

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) player->runSpeed = 8.0f;
    else player->runSpeed = 6.0f;

    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) player->setCrouch(true);
    else player->setCrouch(false);

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) player->jump();

    player->move(moveDir);
    player->update(deltaTime, shapes);

    cameraPos = player->getCameraPosition();

    for (auto& entity : entities) {
        entity->update(deltaTime);
        
        EnemyEntity* enemy = dynamic_cast<EnemyEntity*>(entity.get());
        if (enemy && enemy->isAlive) {
            enemy->setTargetPosition(cameraPos);
        }
    }
}

void DemoPhysics::renderScene(Shader& shader) {
    for (const auto& shape : shapes) {
        shader.setVec3("objectColor", shape->getColor());
        shape->draw(shader);
    }
}

void DemoPhysics::drawShadow(Shader& shadowShader) {
    renderScene(shadowShader);
}

glm::vec3 DemoPhysics::getLightPos() const {
    return lightPos;
}

void DemoPhysics::draw(Shader& lightingShader, Shader& lampShader, const glm::mat4& view, const glm::mat4& proj) {
    int scrWidth, scrHeight;
    glfwGetFramebufferSize(glfwGetCurrentContext(), &scrWidth, &scrHeight);

    glm::mat4 lightProjection, lightView, lightSpaceMatrix;
    float near_plane = 1.0f, far_plane = 200.0f;

    lightProjection = glm::ortho(-35.0f, 35.0f, -35.0f, 35.0f, near_plane, far_plane);
    lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
    lightSpaceMatrix = lightProjection * lightView;

    depthShader->use();
    depthShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);

    shadowMap->bind();
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    renderScene(*depthShader);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    shadowMap->unbind(scrWidth, scrHeight);

    if (postProcessor->enabled) {
        postProcessor->resize(scrWidth, scrHeight);
        postProcessor->beginRender();
    } else {
        glViewport(0, 0, scrWidth, scrHeight);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    lightingShader.use();
    lightingShader.setVec3("lightPos", lightPos);
    lightingShader.setVec3("lightColor", glm::vec3(1.0f));
    lightingShader.setVec3("viewPos", cameraPos);
    lightingShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

    glActiveTexture(GL_TEXTURE10);
    glBindTexture(GL_TEXTURE_2D, shadowMap->depthMap);
    lightingShader.setInt("shadowMap", 10);

    glDisable(GL_CULL_FACE);

    renderScene(lightingShader);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    if (skybox) {
        skybox->draw(view, proj);
    }

    if (postProcessor->enabled) {
        postProcessor->endRender();
        postProcessor->draw(view, proj, 60.0f);
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDisable(GL_DEPTH_TEST);

    hud->resize(scrWidth, scrHeight);

    float crosshairSize = 4.0f;
    float x = (scrWidth / 2.0f) - (crosshairSize / 2.0f);
    float y = (scrHeight / 2.0f) - (crosshairSize / 2.0f);

    hud->drawSprite(crosshairTexture, glm::vec2(x, y), glm::vec2(crosshairSize, crosshairSize));

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}