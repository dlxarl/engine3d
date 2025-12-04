#include "include/DemoPhysics.h"
#include "Cube.h"
#include "Plane.h"
#include "Texture.h"
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

extern glm::vec3 cameraFront;
extern glm::vec3 cameraUp;
extern glm::vec3 cameraPos;

void DemoPhysics::load() {
    shapes.clear();

    shadowMap = std::make_unique<ShadowMap>();
    depthShader = std::make_unique<Shader>("src/shadow_depth.vert", "src/shadow_depth.frag");

    skybox = std::make_unique<Skybox>("assets/textures/skybox/night.hdr");

    auto grassTexture = std::make_shared<Texture>("assets/textures/grass/albedo.jpg", "texture_albedo");

    auto floor = std::make_unique<Plane>();
    floor->setPosition(glm::vec3(0.0f, -2.5f, 0.0f));
    floor->setScale(glm::vec3(20.0f, 0.1f, 20.0f));
    floor->setColor(glm::vec3(0.5f, 0.5f, 0.5f));
    floor->isStatic = true;
    floor->hasCollision = true;
    floor->addTexture(grassTexture);
    shapes.push_back(std::move(floor));

    auto fallingCube = std::make_unique<Cube>();
    fallingCube->setPosition(glm::vec3(0.5f, 5.0f, 0.0f));
    fallingCube->setColor(glm::vec3(1.0f, 0.5f, 0.0f));
    fallingCube->useGravity = true;
    fallingCube->hasCollision = true;
    shapes.push_back(std::move(fallingCube));

    auto floatingCube = std::make_unique<Cube>();
    floatingCube->setPosition(glm::vec3(-2.5f, 1.0f, 2.0f));
    floatingCube->setColor(glm::vec3(0.0f, 1.0f, 1.0f));
    floatingCube->useGravity = false;
    floatingCube->hasCollision = true;
    shapes.push_back(std::move(floatingCube));

    lightPos = glm::vec3(0.0f, 20.0f, 0.0f);
    lightCube = std::make_unique<Cube>();
    lightCube->setPosition(lightPos);
    lightCube->setScale(glm::vec3(0.5f));
    lightCube->setColor(glm::vec3(1.0f, 1.0f, 1.0f));

    player = std::make_shared<Player>(glm::vec3(0.0f, 25.0f, 2.0f));
}

void DemoPhysics::update(float deltaTime) {
    GLFWwindow* window = glfwGetCurrentContext();

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        load();
        return;
    }

    float time = (float)glfwGetTime();
    lightPos.x = sin(time * 0.5f) * 20.0f;
    lightPos.z = cos(time * 0.5f) * 20.0f;
    lightPos.y = 15.0f;

    lightCube->setPosition(lightPos);

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
                    if (object->velocity.y < 0) {
                        float otherTop = other->position.y + other->scale.y * 0.5f;
                        float myBottom = object->scale.y * 0.5f;
                        object->position.y = otherTop + myBottom;
                        object->velocity.y = 0.0f;
                    } else {
                        object->setPosition(oldPosition);
                        object->velocity = glm::vec3(0.0f);
                    }
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
    else player->runSpeed = 4.0f;

    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) player->setCrouch(true);
    else player->setCrouch(false);

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) player->jump();

    player->move(moveDir);
    player->update(deltaTime, shapes);

    cameraPos = player->getCameraPosition();
}

void DemoPhysics::renderScene(Shader& shader) {
    for (const auto& shape : shapes) {
        shader.setVec3("objectColor", shape->getColor());
        shape->draw(shader);
    }
}

void DemoPhysics::draw(Shader& lightingShader, Shader& lampShader, const glm::mat4& view, const glm::mat4& proj) {
    glm::mat4 lightProjection, lightView;
    glm::mat4 lightSpaceMatrix;
    float near_plane = 1.0f, far_plane = 100.0f;

    lightProjection = glm::ortho(-35.0f, 35.0f, -35.0f, 35.0f, near_plane, far_plane);

    lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));

    lightSpaceMatrix = lightProjection * lightView;

    depthShader->use();
    depthShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);

    shadowMap->bind();
    glClear(GL_DEPTH_BUFFER_BIT);

    glCullFace(GL_FRONT);
    renderScene(*depthShader);
    glCullFace(GL_BACK);

    int scrWidth, scrHeight;
    glfwGetFramebufferSize(glfwGetCurrentContext(), &scrWidth, &scrHeight);
    shadowMap->unbind(scrWidth, scrHeight);

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

    lampShader.use();
    lightCube->draw(lampShader);

    if (skybox) {
        skybox->draw(view, proj);
    }
}