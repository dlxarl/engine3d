#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Input {
public:
    Input();

    float yaw;
    float pitch;
    float fov;

    void handleMouse(double xpos, double ypos);
    void handleScroll(double xoffset, double yoffset);
    bool isKeyPressed(GLFWwindow* window, int key);

private:
    float lastX;
    float lastY;
    bool firstMouse;
};