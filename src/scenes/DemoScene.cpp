#include "include/DemoScene.h"
#include "Cube.h"
#include "Plane.h"
#include "Sphere.h"
#include "Model.h"
#include "Texture.h"
#include <GLFW/glfw3.h>

void DemoScene::load() {
    skybox = std::make_unique<Skybox>("assets/textures/skybox/night.hdr");

    auto floor = std::make_unique<Plane>();
    floor->setPosition(glm::vec3(0.0f, -1.0f, 0.0f));
    floor->setColor(glm::vec3(0.5f, 0.5f, 0.5f));
    shapes.push_back(std::move(floor));

    auto sphere = std::make_unique<Sphere>(1.0f);
    sphere->setPosition(glm::vec3(-1.5f, 0.0f, -2.0f));
    sphere->setColor(glm::vec3(1.0f, 0.0f, 0.0f));
    shapes.push_back(std::move(sphere));

    auto brickTexture = std::make_shared<Texture>("assets/textures/wood.jpg");

    auto cube1 = std::make_unique<Cube>();
    cube1->setPosition(glm::vec3(1.5f, 0.0f, 0.0f));
    cube1->setTexture(brickTexture);
    shapes.push_back(std::move(cube1));

    auto car = std::make_unique<Model>("assets/models/golf.obj");
    car->setPosition(glm::vec3(0.0f, -1.1f, 0.0f));
    car->setScale(glm::vec3(0.001f));
    car->setColor(glm::vec3(1.0f, 0.0f, 1.0f));
    shapes.push_back(std::move(car));

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

void DemoScene::draw(Shader& lightingShader, Shader& lampShader, const glm::mat4& view, const glm::mat4& proj) {
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