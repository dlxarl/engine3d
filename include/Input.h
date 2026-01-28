#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <unordered_map>

class Input {
public:
    Input();

    void update(GLFWwindow* window);

    bool isKeyDown(int key);        // тримається
    bool isKeyPressed(int key);     // натиснуто один раз
    bool isKeyReleased(int key);    // відпущено один раз

    void handleMouse(double xpos, double ypos);
    void handleScroll(double xoffset, double yoffset);

    float yaw;
    float pitch;
    float fov;

private:
    std::unordered_map<int, bool> currentKeys;
    std::unordered_map<int, bool> previousKeys;

    float lastX;
    float lastY;
    bool firstMouse;
};

extern Input* GInput;