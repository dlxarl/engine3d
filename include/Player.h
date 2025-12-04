#pragma once
#include "Cube.h"
#include <GLFW/glfw3.h>
#include <vector>
#include <memory>

class Player : public Cube {
public:
    Player(glm::vec3 startPos);

    void update(float deltaTime, const std::vector<std::unique_ptr<Shape>>& worldObjects);
    void move(glm::vec3 direction);
    void jump();

    glm::vec3 getCameraPosition() const;

    float runSpeed = 6.0f;

private:
    float walkSpeed = 3.0f;
    float crouchSpeed = 1.5f;

    // ФІЗИКА
    float gravity = -19.6f;
    float jumpHeight = 2.0f;

    bool isGrounded;
    bool isCrouching;

    float standHeight = 2.0f;
    float crouchHeight = 1.0f;
    float playerWidth = 0.6f;

    void applyGravity(float deltaTime);
    void checkCollisions(const std::vector<std::unique_ptr<Shape>>& worldObjects, glm::vec3 velocityStep);
};