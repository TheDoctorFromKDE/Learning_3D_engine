#include "light_source.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void drawLightSource(Shader& shaderProgram, GLuint lightVAO, const std::vector<float>& sphereVertices) {
    glBindVertexArray(lightVAO);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 100.0f, 0.0f)); // Pozice světla
    model = glm::scale(model, glm::vec3(2.0f)); // Velikost koule
    shaderProgram.setMat4("model", model);
    shaderProgram.setVec3("objectColor", glm::vec3(1.0f, 1.0f, 0.0f)); // Barva světla (žlutá)
    glDrawArrays(GL_TRIANGLES, 0, sphereVertices.size() / 3);
    glBindVertexArray(0);
}