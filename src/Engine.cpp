#include "Engine.h"
#include <iostream>

glm::vec3 cameraPos   = glm::vec3(0.0f, 2.0f,  6.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

Engine::Engine() {
    window = nullptr;
    width = 1920;
    height = 1080;
    input = std::make_unique<Input>();
    deltaTime = 0.0f;
    lastFrame = 0.0f;
}

Engine::~Engine() {
    glfwTerminate();
}

int Engine::init(int width, int height, const char* title) {
    this->width = width;
    this->height = height;

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

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glEnable(GL_DEPTH_TEST);

    lightingShader = std::make_unique<Shader>("src/lighting.vert", "src/lighting.frag");
    lampShader     = std::make_unique<Shader>("src/lighting.vert", "src/lamp.frag");

    return 0;
}

void Engine::setScene(std::shared_ptr<Scene> scene) {
    currentScene = scene;
    if (currentScene) {
        currentScene->load();
    }
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
}

void Engine::update() {
    glm::vec3 front;
    front.x = cos(glm::radians(input->yaw)) * cos(glm::radians(input->pitch));
    front.y = sin(glm::radians(input->pitch));
    front.z = sin(glm::radians(input->yaw)) * cos(glm::radians(input->pitch));
    cameraFront = glm::normalize(front);

    if (currentScene) {
        currentScene->update(deltaTime);
    }
}

void Engine::render() {
    int displayW, displayH;
    glfwGetFramebufferSize(window, &displayW, &displayH);
    glViewport(0, 0, displayW, displayH);
    float aspectRatio = (displayH == 0) ? 1.0f : (float)displayW / (float)displayH;

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (currentScene) {
        glm::mat4 projection = glm::perspective(glm::radians(input->fov), aspectRatio, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        lightingShader->use();
        lightingShader->setMat4("projection", projection);
        lightingShader->setMat4("view", view);
        lightingShader->setVec3("viewPos", cameraPos);

        lampShader->use();
        lampShader->setMat4("projection", projection);
        lampShader->setMat4("view", view);

        currentScene->draw(*lightingShader, *lampShader, view, projection);
    }
}

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