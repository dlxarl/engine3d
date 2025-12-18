#pragma once
#include "Entity.h"
#include "Model.h"
#include <string>

class EnemyEntity : public Entity {
public:
    EnemyEntity(const std::string& modelPath, glm::vec3 startPosition, glm::vec3 entityScale = glm::vec3(0.4f));

    void update(float deltaTime) override;
    void onDamage(float damage) override;
    void onDeath() override;

    // Агресивна поведінка (поки не використовується)
    void setTargetPosition(glm::vec3 target);

private:
    glm::vec3 targetPosition;
    // AI variables removed - no movement for now
};