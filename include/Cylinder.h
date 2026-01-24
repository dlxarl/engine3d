#pragma once
#include "Shape.h"

class Cylinder : public Shape {
public:
    Cylinder(float radius = 0.5f, float height = 1.0f, int segments = 32);
    void draw(Shader& shader) override;

private:
    void build(float radius, float height, int segments);
};
