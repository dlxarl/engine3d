#pragma once
#include "Entity.h"
#include "Model.h"
#include <string>

class FriendlyEntity : public Entity {
public:
    FriendlyEntity(const std::string& modelPath, glm::vec3 startPosition);

    void update(float deltaTime) override;
    void onDamage(float damage) override;
    void onDeath() override;

private:
    float wanderTimer;
    glm::vec3 wanderDirection;
    float changeDirectionTime;
};