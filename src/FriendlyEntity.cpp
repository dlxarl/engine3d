#include "FriendlyEntity.h"
// #include <glm/gtc/random.hpp> // Removed - no longer using random functions
#include <iostream>

FriendlyEntity::FriendlyEntity(const std::string& modelPath, glm::vec3 startPosition, glm::vec3 entityScale) {
    // Ініціалізуємо базові властивості Entity
    health = 50.0f;
    maxHealth = 50.0f;
    moveSpeed = 1.5f;
    position = startPosition;
    isAlive = true;

    // Створюємо візуальне представлення (3D модель)
    visualShape = std::make_shared<Model>(modelPath);
    visualShape->setPosition(position);
    visualShape->useGravity = false;
    visualShape->hasCollision = true;

    // Встановлюємо масштаб
    setScale(entityScale);

    // Додаємо текстуру для кота
    if (modelPath.find("cat.obj") != std::string::npos) {
        auto catTexture = std::make_shared<Texture>("assets/textures/cat.jpg", "texture_albedo");
        visualShape->addTexture(catTexture);
    }

    std::cout << "FriendlyEntity created at position: (" 
              << position.x << ", " << position.y << ", " << position.z << ") with scale: ("
              << scale.x << ", " << scale.y << ", " << scale.z << ")" << std::endl;
}

void FriendlyEntity::update(float deltaTime) {
    if (!isAlive) return;

    // Викликаємо базове оновлення
    Entity::update(deltaTime);

    // Поки що не рухаємося
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