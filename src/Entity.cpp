//
// Created by Ілля on 11/12/2025.
//

#include "Entity.h"
#include <algorithm>

Entity::Entity() :
    health(100.0f),
    maxHealth(100.0f),
    moveSpeed(2.0f),
    isAlive(true),
    position(0.0f),
    velocity(0.0f),
    direction(0.0f, 0.0f, 1.0f), // Напрямлений вперед за замовчуванням
    visualShape(nullptr)
{
}

void Entity::update(float deltaTime) {
    if (!isAlive) return;

    // Оновлюємо позицію на основі швидкості
    position += velocity * deltaTime;

    // Синхронізуємо візуальне представлення з позицією ентіті
    if (visualShape) {
        visualShape->setPosition(position);
    }

    // Зменшуємо швидкість (фрикція)
    velocity *= 0.9f; // Можна налаштувати

    // Обмежуємо мінімальну швидкість
    if (glm::length(velocity) < 0.01f) {
        velocity = glm::vec3(0.0f);
    }
}

void Entity::takeDamage(float damage) {
    if (!isAlive) return;

    health -= damage;
    health = std::max(0.0f, health); // Не допускаємо негативного здоров'я

    onDamage(damage);

    if (health <= 0.0f) {
        die();
    }
}

void Entity::die() {
    if (!isAlive) return;

    isAlive = false;
    velocity = glm::vec3(0.0f); // Зупиняємо рух при смерті

    onDeath();
}

void Entity::move(glm::vec3 moveDirection) {
    if (!isAlive) return;

    // Нормалізуємо напрямок руху
    if (glm::length(moveDirection) > 0.0f) {
        moveDirection = glm::normalize(moveDirection);
        direction = moveDirection; // Оновлюємо напрямок ентіті
    }

    // Додаємо швидкість у напрямку руху
    velocity += moveDirection * moveSpeed;
}