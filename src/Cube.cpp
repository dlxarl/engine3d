#include "Cube.h"

Cube::Cube() {
 float vertices[] = {
  -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,
   0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f,
  -0.5f,  0.5f, -0.5f, -0.5f, -0.5f, -0.5f,

  -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,
   0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  0.5f,
  -0.5f,  0.5f,  0.5f, -0.5f, -0.5f,  0.5f,

  -0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f,
  -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f,
  -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,  0.5f,

   0.5f,  0.5f,  0.5f,  0.5f,  0.5f, -0.5f,
   0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,
   0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,

  -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,
   0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,
  -0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f,

  -0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f,
   0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  0.5f,
  -0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f
};

 glGenVertexArrays(1, &VAO);
 glGenBuffers(1, &VBO);

 glBindVertexArray(VAO);

 glBindBuffer(GL_ARRAY_BUFFER, VBO);
 glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

 glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
 glEnableVertexAttribArray(0);

 glBindBuffer(GL_ARRAY_BUFFER, 0);
 glBindVertexArray(0);
}

void Cube::draw(Shader& shader) {
 shader.setMat4("model", model);

 glBindVertexArray(VAO);
 glDrawArrays(GL_TRIANGLES, 0, 36);
 glBindVertexArray(0);
}