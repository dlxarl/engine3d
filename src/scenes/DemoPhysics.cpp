#include "include/DemoPhysics.h"
#include "Cube.h"
// #include "Plane.h"
// #include "Sphere.h"
#include <GLFW/glfw3.h>
#include <iostream>

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