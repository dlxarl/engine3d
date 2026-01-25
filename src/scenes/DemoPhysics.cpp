#include "include/DemoPhysics.h"
#include "Cube.h"
#include "Plane.h"
#include "Texture.h"
#include "ShadowMap.h"
#include "PostProcessor.h"
#include "Player.h"
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "Sphere.h"
#include "Cylinder.h"
extern glm::vec3 cameraFront;
extern glm::vec3 cameraUp;
extern glm::vec3 cameraPos;

// Активний об’єкт
static std::shared_ptr<Shape> g_controlledShape = nullptr;
static int g_controlledIndex = 0;

// Освітлення і тіні
static bool g_enableLighting = true;
static bool g_enableShadows = true;

void DemoPhysics::load() {
    shapes.clear();

    postProcessor = std::make_unique<PostProcessor>(1920, 1080);
    shadowMap = std::make_unique<ShadowMap>();
    depthShader = std::make_unique<Shader>("src/shadow_depth.vert", "src/shadow_depth.frag");


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
    floatingCube->setPosition(glm::vec3(-2.5f, 0.0f, 2.0f));
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

    auto cylinder = std::make_shared<Cylinder>(0.5f, 1.5f, 32);
    cylinder->setPosition(glm::vec3(0.0f, 0.0f, -3.0f));
    cylinder->setColor(glm::vec3(0.7f, 0.2f, 1.0f));
    cylinder->useGravity = false;
    cylinder->hasCollision = true;
    shapes.push_back(cylinder);

    // Активний об’єкт — перший у списку
    g_controlledIndex = 0;
    g_controlledShape = shapes[g_controlledIndex];

    lightPos = glm::vec3(40.0f, 5.0f, -5.0f);
    lightCube = std::make_unique<Cube>();
    lightCube->setPosition(lightPos);
    lightCube->setScale(glm::vec3(0.5f));

    player = std::make_shared<Player>(glm::vec3(0.0f, 0.5f, 2.0f));
    player->setGrounded(true);
}

void DemoPhysics::update(float deltaTime) {
    GLFWwindow* window = glfwGetCurrentContext();

    // Перезавантаження сцени — тепер на T
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
        load();
        return;
    }

    // Перемикання об’єкта на N
    static bool nPressed = false;
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS && !nPressed) {
        nPressed = true;

        g_controlledIndex++;
        if (g_controlledIndex >= shapes.size())
            g_controlledIndex = 0;

        g_controlledShape = shapes[g_controlledIndex];
        std::cout << "Selected object #" << g_controlledIndex << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_RELEASE) {
        nPressed = false;
    }

    // Перемикач освітлення (L)
    static bool lPressed = false;
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && !lPressed) {
        lPressed = true;
        g_enableLighting = !g_enableLighting;
        std::cout << "Lighting: " << (g_enableLighting ? "ON" : "OFF") << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_RELEASE) lPressed = false;

    // Перемикач тіней (K)
    static bool kPressed = false;
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS && !kPressed) {
        kPressed = true;
        g_enableShadows = !g_enableShadows;
        std::cout << "Shadows: " << (g_enableShadows ? "ON" : "OFF") << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_RELEASE) kPressed = false;

    // Постпроцесинг
    static bool mKeyPressed = false;
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS && !mKeyPressed) {
        postProcessor->enabled = !postProcessor->enabled;
        std::cout << "PostProcessing: " << (postProcessor->enabled ? "ON" : "OFF") << std::endl;
        mKeyPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_RELEASE) {
        mKeyPressed = false;
    }

    // Фізика
    float gravity = -19.6f;

    for (auto& object : shapes) {
        if (object->isStatic) continue;

        if (object->useGravity)
            object->velocity.y += gravity * deltaTime;

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

    // Рух активного об’єкта стрілками
    if (g_controlledShape) {
        glm::vec3 pos = g_controlledShape->position;
        float moveSpeed = 3.0f * deltaTime;

        // XZ рух
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)    pos.z -= moveSpeed;
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)  pos.z += moveSpeed;
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)  pos.x -= moveSpeed;
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) pos.x += moveSpeed;

        // Y рух (вгору/вниз)
        if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS)
            pos.y += moveSpeed;

        if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS)
            pos.y -= moveSpeed;

        g_controlledShape->setPosition(pos);
    }


    //Трансформації R/I/O
    if (g_controlledShape) {

        // R — обертання
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
            g_controlledShape->rotate(90.0f * deltaTime, glm::vec3(0, 1, 0));
        }

        // I — збільшення
        if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
            glm::vec3 sc = g_controlledShape->scale;
            sc += glm::vec3(1.0f) * deltaTime;
            g_controlledShape->setScale(sc);
        }

        // O — зменшення
        if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
            glm::vec3 sc = g_controlledShape->scale;
            sc -= glm::vec3(1.0f) * deltaTime;
            sc = glm::max(sc, glm::vec3(0.1f));
            g_controlledShape->setScale(sc);
        }
    }

    // Рух гравця
    glm::vec3 moveDir = glm::vec3(0.0f);
    glm::vec3 flatFront = glm::normalize(glm::vec3(cameraFront.x, 0.0f, cameraFront.z));
    glm::vec3 flatRight = glm::normalize(glm::cross(flatFront, glm::vec3(0.0f, 1.0f, 0.0f)));

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) moveDir += flatFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) moveDir -= flatFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) moveDir -= flatRight;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) moveDir += flatRight;

    if (glm::length(moveDir) > 0) moveDir = glm::normalize(moveDir);

    player->runSpeed = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) ? 8.0f : 6.0f;

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

    // Передаємо прапорці освітлення і тіней
    lightingShader.setInt("enableLighting", g_enableLighting ? 1 : 0);
    lightingShader.setInt("enableShadows", g_enableShadows ? 1 : 0);

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


    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}

void DemoPhysics::drawDepth(Shader& depthShader) {
    renderScene(depthShader);
}
