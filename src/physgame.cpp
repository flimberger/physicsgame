#include "Model.hpp"
#include "LoadDDS.hpp"
#include "LoadObj.hpp"

#include <btBulletDynamicsCommon.h>

#include <GL/glew.h>

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

static GLuint s_vertexArrayId;
static GLuint s_vertexBufferId;
static GLuint s_uvCoordBufferId;
static GLuint s_normalBufferId;
static GLuint s_textureId;
static GLuint s_lightPositionId;
static GLuint s_lightColorId;
static GLuint s_lightPowerId;
static GLuint s_uniformTextureId;
static GLuint s_programId;
static GLuint s_mvpMatrixId;
static GLuint s_modelMatrixId;
static GLuint s_viewMatrixId;
static GLFWwindow *s_window;
static Model s_model;
static glm::vec3 s_position { 3, 1, 9 };
static glm::vec3 s_direction;
static glm::vec3 s_right;
static glm::vec3 s_up;
static float s_horizontalAngle { 3.14f };
static float s_verticalAngle { 0.0f };
static const float SPEED { 5.0f };
static const float MOUSE_SPEED { 0.02f };

static void SetupOpenGL();
static void CleanupOpenGL();
static void LoadShaders(const std::string &vertexShaderFile, const std::string &fragmentShaderFile);
static void CompileShader(GLuint shaderId, const std::string sourceFile);
static void ProcessInputs();
static void ShowStatus(double boxHeight, double fps);

int
main()
{
    SetupOpenGL();

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

        ProcessInputs();
        modelMatrix = glm::translate(glm::mat4 { 1.0f }, glm::vec3 {
            transformation.getOrigin().getX(),
            transformation.getOrigin().getY(),
            transformation.getOrigin().getZ()
        });
        viewMatrix = glm::lookAt(
            s_position,
            s_position + s_direction,
            s_up
        );
        mvp = projectionMatrix * viewMatrix * modelMatrix;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(s_programId);
        glUniformMatrix4fv(s_mvpMatrixId, 1, GL_FALSE, &mvp[0][0]);
        glUniformMatrix4fv(s_modelMatrixId, 1, GL_FALSE, &modelMatrix[0][0]);
        glUniformMatrix4fv(s_viewMatrixId, 1, GL_FALSE, &viewMatrix[0][0]);
        glUniform3f(s_lightPositionId, lightPosition.x, lightPosition.y, lightPosition.z);
        glUniform3f(s_lightColorId, lightColor.x, lightColor.y, lightColor.z);
        glUniform1f(s_lightPowerId, lightPower);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, s_textureId);
        glUniform1i(s_uniformTextureId, 0);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, s_vertexBufferId);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, s_uvCoordBufferId);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, s_normalBufferId);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        glDrawArrays(GL_TRIANGLES, 0, s_model.vertices().size());

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);

        currentTime = glfwGetTime();
        ++nFrames;
        if (currentTime - lastTime > 1.0) {
            fps = 1000.0/nFrames;
            nFrames = 0;
            lastTime += 1.0;
        }
        ShowStatus(transformation.getOrigin().getY(), fps);

        glfwSwapBuffers(s_window);
        glfwPollEvents();
    } while (glfwGetKey(s_window, GLFW_KEY_ESCAPE) != GLFW_PRESS
             && glfwGetKey(s_window, GLFW_KEY_Q) != GLFW_PRESS
             && glfwWindowShouldClose(s_window) == 0);
    dynamicsWorld->removeRigidBody(fallRigidBody.get());
    dynamicsWorld->removeRigidBody(groundRigidBody.get());
    CleanupOpenGL();

    return 0;
}

/*
 * Setup OpenGL
 *
 * Set up GLFW and GLEW, opens a window.
 */
