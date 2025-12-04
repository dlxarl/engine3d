#include "include/DemoPhysics.h"
#include "Cube.h"
#include <GLFW/glfw3.h>
#include <iostream>

extern glm::vec3 cameraFront;
extern glm::vec3 cameraUp;
extern glm::vec3 cameraPos;

void DemoPhysics::load() {
    skybox = std::make_unique<Skybox>("assets/textures/skybox/night.hdr");

    auto floor = std::make_unique<Cube>();
    floor->setPosition(glm::vec3(0.0f, -2.5f, 0.0f));
    floor->setScale(glm::vec3(20.0f, 1.0f, 20.0f));
    floor->setColor(glm::vec3(0.5f, 0.5f, 0.5f));
    floor->isStatic = true;
    floor->hasCollision = true;
    shapes.push_back(std::move(floor));

    auto fallingCube = std::make_unique<Cube>();
    fallingCube->setPosition(glm::vec3(0.5f, 5.0f, 0.0f));
    fallingCube->setColor(glm::vec3(1.0f, 0.5f, 0.0f));
    fallingCube->useGravity = true;
    fallingCube->hasCollision = true;
    shapes.push_back(std::move(fallingCube));

    auto floatingCube = std::make_unique<Cube>();
    floatingCube->setPosition(glm::vec3(-1.5f, 2.0f, 0.0f));
    floatingCube->setColor(glm::vec3(0.0f, 1.0f, 1.0f));
    floatingCube->useGravity = false;
    floatingCube->hasCollision = true;
    shapes.push_back(std::move(floatingCube));

    lightPos = glm::vec3(0.0f, 4.0f, 4.0f);
    lightCube = std::make_unique<Cube>();
    lightCube->setPosition(lightPos);
    lightCube->setScale(glm::vec3(0.2f));

    player = std::make_shared<Player>(glm::vec3(0.0f, 50.0f, 0.0f));
}

void DemoPhysics::update(float deltaTime) {
    lightPos.x = sin(glfwGetTime()) * 3.0f;
    lightCube->setPosition(lightPos);

    float gravity = -9.8f;

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

        if (object->position.y < -10.0f) {
            object->setPosition(glm::vec3(0.0f, 8.0f, 0.0f));
            object->velocity = glm::vec3(0.0f);
        }
    }

    GLFWwindow* window = glfwGetCurrentContext();

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

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) player->jump();

    player->move(moveDir);
    player->update(deltaTime, shapes);

    cameraPos = player->getCameraPosition();
}

void DemoPhysics::draw(Shader& lightingShader, Shader& lampShader, const glm::mat4& view, const glm::mat4& proj) {
    lightingShader.use();
    lightingShader.setVec3("lightPos", lightPos);
    lightingShader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));

    glDisable(GL_CULL_FACE);

    for (const auto& shape : shapes) {
        lightingShader.setVec3("objectColor", shape->getColor());
        shape->draw(lightingShader);
    }

    glEnable(GL_CULL_FACE);

    lampShader.use();
    lightCube->draw(lampShader);

    if (skybox) {
        skybox->draw(view, proj);
    }
}