#include "EnemyEntity.h"
// #include <glm/gtc/random.hpp> // Removed - no longer using random functions
#include <iostream>

EnemyEntity::EnemyEntity(const std::string& modelPath, glm::vec3 startPosition, glm::vec3 entityScale) {
    // Ініціалізуємо базові властивості Entity
    health = 75.0f;
    maxHealth = 75.0f;
    moveSpeed = 2.0f;
    position = startPosition;
    isAlive = true;

    // Створюємо візуальне представлення (3D модель)
    visualShape = std::make_shared<Model>(modelPath);
    visualShape->setPosition(position);
    visualShape->useGravity = false;
    visualShape->hasCollision = true;

    // Встановлюємо масштаб
    setScale(entityScale);

    std::cout << "EnemyEntity created at position: ("
              << position.x << ", " << position.y << ", " << position.z << ") with scale: ("
              << scale.x << ", " << scale.y << ", " << scale.z << ")" << std::endl;
}

void EnemyEntity::update(float deltaTime) {
    if (!isAlive) return;

    // Викликаємо базове оновлення
    Entity::update(deltaTime);

    // Поки що не рухаємося
}

void EnemyEntity::setTargetPosition(glm::vec3 target) {
    targetPosition = target;
    // hasTarget = true; // Поки не використовується
}

void EnemyEntity::onDamage(float damage) {
    std::cout << "EnemyEntity took " << damage << " damage! Health: " << health << "/" << maxHealth << std::endl;

    // При отриманні шкоди ворог стає агресивним (поки не використовується)
    // hasTarget = true;
}

void EnemyEntity::onDeath() {
    std::cout << "EnemyEntity died!" << std::endl;
    if (visualShape) {
        visualShape->setColor(glm::vec3(0.5f, 0.1f, 0.1f)); // Стати червоним при смерті
    }
}