#pragma once
#include "Entity.h"
#include "Model.h"
#include <string>

class EnemyEntity : public Entity {
public:
    EnemyEntity(const std::string& modelPath, glm::vec3 startPosition);

    void update(float deltaTime) override;
    void onDamage(float damage) override;
    void onDeath() override;

    // Агресивна поведінка
    void setTargetPosition(glm::vec3 target);

private:
    glm::vec3 targetPosition;
    float attackRange;
    float detectionRange;
    bool hasTarget;
};