#include <windows.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "camera.h"
#include "lighting.h"
#include "shader_program.h"
#include "light_source.h"
#include "sphere.h"

int windowWidth = 800;
int windowHeight = 600;
int windowPosX = 100;
int windowPosY = 100;
bool isMouseCaptured = false;
DWORD lastTime = 0;
bool keys[256];
int centerX, centerY;
float speedCamera = 7.0f;

GLuint VAO, VBO, NBO, EBO;
GLuint lightVAO, lightVBO, lightNBO;
std::vector<float> sphereVertices; // Přidáno globální pole vertexů koule
GLuint depthMapFBO, depthMap;
const GLuint SHADOW_WIDTH = 4096, SHADOW_HEIGHT = 4096;
Shader* shaderProgram;
Shader* depthShader;

// Globální proměnné pro barvy sloupů
struct Color {
    float r, g, b;
};

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

void drawColumn(float x, float z, const Color& color) {
    shaderProgram->setVec3("objectColor", glm::vec3(color.r, color.g, color.b));
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.0f, z));
    shaderProgram->setMat4("model", model);
    glDrawElements(GL_TRIANGLES, 30, GL_UNSIGNED_INT, 0);
}

void renderScene(Shader& shader) {
    // Vykreslení země
    glm::mat4 model = glm::mat4(1.0f);
    shader.setMat4("model", model);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(30 * sizeof(GLuint)));

    // Vykreslení sloupů
    int index = 0;
    for (int i = -5; i <= 5; ++i) {
        for (int j = -5; j <= 5; ++j) {
            model = glm::translate(glm::mat4(1.0f), glm::vec3(i * 5.0f, 0.0f, j * 5.0f));
            shader.setMat4("model", model);
            drawColumn(i * 5.0f, j * 5.0f, columnColors[index++]);
        }
    }
}

void display() {
    ULONGLONG currentTime = GetTickCount64();
    float deltaTime = (currentTime - lastTime) / 1000.0f; // Převod na sekundy
    lastTime = currentTime;

    if (keys['w']) {
        moveCameraForward(speedCamera * deltaTime);
    }
    if (keys['s']) {
        moveCameraBackward(speedCamera * deltaTime);
    }
    if (keys['a']) {
        moveCameraLeft(speedCamera * deltaTime);  // Prohozeno moveCameraLeft -> moveCameraRight
    }
    if (keys['d']) {
        moveCameraRight(speedCamera * deltaTime);   // Prohozeno moveCameraRight -> moveCameraLeft
    }

    // První průchod: vykreslení depth mapy
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    depthShader->use();
    glm::mat4 lightProjection, lightView;
    glm::mat4 lightSpaceMatrix;
    lightProjection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, 1.0f, 100.0f);
    lightView = glm::lookAt(glm::vec3(5.0f, 10.0f, 5.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    lightSpaceMatrix = lightProjection * lightView;
    depthShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
    glBindVertexArray(VAO); // Přidáno bindování VAO
    renderScene(*depthShader);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Druhý průchod: vykreslení scény s osvětlením a stíny
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
    shaderProgram->setVec3("lightPos", glm::vec3(5.0f, 10.0f, 5.0f));
    shaderProgram->setVec3("viewPos", glm::vec3(cameraPosX, cameraPosY, cameraPosZ));

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    shaderProgram->setInt("shadowMap", 1);

    glBindVertexArray(VAO); // Přidáno bindování VAO
    renderScene(*shaderProgram);
    glBindVertexArray(0);

    glutSwapBuffers();
}

void initialize() {
    glewExperimental = GL_TRUE; // Potřebné pro některé implementace OpenGL
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Error initializing GLEW: " << glewGetErrorString(err) << std::endl;
        exit(1); // Ukončení programu při chybě
    }

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, static_cast<double>(windowWidth) / windowHeight, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);

    glClearColor(0.5, 0.5, 0.5, 1.0); // Šedé pozadí
    glEnable(GL_DEPTH_TEST);

    // Debug výpis pro inicializaci barev a světla
    glm::vec3 lightColor(1.0f, 1.0f, 1.0f); // Bílá barva světla
    glm::vec3 objectColor(1.0f, 0.5f, 0.31f); // Oranžová barva objektů

    std::cerr << "Light color: " << lightColor.r << ", " << lightColor.g << ", " << lightColor.b << std::endl;
    std::cerr << "Object color: " << objectColor.r << ", " << objectColor.g << ", " << objectColor.b << std::endl;

    // Inicializace shader programů
    shaderProgram = new Shader("shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl");
    depthShader = new Shader("shaders/depth_vertex_shader.glsl", "shaders/depth_fragment_shader.glsl");

    // Nastavení barev ve shaderu
    shaderProgram->use();
    shaderProgram->setVec3("lightColor", lightColor);
    shaderProgram->setVec3("objectColor", objectColor);

    // Inicializace náhodných barev sloupů
    srand(static_cast<unsigned int>(time(0)));
    initializeColumnColors();

    // Inicializace VAO, VBO, NBO a EBO
    GLfloat vertices[] = {
        // Přední strana
        -0.5f, 0.0f,  0.5f,
         0.5f, 0.0f,  0.5f,
         0.5f, 2.0f,  0.5f,
        -0.5f, 2.0f,  0.5f,
        // Zadní strana
        -0.5f, 0.0f, -0.5f,
         0.5f, 0.0f, -0.5f,
         0.5f, 2.0f, -0.5f,
        -0.5f, 2.0f, -0.5f,
        // Levá strana
        -0.5f, 0.0f, -0.5f,
        -0.5f, 0.0f,  0.5f,
        -0.5f, 2.0f,  0.5f,
        -0.5f, 2.0f, -0.5f,
        // Pravá strana
         0.5f, 0.0f, -0.5f,
         0.5f, 0.0f,  0.5f,
         0.5f, 2.0f,  0.5f,
         0.5f, 2.0f, -0.5f,
         // Horní strana
         -0.5f, 2.0f, -0.5f,
          0.5f, 2.0f, -0.5f,
          0.5f, 2.0f,  0.5f,
         -0.5f, 2.0f,  0.5f,

         // Zem
         -50.0f, 0.0f, -50.0f,
          50.0f, 0.0f, -50.0f,
          50.0f, 0.0f,  50.0f,
         -50.0f, 0.0f,  50.0f
    };

    GLfloat normals[] = {
        // Přední strana
        0.0f,  0.0f,  1.0f,
        0.0f,  0.0f,  1.0f,
        0.0f,  0.0f,  1.0f,
        0.0f,  0.0f,  1.0f,
        // Zadní strana
        0.0f,  0.0f, -1.0f,
        0.0f,  0.0f, -1.0f,
        0.0f,  0.0f, -1.0f,
        0.0f,  0.0f, -1.0f,
        // Levá strana
       -1.0f,  0.0f,  0.0f,
       -1.0f,  0.0f,  0.0f,
       -1.0f,  0.0f,  0.0f,
       -1.0f,  0.0f,  0.0f,
       // Pravá strana
       1.0f,  0.0f,  0.0f,
       1.0f,  0.0f,  0.0f,
       1.0f,  0.0f,  0.0f,
       1.0f,  0.0f,  0.0f,
       // Horní strana
       0.0f,  1.0f,  0.0f,
       0.0f,  1.0f,  0.0f,
       0.0f,  1.0f,  0.0f,
       0.0f,  1.0f,  0.0f,

       // Zem
       0.0f,  1.0f,  0.0f,
       0.0f,  1.0f,  0.0f,
       0.0f,  1.0f,  0.0f,
       0.0f,  1.0f,  0.0f
    };

    GLuint indices[] = {
        // Přední strana
        0,  1,  2,
        2,  3,  0,
        // Zadní strana
        4,  5,  6,
        6,  7,  4,
        // Levá strana
        8,  9,  10,
        10, 11, 8,
        // Pravá strana
        12, 13, 14,
        14, 15, 12,
        // Horní strana
        16, 17, 18,
        18, 19, 16,

        // Zem
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

    // Vytvoření framebufferu a textury pro shadow mapu
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

    // Skrytí kurzoru a nastavení omezení pohybu kurzoru
    glutSetCursor(GLUT_CURSOR_NONE);
    RECT rect;
    GetClientRect(GetForegroundWindow(), &rect);
    MapWindowPoints(GetForegroundWindow(), nullptr, (POINT*)&rect, 2);
    ClipCursor(&rect);
    SetCapture(GetForegroundWindow());
    isMouseCaptured = true;

    // Nastavení pozice myši na střed okna
    centerX = windowWidth / 2;
    centerY = windowHeight / 2;
    glutWarpPointer(centerX, centerY);
}

