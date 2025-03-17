#pragma once
#include <vector>

struct Color {
    float r, g, b;
};

extern std::vector<Color> columnColors;

void initializeColumnColors();