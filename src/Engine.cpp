#include "Engine.h"
#include "Cube.h"
#include <iostream>

Engine::Engine() {
    window = nullptr;
    width = 800;
    height = 600;

    input = std::make_unique<Input>();

    cameraPos   = glm::vec3(0.0f, 0.0f,  3.0f);
    cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

    lightPos    = glm::vec3(1.2f, 1.0f, 2.0f);

    deltaTime = 0.0f;
    lastFrame = 0.0f;
}

Engine::~Engine() {
    glfwTerminate();
}

int Engine::init(int width, int height, const char* title) {
    this->width = width;
    this->height = height;

    // GLFW initialization
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetWindowUserPointer(window, this);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // GLAD initialization
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glEnable(GL_DEPTH_TEST);

    // Shaders
    lightingShader = std::make_unique<Shader>("src/lighting.vert", "src/lighting.frag");
    lampShader     = std::make_unique<Shader>("src/lighting.vert", "src/lamp.frag");

    // Scene objects

    // Orange cube
    auto cube1 = std::make_unique<Cube>();
    cube1->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    cube1->setColor(glm::vec3(1.0f, 0.5f, 0.31f));
    shapes.push_back(std::move(cube1));

    // Blue cube
    auto cube2 = std::make_unique<Cube>();
    cube2->setPosition(glm::vec3(2.0f, 1.0f, -3.0f));
    cube2->setColor(glm::vec3(0.0f, 0.8f, 0.8f));
    shapes.push_back(std::move(cube2));

    // Lamp (light source)
    lightCube = std::make_unique<Cube>();
    lightCube->setPosition(lightPos);
    lightCube->setScale(glm::vec3(0.2f));

    return 0;
}

void Engine::run() {
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput();
        update();
        render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void Engine::processInput() {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float velocity = 2.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraFront * velocity;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraFront * velocity;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * velocity;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * velocity;
}

void Engine::update() {
    glm::vec3 front;
    front.x = cos(glm::radians(input->yaw)) * cos(glm::radians(input->pitch));
    front.y = sin(glm::radians(input->pitch));
    front.z = sin(glm::radians(input->yaw)) * cos(glm::radians(input->pitch));
    cameraFront = glm::normalize(front);
}

void Engine::render() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    lightPos.x = 1.0f + sin(glfwGetTime()) * 2.0f;
    lightPos.z = sin(glfwGetTime() / 2.0f) * 1.0f;
    glm::mat4 projection = glm::perspective(glm::radians(input->fov), (float)width / height, 0.1f, 100.0f);
    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

    // Objects rendering
    lightingShader->use();
    lightingShader->setVec3("lightColor",  glm::vec3(1.0f, 1.0f, 1.0f));
    lightingShader->setVec3("lightPos", lightPos);
    lightingShader->setVec3("viewPos", cameraPos);

    lightingShader->setMat4("projection", projection);
    lightingShader->setMat4("view", view);

    for (const auto& shape : shapes) {
        lightingShader->setVec3("objectColor", shape->getColor());
        shape->draw(*lightingShader);
    }

    // Lamp rendering
    lampShader->use();
    lampShader->setMat4("projection", projection);
    lampShader->setMat4("view", view);

    lightCube->setPosition(lightPos);
    lightCube->draw(*lampShader);
}

// Callbacks
void Engine::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void Engine::mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
    if (engine) engine->input->handleMouse(xpos, ypos);
}

void Engine::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
    if (engine) engine->input->handleScroll(xoffset, yoffset);
}