void keyDown(unsigned char key, int x, int y) {
    keys[key] = true;

    if (key == 27) { // ESC key
        if (isMouseCaptured) {
            ClipCursor(NULL);
            ReleaseCapture();
            glutSetCursor(GLUT_CURSOR_INHERIT); // Zobrazení kurzoru
            isMouseCaptured = false;
        }
        else {
            RECT rect;
            GetClientRect(GetForegroundWindow(), &rect);
            MapWindowPoints(GetForegroundWindow(), nullptr, (POINT*)&rect, 2);
            ClipCursor(&rect);
            SetCapture(GetForegroundWindow());
            glutSetCursor(GLUT_CURSOR_NONE); // Skrytí kurzoru
            isMouseCaptured = true;

            // Nastavení pozice myši na střed okna
            glutWarpPointer(centerX, centerY);
        }
    }

    // Klávesová zkratka pro náhodné změny barev
    if (key == 'r') {
        initializeColumnColors();
        std::cout << "Colors randomized" << std::endl;
    }
}

void keyUp(unsigned char key, int x, int y) {
    keys[key] = false;
}

void passiveMotion(int x, int y) {
    if (isMouseCaptured && (x != centerX || y != centerY)) {
        int dx = x - centerX;
        int dy = y - centerY;

        processMouseMovement(dx, dy);

        // Nastavení pozice myši zpět na střed okna
        glutWarpPointer(centerX, centerY);
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(windowPosX, windowPosY);
    glutCreateWindow("3D Space with Colored Columns");

    initialize();

    glutDisplayFunc(display);
    glutIdleFunc(display); // Přidáno pro kontinuální aktualizaci
    glutKeyboardFunc(keyDown);
    glutKeyboardUpFunc(keyUp);
    glutPassiveMotionFunc(passiveMotion); // Pro pohyb myší

    lastTime = GetTickCount64(); // Inicializace času
    memset(keys, 0, sizeof(keys)); // Inicializace pole kláves

    glutMainLoop();

    return 0;
}