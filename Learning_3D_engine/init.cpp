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
#include "shader_program.h"
#include "init.h"
#include "camera.h"
#include "lighting.h"
#include "colors.h"
#include "globals.h"

extern int windowWidth;
extern int windowHeight;
extern GLuint VAO, VBO, NBO, EBO;
extern GLuint depthMapFBO, depthMap;
extern Shader* shaderProgram;
extern Shader* depthShader;
extern bool isMouseCaptured; // Přidání externí deklarace
extern int centerX, centerY; // Přidání externí deklarace

void initialize() {
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Error initializing GLEW: " << glewGetErrorString(err) << std::endl;
        exit(1);
    }

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, static_cast<double>(windowWidth) / windowHeight, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);

    glClearColor(0.5, 0.5, 0.5, 1.0);
    glEnable(GL_DEPTH_TEST);

    glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
    glm::vec3 objectColor(1.0f, 0.5f, 0.31f);

    std::cerr << "Light color: " << lightColor.r << ", " << lightColor.g << ", " << lightColor.b << std::endl;
    std::cerr << "Object color: " << objectColor.r << ", " << objectColor.g << ", " << objectColor.b << std::endl;

    shaderProgram = new Shader("shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl");
    depthShader = new Shader("shaders/depth_vertex_shader.glsl", "shaders/depth_fragment_shader.glsl");

    shaderProgram->use();
    shaderProgram->setVec3("lightColor", lightColor);
    shaderProgram->setVec3("objectColor", objectColor);

    srand(static_cast<unsigned int>(time(0)));
    initializeColumnColors();

    GLfloat vertices[] = {
        -0.5f, 0.0f,  0.5f,
         0.5f, 0.0f,  0.5f,
         0.5f, 2.0f,  0.5f,
        -0.5f, 2.0f,  0.5f,
        -0.5f, 0.0f, -0.5f,
         0.5f, 0.0f, -0.5f,
         0.5f, 2.0f, -0.5f,
        -0.5f, 2.0f, -0.5f,
        -0.5f, 0.0f, -0.5f,
        -0.5f, 0.0f,  0.5f,
        -0.5f, 2.0f,  0.5f,
        -0.5f, 2.0f, -0.5f,
         0.5f, 0.0f, -0.5f,
         0.5f, 0.0f,  0.5f,
         0.5f, 2.0f,  0.5f,
         0.5f, 2.0f, -0.5f,
         -0.5f, 2.0f, -0.5f,
          0.5f, 2.0f, -0.5f,
          0.5f, 2.0f,  0.5f,
         -0.5f, 2.0f,  0.5f,
         -50.0f, 0.0f, -50.0f,
          50.0f, 0.0f, -50.0f,
          50.0f, 0.0f,  50.0f,
         -50.0f, 0.0f,  50.0f
    };

    GLfloat normals[] = {
        0.0f,  0.0f,  1.0f,
        0.0f,  0.0f,  1.0f,
        0.0f,  0.0f,  1.0f,
        0.0f,  0.0f,  1.0f,
        0.0f,  0.0f, -1.0f,
        0.0f,  0.0f, -1.0f,
        0.0f,  0.0f, -1.0f,
        0.0f,  0.0f, -1.0f,
       -1.0f,  0.0f,  0.0f,
       -1.0f,  0.0f,  0.0f,
       -1.0f,  0.0f,  0.0f,
       -1.0f,  0.0f,  0.0f,
       1.0f,  0.0f,  0.0f,
       1.0f,  0.0f,  0.0f,
       1.0f,  0.0f,  0.0f,
       1.0f,  0.0f,  0.0f,
       0.0f,  1.0f,  0.0f,
       0.0f,  1.0f,  0.0f,
       0.0f,  1.0f,  0.0f,
       0.0f,  1.0f,  0.0f,
       0.0f,  1.0f,  0.0f,
       0.0f,  1.0f,  0.0f,
       0.0f,  1.0f,  0.0f,
       0.0f,  1.0f,  0.0f
    };

    GLuint indices[] = {
        0,  1,  2,
        2,  3,  0,
        4,  5,  6,
        6,  7,  4,
        8,  9,  10,
        10,  11, 8,
        12, 13, 14,
        14, 15, 12,
        16, 17, 18,
        18, 19, 16,
        20, 21, 22,
        22, 23, 20
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &NBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, NBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindVertexArray(0);

    glGenFramebuffers(1, &depthMapFBO);
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    GLfloat borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glutSetCursor(GLUT_CURSOR_NONE);
    RECT rect;
    GetClientRect(GetForegroundWindow(), &rect);
    MapWindowPoints(GetForegroundWindow(), nullptr, (POINT*)&rect, 2);
    ClipCursor(&rect);
    SetCapture(GetForegroundWindow());
    isMouseCaptured = true;

    centerX = windowWidth / 2;
    centerY = windowHeight / 2;
    glutWarpPointer(centerX, centerY);
}