#pragma once
#include "Shape.h"
#include <string>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Model : public Shape {
public:
    Model(const std::string& path);
    void draw(Shader& shader) override;

private:
    struct MeshData {
        unsigned int VAO, VBO, EBO;
        unsigned int indexCount;
    };
    std::vector<MeshData> meshes;

    void processNode(aiNode *node, const aiScene *scene);
    void processMesh(aiMesh *mesh, const aiScene *scene);
};