#include "Mesh.hpp"
#include "LoadDDS.hpp"
#include "LoadObj.hpp"

#include <btBulletDynamicsCommon.h>

#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <memory>
#include <vector>

static GLuint g_vertexArrayId;
static GLuint g_elementBufferId;
static GLuint g_textureId;
static GLuint g_lightPositionId;
static GLuint g_lightColorId;
static GLuint g_lightPowerId;
static GLuint g_uniformTextureId;
static GLuint g_programId;
static GLuint g_mvpMatrixId;
static GLuint g_modelMatrixId;
static GLuint g_viewMatrixId;
static GLFWwindow *g_window;
static Mesh g_model;
static glm::vec3 g_position { 3, 1, 9 };
static glm::vec3 g_direction;
static glm::vec3 g_right;
static glm::vec3 g_up;
static float g_horizontalAngle { 3.14f };
static float g_verticalAngle { 0.0f };
static const float SPEED { 5.0f };
static const float MOUSE_SPEED { 0.02f };

static void setupOpenGL();
static void cleanupOpenGL();
static void loadShaders(const std::string &vertexShaderFile, const std::string &fragmentShaderFile);
static void compileShader(GLuint shaderId, const std::string sourceFile);
static void processInputs();
static void showStatus(double boxHeight, double fps);

int
main()
{
    setupOpenGL();

    std::unique_ptr<btBroadphaseInterface> broadphase { new btDbvtBroadphase };
    std::unique_ptr<btDefaultCollisionConfiguration> collisionConfiguration {
        new btDefaultCollisionConfiguration
    };
    std::unique_ptr<btCollisionDispatcher> dispatcher { new btCollisionDispatcher {
        collisionConfiguration.get()
    } };
    std::unique_ptr<btSequentialImpulseConstraintSolver> solver {
        new btSequentialImpulseConstraintSolver
    };
    std::unique_ptr<btDiscreteDynamicsWorld> dynamicsWorld { new btDiscreteDynamicsWorld {
        dispatcher.get(), broadphase.get(), solver.get(), collisionConfiguration.get()
    } };
    dynamicsWorld->setGravity(btVector3 { 0, -10, 0 });
    std::unique_ptr<btCollisionShape> groundShape {
        new btStaticPlaneShape { btVector3 { 0, 1, 0 }, 1 }
    };
    std::unique_ptr<btSphereShape> fallShape { new btSphereShape { 1 } };
    std::unique_ptr<btDefaultMotionState> groundMotionState { new btDefaultMotionState {
        btTransform { btQuaternion { 0, 0, 0, 1 }, btVector3 { 0, -1, 0 } }
    } };
    btRigidBody::btRigidBodyConstructionInfo groundRigidBodyConstructionInfo {
        0, groundMotionState.get(), groundShape.get(), btVector3 { 0, 0, 0 }
    };
    std::unique_ptr<btRigidBody> groundRigidBody { new btRigidBody {
        groundRigidBodyConstructionInfo
    } };

    dynamicsWorld->addRigidBody(groundRigidBody.get());

    std::unique_ptr<btDefaultMotionState> fallMotionState { new btDefaultMotionState {
        btTransform { btQuaternion { 0, 0, 0, 1 }, btVector3 { 0, 50, 0 } }
    } };
    btScalar mass { 1 };
    btVector3 fallInertia { 0, 0, 0 };

    fallShape->calculateLocalInertia(mass, fallInertia);

    btRigidBody::btRigidBodyConstructionInfo fallRigidBodyConstructionInfo {
        mass, fallMotionState.get(), fallShape.get(), fallInertia
    };
    std::unique_ptr<btRigidBody> fallRigidBody { new btRigidBody {
        fallRigidBodyConstructionInfo
    } };

    dynamicsWorld->addRigidBody(fallRigidBody.get());

    glm::mat4 projectionMatrix = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
    glm::mat4 viewMatrix;
    glm::mat4 modelMatrix;
    glm::mat4 mvp;
    glm::vec3 lightPosition { 3, 3, 3 };
    glm::vec3 lightColor { 1, 1, 1 };
    float lightPower { 50.0f };

    btTransform transformation;

    double currentTime, lastTime = glfwGetTime();
    double fps { 0.0 };
    size_t nFrames { 0l };

    do {
        dynamicsWorld->stepSimulation(1 / 60.0f, 10);
        fallRigidBody->getMotionState()->getWorldTransform(transformation);

        processInputs();
        modelMatrix = glm::translate(glm::mat4 { 1.0f }, glm::vec3 {
            transformation.getOrigin().getX(),
            transformation.getOrigin().getY(),
            transformation.getOrigin().getZ()
        });
        viewMatrix = glm::lookAt(
            g_position,
            g_position + g_direction,
            g_up
        );
        mvp = projectionMatrix * viewMatrix * modelMatrix;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(g_programId);
        glUniformMatrix4fv(g_mvpMatrixId, 1, GL_FALSE, &mvp[0][0]);
        glUniformMatrix4fv(g_modelMatrixId, 1, GL_FALSE, &modelMatrix[0][0]);
        glUniformMatrix4fv(g_viewMatrixId, 1, GL_FALSE, &viewMatrix[0][0]);
        glUniform3f(g_lightPositionId, lightPosition.x, lightPosition.y, lightPosition.z);
        glUniform3f(g_lightColorId, lightColor.x, lightColor.y, lightColor.z);
        glUniform1f(g_lightPowerId, lightPower);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, g_textureId);
        glUniform1i(g_uniformTextureId, 0);

        g_model.beginDraw();

        glDrawArrays(GL_TRIANGLES, 0, g_model.vertices().size());
        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_elementBufferId);
        // glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, nullptr);

        g_model.endDraw();

        currentTime = glfwGetTime();
        ++nFrames;
        if (currentTime - lastTime > 1.0) {
            fps = 1000.0/nFrames;
            nFrames = 0;
            lastTime += 1.0;
        }
        showStatus(transformation.getOrigin().getY(), fps);

        glfwSwapBuffers(g_window);
        glfwPollEvents();
    } while (glfwGetKey(g_window, GLFW_KEY_ESCAPE) != GLFW_PRESS
             && glfwGetKey(g_window, GLFW_KEY_Q) != GLFW_PRESS
             && glfwWindowShouldClose(g_window) == 0);
    dynamicsWorld->removeRigidBody(fallRigidBody.get());
    dynamicsWorld->removeRigidBody(groundRigidBody.get());
    cleanupOpenGL();

    return 0;
}

