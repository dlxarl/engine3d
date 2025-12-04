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

    auto cube = std::make_unique<Cube>();
    cube->setPosition(glm::vec3(2.0f, 0.0f, 0.0f));
    auto woodTex = std::make_shared<Texture>("assets/textures/wood.jpg", "texture_albedo");
    cube->addTexture(woodTex);
    shapes.push_back(std::move(cube));

    // building
    auto building = std::make_unique<Model>("assets/models/cartoon_building.obj");
    building->setPosition(glm::vec3(0.0f, -1.1f, 0.0f));
    building->setScale(glm::vec3(0.1f));

    auto albedo = std::make_shared<Texture>("assets/textures/cartoon_building/cartoon_building_Albedo.png", "texture_albedo");
    building->addTexture(albedo);

    auto normal = std::make_shared<Texture>("assets/textures/cartoon_building/cartoon_building_Normal.png", "texture_normal");
    building->addTexture(normal);

    auto roughness = std::make_shared<Texture>("assets/textures/cartoon_building/cartoon_building_Roughness.png", "texture_roughness");
    building->addTexture(roughness);

    auto metallic = std::make_shared<Texture>("assets/textures/cartoon_building/cartoon_building_Metallic.png", "texture_metallic");
    building->addTexture(metallic);

    auto ao = std::make_shared<Texture>("assets/textures/cartoon_building/cartoon_building_AO.png", "texture_ao");
    building->addTexture(ao);

    shapes.push_back(std::move(building));

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