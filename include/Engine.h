#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include "Input.h"
#include "Shader.h"
#include "Scene.h"

class Engine {
public:
    Engine();
    ~Engine();

    int init(int width, int height, const char* title);
    void run();

    void setScene(std::shared_ptr<Scene> scene);

private:
    GLFWwindow* window;
    int width, height;

    std::unique_ptr<Input> input;

    std::unique_ptr<Shader> lightingShader;
    std::unique_ptr<Shader> lampShader;

    std::shared_ptr<Scene> currentScene;

    glm::vec3 cameraPos;
    glm::vec3 cameraFront;
    glm::vec3 cameraUp;

    float deltaTime;
    float lastFrame;

    void processInput();
    void update();
    void render();

    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
};