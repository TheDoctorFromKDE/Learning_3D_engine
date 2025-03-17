#include <windows.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "camera.h"
#include "lighting.h"
#include "shader_program.h"
#include "init.h"
#include "input.h"
#include "render.h"
#include "colors.h"
#include "globals.h" // Zahrnutí globals.h

int windowWidth = 800;
int windowHeight = 600;
int windowPosX = 100;
int windowPosY = 100;
DWORD lastTime = 0;
bool keys[256];

GLuint VAO, VBO, NBO, EBO;
GLuint lightVAO, lightVBO, lightNBO;
std::vector<float> sphereVertices;
GLuint depthMapFBO, depthMap;
const unsigned int SHADOW_WIDTH = 4096; // Přidání definice
const unsigned int SHADOW_HEIGHT = 4096; // Přidání definice
Shader* shaderProgram;
Shader* depthShader;

void display() {
    ULONGLONG currentTime = GetTickCount64();
    float deltaTime = (currentTime - lastTime) / 1000.0f;
    lastTime = currentTime;

    if (keys['w']) {
        moveCameraForward(speedCamera * deltaTime);
    }
    if (keys['s']) {
        moveCameraBackward(speedCamera * deltaTime);
    }
    if (keys['a']) {
        moveCameraLeft(speedCamera * deltaTime);
    }
    if (keys['d']) {
        moveCameraRight(speedCamera * deltaTime);
    }
    if (keys[' ']) { // Mezerník pro pohyb nahoru
        moveCameraUp(speedCamera * deltaTime);
    }
    if (keys[GLUT_KEY_SHIFT_L] || keys[GLUT_KEY_SHIFT_R]) {
        moveCameraDown(speedCamera * deltaTime);
    }

    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    depthShader->use();
    glm::mat4 lightProjection, lightView;
    glm::mat4 lightSpaceMatrix;
    lightProjection = glm::ortho(-75.0f, 75.0f, -75.0f, 75.0f, 1.0f, 150.0f);
    lightView = glm::lookAt(glm::vec3(0.0f, 50.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
    lightSpaceMatrix = lightProjection * lightView;
    depthShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
    glBindVertexArray(VAO);
    renderScene(*depthShader);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glViewport(0, 0, windowWidth, windowHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shaderProgram->use();
    glm::mat4 view = glm::lookAt(glm::vec3(cameraPosX, cameraPosY, cameraPosZ),
        glm::vec3(cameraPosX + cameraFrontX, cameraPosY + cameraFrontY, cameraPosZ + cameraFrontZ),
        glm::vec3(cameraUpX, cameraUpY, cameraUpZ));
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);
    shaderProgram->setMat4("view", view);
    shaderProgram->setMat4("projection", projection);
    shaderProgram->setMat4("lightSpaceMatrix", lightSpaceMatrix);
    shaderProgram->setVec3("lightPos", glm::vec3(0.0f, 50.0f, 0.0f));
    shaderProgram->setVec3("viewPos", glm::vec3(cameraPosX, cameraPosY, cameraPosZ));

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    shaderProgram->setInt("shadowMap", 1);

    glBindVertexArray(VAO);
    renderScene(*shaderProgram);
    glBindVertexArray(0);

    glutSwapBuffers();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(windowPosX, windowPosY);
    glutCreateWindow("3D Space with Colored Columns");

    initialize();

    glutDisplayFunc(display);
    glutIdleFunc(display);
    glutKeyboardFunc(keyDown);
    glutKeyboardUpFunc(keyUp);
    glutSpecialFunc(specialKeyDown); // Přidání funkce pro speciální klávesy
    glutSpecialUpFunc(specialKeyUp); // Přidání funkce pro uvolnění speciálních kláves
    glutPassiveMotionFunc(passiveMotion);
    glutMotionFunc(processMouseMovementWrapper); // Použití obalové funkce

    lastTime = GetTickCount64();
    memset(keys, 0, sizeof(keys));

    centerX = windowWidth / 2;
    centerY = windowHeight / 2;

    glutMainLoop();

    return 0;
}