//
// Created by Ілля on 11/12/2025.
//

#ifndef INC_3D_ENGINE_ENTITY_H
#define INC_3D_ENGINE_ENTITY_H

#pragma once
#include <glm/glm.hpp>
#include <memory>
#include "Shape.h"

class Entity {
public:
    Entity();
    virtual ~Entity() = default;

    // Основні властивості
    float health;
    float maxHealth;
    float moveSpeed;
    bool isAlive;

    // Позиція та рух
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 direction; // Напрямок руху
    glm::vec3 scale; // Масштаб ентіті

    // Методи життєвого циклу
    virtual void update(float deltaTime);
    virtual void takeDamage(float damage);
    virtual void die();
    virtual void move(glm::vec3 direction);

    // Масштабування
    void setScale(glm::vec3 newScale);

    // Гетери та сетери
    bool isDead() const { return !isAlive; }
    float getHealthPercentage() const { return health / maxHealth; }

    // Візуальне представлення (може бути nullptr для невидимих ентіті)
    std::shared_ptr<Shape> visualShape;

protected:
    virtual void onDeath() {} // Для перевизначення в підкласах
    virtual void onDamage(float damage) {} // Для перевизначення в підкласах
};

#endif //INC_3D_ENGINE_ENTITY_H