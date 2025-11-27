#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Engine {
public:
    Engine();
    ~Engine();

    int init(int width, int height, const char* title);
    void run();

private:
    GLFWwindow* window;
    int width;
    int height;

    float yaw;
    float pitch;
    float fov;

    float lastX;
    float lastY;
    bool firstMouse;

    float deltaTime;
    float lastFrame;

    void processInput();
    void update();
    void render();

    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
};