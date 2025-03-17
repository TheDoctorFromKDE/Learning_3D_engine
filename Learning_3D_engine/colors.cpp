#include <vector>
#include <cstdlib>
#include "colors.h"

std::vector<Color> columnColors;

void initializeColumnColors() {
    columnColors.clear();
    for (int i = 0; i < 11; ++i) {
        for (int j = 0; j < 11; ++j) {
            Color color = {
                static_cast<float>(rand()) / static_cast<float>(RAND_MAX),
                static_cast<float>(rand()) / static_cast<float>(RAND_MAX),
                static_cast<float>(rand()) / static_cast<float>(RAND_MAX)
            };
            columnColors.push_back(color);
        }
    }
}