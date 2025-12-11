#include "EnemyEntity.h"
#include <glm/gtc/random.hpp>
#include <iostream>

EnemyEntity::EnemyEntity(const std::string& modelPath, glm::vec3 startPosition) {
    // Ініціалізуємо базові властивості Entity
    health = 75.0f;
    maxHealth = 75.0f;
    moveSpeed = 2.0f;
    position = startPosition;
    isAlive = true;

    // Створюємо візуальне представлення (3D модель)
    visualShape = std::make_shared<Model>(modelPath);
    visualShape->setPosition(position);
    visualShape->setScale(glm::vec3(0.4f)); // Трохи більший розмір для ворогів
    visualShape->useGravity = false;
    visualShape->hasCollision = true;

    // Ініціалізуємо AI змінні
    targetPosition = position;
    attackRange = 1.5f;
    detectionRange = 8.0f;
    hasTarget = false;

    std::cout << "EnemyEntity created at position: ("
              << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
}

void EnemyEntity::update(float deltaTime) {
    if (!isAlive) return;

    // Викликаємо базове оновлення
    Entity::update(deltaTime);

    // AI поведінка: переслідування цілі або патрулювання
    if (hasTarget) {
        glm::vec3 toTarget = targetPosition - position;
        float distanceToTarget = glm::length(toTarget);

        if (distanceToTarget > attackRange) {
            // Рухаємося до цілі
            glm::vec3 moveDir = glm::normalize(toTarget);
            move(moveDir);
        } else {
            // В межах зони атаки - зупиняємося
            velocity = glm::vec3(0.0f);
        }
    } else {
        // Патрулювання - випадковий рух
        if (glm::length(velocity) < 0.1f) {
            // Якщо стоїмо, вибираємо новий напрямок
            glm::vec3 randomDir = glm::normalize(glm::vec3(
                glm::linearRand(-1.0f, 1.0f),
                0.0f,
                glm::linearRand(-1.0f, 1.0f)
            ));
            move(randomDir * 0.3f);
        }
    }
}

void EnemyEntity::setTargetPosition(glm::vec3 target) {
    targetPosition = target;
    hasTarget = true;
}

void EnemyEntity::onDamage(float damage) {
    std::cout << "EnemyEntity took " << damage << " damage! Health: " << health << "/" << maxHealth << std::endl;

    // При отриманні шкоди ворог стає агресивним
    hasTarget = true;
}

void EnemyEntity::onDeath() {
    std::cout << "EnemyEntity died!" << std::endl;
    if (visualShape) {
        visualShape->setColor(glm::vec3(0.5f, 0.1f, 0.1f)); // Стати червоним при смерті
    }
}