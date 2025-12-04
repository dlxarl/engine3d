#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <memory>
#include <iostream>
#include "Input.h"
#include "Shader.h"
#include "Shape.h"

class Engine {
public:
    Engine();
    ~Engine();

    int init(int width, int height, const char* title);
    void run();

private:
    GLFWwindow* window;
    int width, height;

    std::unique_ptr<Input> input;
    std::unique_ptr<Shader> shader;

    std::unique_ptr<Shader> lightingShader;
    std::unique_ptr<Shader> lampShader;

    std::unique_ptr<Shape> lightCube;
    glm::vec3 lightPos;

    std::vector<std::unique_ptr<Shape>> shapes;

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