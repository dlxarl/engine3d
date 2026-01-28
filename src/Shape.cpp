#include "Shape.h"

Shape::Shape() {
    model = glm::mat4(1.0f);
    color = glm::vec3(1.0f);

    position = glm::vec3(0.0f);
    velocity = glm::vec3(0.0f);
    scale = glm::vec3(1.0f);
    rotation = glm::vec3(0.0f);

    useGravity = false;
    isStatic = false;
    hasCollision = true;

    VAO = 0;
    VBO = 0;
}

Shape::~Shape() {
    if (VAO != 0) glDeleteVertexArrays(1, &VAO);
    if (VBO != 0) glDeleteBuffers(1, &VBO);
}

void Shape::setPosition(glm::vec3 pos) {
    position = pos;
    updateModelMatrix();
}

void Shape::rotate(float angle, glm::vec3 axis) {
    rotation += axis * angle;
    updateModelMatrix();
}

void Shape::setScale(glm::vec3 scaleVec) {
    scale = scaleVec;
    updateModelMatrix();
}

void Shape::updateModelMatrix() {
    model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, scale);
}

void Shape::setColor(glm::vec3 newColor) {
    color = newColor;
}

glm::vec3 Shape::getColor() const {
    return color;
}

void Shape::addTexture(std::shared_ptr<Texture> tex) {
    textures.push_back(tex);
}

bool Shape::checkCollision(Shape& other) {
    float halfX = 0.5f * scale.x;
    float halfY = 0.5f * scale.y;
    float halfZ = 0.5f * scale.z;

    float otherHalfX = 0.5f * other.scale.x;
    float otherHalfY = 0.5f * other.scale.y;
    float otherHalfZ = 0.5f * other.scale.z;

    bool collisionX = position.x + halfX >= other.position.x - otherHalfX &&
                      other.position.x + otherHalfX >= position.x - halfX;

    bool collisionY = position.y + halfY >= other.position.y - otherHalfY &&
                      other.position.y + otherHalfY >= position.y - halfY;

    bool collisionZ = position.z + halfZ >= other.position.z - otherHalfZ &&
                      other.position.z + otherHalfZ >= position.z - halfZ;

    return collisionX && collisionY && collisionZ;
}