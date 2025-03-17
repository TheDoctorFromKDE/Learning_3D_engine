#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <GL/gl.h>
#include "shader_program.h"
#include "colors.h"
#include "render.h"

extern Shader* shaderProgram;
extern GLuint VAO;

void drawColumn(float x, float z, const Color& color) {
    shaderProgram->setVec3("objectColor", glm::vec3(color.r, color.g, color.b));
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.0f, z));
    shaderProgram->setMat4("model", model);
    glDrawElements(GL_TRIANGLES, 30, GL_UNSIGNED_INT, 0);
}

void renderScene(Shader& shader) {
    glm::mat4 model = glm::mat4(1.0f);
    shader.setMat4("model", model);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(30 * sizeof(GLuint)));

    int index = 0;
    for (int i = -5; i <= 5; ++i) {
        for (int j = -5; j <= 5; ++j) {
            model = glm::translate(glm::mat4(1.0f), glm::vec3(i * 5.0f, 0.0f, j * 5.0f));
            shader.setMat4("model", model);
            if (index < columnColors.size()) {
                drawColumn(i * 5.0f, j * 5.0f, columnColors[index++]);
            }
            else {
                std::cerr << "Error: index " << index << " out of range for columnColors vector." << std::endl;
                return;
            }
        }
    }
}