/*
 * Setup OpenGL
 *
 * Set up GLFW and GLEW, opens a window.
 */
void
setupOpenGL()
{
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW." << std::endl;
        exit(1);
    }
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    g_window = glfwCreateWindow(1024, 768, "Bullet + OpenGL", nullptr, nullptr);

    if (g_window == nullptr) {
        std::cerr << "Failed to open GLFW window." << std::endl;
        glfwTerminate();
        exit(1);
    }
    glfwMakeContextCurrent(g_window);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW." << std::endl;
        glfwTerminate();
        exit(1);
    }
    glfwSetInputMode(g_window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetCursorPos(g_window, 1024/2, 768/2);

    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);

    glGenVertexArrays(1, &g_vertexArrayId);
    glBindVertexArray(g_vertexArrayId);

    g_model = loadModelFromObjFile("../res/textures/cube.obj");

    g_model.setup();

    std::vector<unsigned short> indices;

    glGenBuffers(1, &g_elementBufferId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_elementBufferId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0],
                 GL_STATIC_DRAW);

    loadShaders("../res/shaders/vertex.glsl", "../res/shaders/fragment.glsl");
    g_mvpMatrixId = glGetUniformLocation(g_programId, "mvpMatrix");
    g_modelMatrixId = glGetUniformLocation(g_programId, "modelMatris");
    g_viewMatrixId = glGetUniformLocation(g_programId, "modelMatrix");

    g_textureId = loadDDS("../res/textures/uvmap.DDS");
    g_uniformTextureId = glGetUniformLocation(g_programId, "myTextureSampler");

    glUseProgram(g_programId);
    g_lightPositionId = glGetUniformLocation(g_programId, "lightPosition_worldspace");
    g_lightColorId = glGetUniformLocation(g_programId, "lightColor");
    g_lightPowerId = glGetUniformLocation(g_programId, "lightPower");
}

