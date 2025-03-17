#pragma once

#include "shader_program.h"
#include "colors.h"

void drawColumn(float x, float z, const Color& color);
void renderScene(Shader& shader);