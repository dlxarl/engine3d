#include "Player.h"
#include <iostream>
#include <cmath>

Player::Player(glm::vec3 startPos) {
    position = startPos;
    scale = glm::vec3(playerWidth, standHeight, playerWidth);

    useGravity = false;
    hasCollision = true;
    isGrounded = false;
    isCrouching = false;
}

void Player::update(float deltaTime, const std::vector<std::unique_ptr<Shape>>& worldObjects) {
    applyGravity(deltaTime);

    float currentYVelocity = velocity.y;
    velocity.y = 0.0f;

    position += velocity * deltaTime;
    updateModelMatrix();
    checkCollisions(worldObjects, velocity * deltaTime);

    velocity.y = currentYVelocity;
    glm::vec3 verticalMove = glm::vec3(0.0f, velocity.y * deltaTime, 0.0f);

    position += verticalMove;
    updateModelMatrix();

    isGrounded = false;
    checkCollisions(worldObjects, verticalMove);
}

void Player::move(glm::vec3 direction) {
    if (glm::length(direction) > 0) {
        float currentY = velocity.y;
        float speed = isCrouching ? crouchSpeed : runSpeed;

        velocity = direction * speed;
        velocity.y = currentY;
    } else {
        float currentY = velocity.y;
        velocity = glm::vec3(0.0f);
        velocity.y = currentY;
    }
}

void Player::jump() {
    if (isGrounded) {
        velocity.y = std::sqrt(2.0f * std::abs(gravity) * jumpHeight);
        isGrounded = false;
    }
}

void Player::applyGravity(float deltaTime) {
    velocity.y += gravity * deltaTime;

    if (velocity.y < -50.0f) velocity.y = -50.0f;
}

void Player::checkCollisions(const std::vector<std::unique_ptr<Shape>>& worldObjects, glm::vec3 velocityStep) {
    for (const auto& obj : worldObjects) {
        if (obj.get() == this) continue;
        if (!obj->hasCollision) continue;

        if (this->checkCollision(*obj)) {
            if (velocityStep.y < 0) {
                isGrounded = true;
                velocity.y = 0.0f;
            }
            else if (velocityStep.y > 0) {
                velocity.y = 0.0f;
            }

            position -= velocityStep;
            updateModelMatrix();
        }
    }
}

glm::vec3 Player::getCameraPosition() const {
    return position + glm::vec3(0.0f, scale.y * 0.4f, 0.0f);
}