static void
cleanupOpenGL()
{
    glDeleteProgram(g_programId);
    glDeleteVertexArrays(1, &g_vertexArrayId);
    glfwTerminate();
}

static void
loadShaders(const std::string &vertexShaderFile, const std::string &fragmentShaderFile)
{
    g_programId = glCreateProgram();
    GLuint vsId = glCreateShader(GL_VERTEX_SHADER);
    GLuint fsId = glCreateShader(GL_FRAGMENT_SHADER);
    GLint result = GL_FALSE;
    GLint infoLogLength;

    compileShader(vsId, vertexShaderFile);
    compileShader(fsId, fragmentShaderFile);
    glAttachShader(g_programId, vsId);
    glAttachShader(g_programId, fsId);
    glLinkProgram(g_programId);
    glGetProgramiv(g_programId, GL_LINK_STATUS, &result);
    glGetProgramiv(g_programId, GL_INFO_LOG_LENGTH, &infoLogLength);

    std::vector<char> infoLog(infoLogLength);

    glGetProgramInfoLog(g_programId, infoLogLength, nullptr, &infoLog[0]);
    std::cerr << &infoLog[0] << std::endl;
    glDeleteShader(vsId);
    glDeleteShader(fsId);
}

static void
compileShader(GLuint shaderId, const std::string sourceFile)
{
    std::string shaderSrc;
    std::ifstream file { sourceFile };

    if (file.is_open()) {
        std::string ln;

        while (std::getline(file, ln))
            shaderSrc += "\n" + ln;
    } else {
        std::cerr << "Warning: failed to open vertex shader source file." << std::endl;
        exit(1);
    }

    GLint result = GL_FALSE;
    GLint infoLogLength;
    char const *shaderSrcPtr = shaderSrc.c_str();

    glShaderSource(shaderId, 1, &shaderSrcPtr, nullptr);
    glCompileShader(shaderId);
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &result);
    glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &infoLogLength);

    std::vector<char> infoLog(infoLogLength);

    glGetShaderInfoLog(shaderId, infoLogLength, nullptr, &infoLog[0]);
    std::cerr << &infoLog[0] << std::endl;
}

static void
processInputs()
{
    static double lastTime = glfwGetTime();
    double now = glfwGetTime();
    float deltaTime = float(now - lastTime);
    double xPos, yPos;

    lastTime = now;
    glfwGetCursorPos(g_window, &xPos, &yPos);
    // TODO: glfwGetWindowSize
    glfwSetCursorPos(g_window, 1024/2, 768/2);
    g_horizontalAngle += MOUSE_SPEED * deltaTime * float(1024/2 - xPos);
    g_verticalAngle += MOUSE_SPEED * deltaTime * float(768/2 - yPos);

    g_direction = {
        cos(g_verticalAngle) * sin(g_horizontalAngle),
        sin(g_verticalAngle),
        cos(g_verticalAngle) * cos(g_horizontalAngle)
    };
    g_right = {
        sin(g_horizontalAngle - M_PI/2),
        0,
        cos(g_horizontalAngle - M_PI/2)
    };
    g_up = glm::cross(g_right, g_direction);

    if (glfwGetKey(g_window, GLFW_KEY_UP) == GLFW_PRESS)
        g_position += g_direction * deltaTime * SPEED;
    if (glfwGetKey(g_window, GLFW_KEY_DOWN) == GLFW_PRESS)
        g_position -= g_direction * deltaTime * SPEED;
    if (glfwGetKey(g_window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        g_position += g_right * deltaTime * SPEED;
    if (glfwGetKey(g_window, GLFW_KEY_LEFT) == GLFW_PRESS)
        g_position -= g_right * deltaTime * SPEED;
}

static void
showStatus(double boxHeight, double fps)
{
    static bool firstRun = true;

    if (firstRun) {
        firstRun = false;
        goto draw;
    }
    std::cout << "\x1b[1F"; // one line up
    std::cout << "\x1b[2K"; // erase line
    std::cout << "\x1b[1F"; // one line up
    std::cout << "\x1b[2K"; // erase line
draw:
    std::cout << "Box height: " << boxHeight << std::endl
              << fps << " ms/Frame" << std::endl;
}
