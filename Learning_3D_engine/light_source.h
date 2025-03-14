#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "shader_program.h"

void drawLightSource(Shader& shaderProgram, GLuint lightVAO, const std::vector<float>& sphereVertices);