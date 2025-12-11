#include "FriendlyEntity.h"
#include <glm/gtc/random.hpp>
#include <iostream>

FriendlyEntity::FriendlyEntity(const std::string& modelPath, glm::vec3 startPosition) {
    // Ініціалізуємо базові властивості Entity
    health = 50.0f;
    maxHealth = 50.0f;
    moveSpeed = 1.5f;
    position = startPosition;
    isAlive = true;

    // Створюємо візуальне представлення (3D модель)
    visualShape = std::make_shared<Model>(modelPath);
    visualShape->setPosition(position);
    visualShape->setScale(glm::vec3(0.3f)); // Менший розмір для дружніх ентіті
    visualShape->useGravity = false;
    visualShape->hasCollision = true;

    // Ініціалізуємо AI змінні
    wanderTimer = 0.0f;
    changeDirectionTime = glm::linearRand(2.0f, 5.0f);
    wanderDirection = glm::normalize(glm::vec3(
        glm::linearRand(-1.0f, 1.0f),
        0.0f,
        glm::linearRand(-1.0f, 1.0f)
    ));

    std::cout << "FriendlyEntity created at position: (" 
              << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
}

void FriendlyEntity::update(float deltaTime) {
    if (!isAlive) return;

    // Викликаємо базове оновлення
    Entity::update(deltaTime);

    // AI поведінка: випадковий рух
    wanderTimer += deltaTime;

    if (wanderTimer >= changeDirectionTime) {
        // Змінюємо напрямок руху
        wanderDirection = glm::normalize(glm::vec3(
            glm::linearRand(-1.0f, 1.0f),
            0.0f,
            glm::linearRand(-1.0f, 1.0f)
        ));
        changeDirectionTime = glm::linearRand(2.0f, 5.0f);
        wanderTimer = 0.0f;
    }

    // Рухаємося у поточному напрямку
    move(wanderDirection * 0.5f); // Повільний рух
}

void FriendlyEntity::onDamage(float damage) {
    std::cout << "FriendlyEntity took " << damage << " damage! Health: " << health << "/" << maxHealth << std::endl;
}

void FriendlyEntity::onDeath() {
    std::cout << "FriendlyEntity died!" << std::endl;
    if (visualShape) {
        visualShape->setColor(glm::vec3(0.3f, 0.3f, 0.3f)); // Стати сірим при смерті
    }
}