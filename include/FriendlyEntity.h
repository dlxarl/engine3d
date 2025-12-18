#pragma once
#include "Entity.h"
#include "Model.h"
#include <string>

class FriendlyEntity : public Entity {
public:
    FriendlyEntity(const std::string& modelPath, glm::vec3 startPosition, glm::vec3 entityScale = glm::vec3(0.3f));

    void update(float deltaTime) override;
    void onDamage(float damage) override;
    void onDeath() override;

private:
    // AI variables removed - no movement for now
};