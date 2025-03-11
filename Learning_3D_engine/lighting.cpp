#include <Windows.h>
#include "lighting.h"
#include <GL/gl.h>

void initializeLighting() {
    GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
    GLfloat light_ambient[] = { 0.5, 0.5, 0.5, 1.0 }; // Zvýšeno z 0.2 na 0.5
    GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 }; // Zvýšeno z 0.8 na 1.0

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
}

void setLightingPosition() {
    GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
}