void
SetupOpenGL()
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

    s_window = glfwCreateWindow(1024, 768, "Bullet + OpenGL", nullptr, nullptr);

    if (s_window == nullptr) {
        std::cerr << "Failed to open GLFW window." << std::endl;
        glfwTerminate();
        exit(1);
    }
    glfwMakeContextCurrent(s_window);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW." << std::endl;
        glfwTerminate();
        exit(1);
    }
    glfwSetInputMode(s_window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetCursorPos(s_window, 1024/2, 768/2);

    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);

    glGenVertexArrays(1, &s_vertexArrayId);
    glBindVertexArray(s_vertexArrayId);

    s_model = LoadModelFromObjFile("../res/textures/cube.obj");

    glGenBuffers(1, &s_vertexBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, s_vertexBufferId);
    glBufferData(GL_ARRAY_BUFFER, s_model.vertices().size() * sizeof(glm::vec3),
                  &s_model.vertices()[0], GL_STATIC_DRAW);

    glGenBuffers(1, &s_uvCoordBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, s_uvCoordBufferId);
    glBufferData(GL_ARRAY_BUFFER, s_model.uvCoords().size() * sizeof(glm::vec2),
                 &s_model.uvCoords()[0], GL_STATIC_DRAW);

    glGenBuffers(1, &s_normalBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, s_normalBufferId);
    glBufferData(GL_ARRAY_BUFFER, s_model.normals().size() * sizeof(glm::vec3),
                 &s_model.normals()[0], GL_STATIC_DRAW);

    LoadShaders("../res/shaders/vertex.glsl", "../res/shaders/fragment.glsl");
    s_mvpMatrixId = glGetUniformLocation(s_programId, "mvpMatrix");
    s_modelMatrixId = glGetUniformLocation(s_programId, "modelMatris");
    s_viewMatrixId = glGetUniformLocation(s_programId, "modelMatrix");

    s_textureId = LoadDDS("../res/textures/uvmap.DDS");
    s_uniformTextureId = glGetUniformLocation(s_programId, "myTextureSampler");

    glUseProgram(s_programId);
    s_lightPositionId = glGetUniformLocation(s_programId, "lightPosition_worldspace");
    s_lightColorId = glGetUniformLocation(s_programId, "lightColor");
    s_lightPowerId = glGetUniformLocation(s_programId, "lightPower");
}

static void
CleanupOpenGL()
{
    glDeleteProgram(s_programId);
    glDeleteBuffers(1, &s_normalBufferId);
    glDeleteBuffers(1, &s_uvCoordBufferId);
    glDeleteBuffers(1, &s_vertexBufferId);
    glDeleteVertexArrays(1, &s_vertexArrayId);
    glfwTerminate();
}

static void
LoadShaders(const std::string &vertexShaderFile, const std::string &fragmentShaderFile)
{
    s_programId = glCreateProgram();
    GLuint vsId = glCreateShader(GL_VERTEX_SHADER);
    GLuint fsId = glCreateShader(GL_FRAGMENT_SHADER);
    GLint result = GL_FALSE;
    GLint infoLogLength;

    CompileShader(vsId, vertexShaderFile);
    CompileShader(fsId, fragmentShaderFile);
    glAttachShader(s_programId, vsId);
    glAttachShader(s_programId, fsId);
    glLinkProgram(s_programId);
    glGetProgramiv(s_programId, GL_LINK_STATUS, &result);
    glGetProgramiv(s_programId, GL_INFO_LOG_LENGTH, &infoLogLength);

    std::vector<char> infoLog(infoLogLength);

    glGetProgramInfoLog(s_programId, infoLogLength, nullptr, &infoLog[0]);
    std::cerr << &infoLog[0] << std::endl;
    glDeleteShader(vsId);
    glDeleteShader(fsId);
}

static void
CompileShader(GLuint shaderId, const std::string sourceFile)
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
ProcessInputs()
{
    static double lastTime = glfwGetTime();
    double now = glfwGetTime();
    float deltaTime = float(now - lastTime);
    double xPos, yPos;

    lastTime = now;
    glfwGetCursorPos(s_window, &xPos, &yPos);
    // TODO: glfwGetWindowSize
    glfwSetCursorPos(s_window, 1024/2, 768/2);
    s_horizontalAngle += MOUSE_SPEED * deltaTime * float(1024/2 - xPos);
    s_verticalAngle += MOUSE_SPEED * deltaTime * float(768/2 - yPos);

    s_direction = {
        cos(s_verticalAngle) * sin(s_horizontalAngle),
        sin(s_verticalAngle),
        cos(s_verticalAngle) * cos(s_horizontalAngle)
    };
    s_right = {
        sin(s_horizontalAngle - M_PI/2),
        0,
        cos(s_horizontalAngle - M_PI/2)
    };
    s_up = glm::cross(s_right, s_direction);

    if (glfwGetKey(s_window, GLFW_KEY_UP) == GLFW_PRESS)
        s_position += s_direction * deltaTime * SPEED;
    if (glfwGetKey(s_window, GLFW_KEY_DOWN) == GLFW_PRESS)
        s_position -= s_direction * deltaTime * SPEED;
    if (glfwGetKey(s_window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        s_position += s_right * deltaTime * SPEED;
    if (glfwGetKey(s_window, GLFW_KEY_LEFT) == GLFW_PRESS)
        s_position -= s_right * deltaTime * SPEED;
}

static void
ShowStatus(double boxHeight, double fps)
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
