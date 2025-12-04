#include "include/DemoScene.h"
#include "Cube.h"
#include <GLFW/glfw3.h>

void DemoScene::load() {
    auto cube1 = std::make_unique<Cube>();
    cube1->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    cube1->setColor(glm::vec3(1.0f, 0.5f, 0.31f));
    shapes.push_back(std::move(cube1));

    auto cube2 = std::make_unique<Cube>();
    cube2->setPosition(glm::vec3(2.0f, 1.0f, -3.0f));
    cube2->setColor(glm::vec3(0.0f, 0.8f, 0.8f));
    shapes.push_back(std::move(cube2));

    lightPos = glm::vec3(1.2f, 1.0f, 2.0f);
    lightCube = std::make_unique<Cube>();
    lightCube->setPosition(lightPos);
    lightCube->setScale(glm::vec3(0.2f));
}

void DemoScene::update(float deltaTime) {
    lightPos.x = 1.0f + sin(glfwGetTime()) * 2.0f;
    lightPos.z = sin(glfwGetTime() / 2.0f) * 1.0f;
    lightCube->setPosition(lightPos);
}

void DemoScene::draw(Shader& lightingShader, Shader& lampShader) {
    lightingShader.use();
    lightingShader.setVec3("lightPos", lightPos);
    lightingShader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));

    for (const auto& shape : shapes) {
        lightingShader.setVec3("objectColor", shape->getColor());
        shape->draw(lightingShader);
    }

    lampShader.use();
    lightCube->draw(lampShader);
}