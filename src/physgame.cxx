#include "Gfx/LoadDDS.hxx"
#include "Gfx/LoadObj.hxx"
#include "Gfx/Material.hxx"
#include "Gfx/Mesh.hxx"
#include "Gfx/Model.hxx"
#include "Gfx/Program.hxx"
#include "Gfx/Shader.hxx"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wold-style-cast"
#pragma clang diagnostic ignored "-Wextra-semi"
#pragma clang diagnostic ignored "-Wfloat-equal"
#pragma clang diagnostic ignored "-Wdocumentation"
#pragma clang diagnostic ignored "-Wsign-conversion"
#pragma clang diagnostic ignored "-Woverloaded-virtual"
#pragma clang diagnostic ignored "-Wcast-align"
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wweak-vtables"
#include <btBulletDynamicsCommon.h>
#pragma clang diagnostic pop

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include <iostream>
#include <fstream>

namespace
{
struct Player
{
    Player();

    const glm::vec3 &GetPosition() const;
    const glm::vec3 &GetDirection() const;
    const glm::vec3 &GetRight() const;
    const glm::vec3 &GetUp() const;

    void SetPosition(const glm::vec3 &newPosition);
    void SetDirection(const glm::vec3 &newDirection);
    void SetRight(const glm::vec3 &newRight);
    void SetUp(const glm::vec3 &newUp);

  private:
    glm::vec3 m_position;
    glm::vec3 m_direction;
    glm::vec3 m_right;
    glm::vec3 m_up;
};
}

Player::Player() : m_position{3, 1, 9} {}

const glm::vec3 &Player::GetPosition() const { return m_position; }

const glm::vec3 &Player::GetDirection() const { return m_direction; }

const glm::vec3 &Player::GetRight() const { return m_right; }

const glm::vec3 &Player::GetUp() const { return m_up; }

void Player::SetPosition(const glm::vec3 &newPosition)
{
    m_position = newPosition;
}

void Player::SetDirection(const glm::vec3 &newDirection)
{
    m_direction = newDirection;
}

void Player::SetRight(const glm::vec3 &newRight) { m_right = newRight; }

void Player::SetUp(const glm::vec3 &newUp) { m_up = newUp; }

static GLuint g_vertexArrayId;

static const float SPEED{5.0f};
static const float MOUSE_SPEED{0.02f};

static GLFWwindow *SetupOpenGL();
static void CleanupOpenGL();
static std::unique_ptr<Program>
LoadShaders(const std::string &vertexShaderFile,
            const std::string &fragmentShaderFile);
static void ProcessInputs(GLFWwindow *window, Player *player);
static void ShowStatus(double boxHeight, double fps);

int main()
{
    auto window = SetupOpenGL();

    auto mesh =
        std::shared_ptr<Mesh>{LoadMeshFromObjFile("../res/textures/cube.obj")};

    if (!mesh) {
        CleanupOpenGL();
        std::cerr << "Failed to load mesh data" << std::endl;

        return 0;
    }

    auto shaderProgram = std::shared_ptr<Program>{LoadShaders(
        "../res/shaders/vertex.glsl", "../res/shaders/fragment.glsl")};

    if (!shaderProgram) {
        CleanupOpenGL();
        std::cerr << "Failed to load shader program" << std::endl;

        return 1;
    }

    GLuint programId = shaderProgram->GetGlId();
    GLint mvpMatrixId = glGetUniformLocation(programId, "mvpMatrix");
    GLint modelMatrixId = glGetUniformLocation(programId, "modelMatris");
    GLint viewMatrixId = glGetUniformLocation(programId, "modelMatrix");

    auto texture =
        std::shared_ptr<Texture>{LoadDDS("../res/textures/uvmap.DDS")};

    if (!texture) {
        CleanupOpenGL();
        std::cerr << "Failed to load texture" << std::endl;

        return 1;
    }

    glUseProgram(programId);
    GLint lightPositionId =
        glGetUniformLocation(programId, "lightPosition_worldspace");
    GLint lightColorId = glGetUniformLocation(programId, "lightColor");
    GLint lightPowerId = glGetUniformLocation(programId, "lightPower");

    auto material =
        std::shared_ptr<Material>{new Material{shaderProgram, texture}};
    auto model = std::unique_ptr<Model>{new Model{material, mesh}};
    auto player = std::unique_ptr<Player>{new Player};

    std::unique_ptr<btBroadphaseInterface> broadphase{new btDbvtBroadphase};
    std::unique_ptr<btDefaultCollisionConfiguration> collisionConfiguration{
        new btDefaultCollisionConfiguration};
    std::unique_ptr<btCollisionDispatcher> dispatcher{
        new btCollisionDispatcher{collisionConfiguration.get()}};
    std::unique_ptr<btSequentialImpulseConstraintSolver> solver{
        new btSequentialImpulseConstraintSolver};
    std::unique_ptr<btDiscreteDynamicsWorld> dynamicsWorld{
        new btDiscreteDynamicsWorld{dispatcher.get(), broadphase.get(),
                                    solver.get(),
                                    collisionConfiguration.get()}};
    dynamicsWorld->setGravity(btVector3{0, -10, 0});
    std::unique_ptr<btCollisionShape> groundShape{
        new btStaticPlaneShape{btVector3{0, 1, 0}, 1}};
    std::unique_ptr<btSphereShape> fallShape{new btSphereShape{1}};
    std::unique_ptr<btDefaultMotionState> groundMotionState{
        new btDefaultMotionState{
            btTransform{btQuaternion{0, 0, 0, 1}, btVector3{0, -1, 0}}}};
    btRigidBody::btRigidBodyConstructionInfo groundRigidBodyConstructionInfo{
        0, groundMotionState.get(), groundShape.get(), btVector3{0, 0, 0}};
    std::unique_ptr<btRigidBody> groundRigidBody{
        new btRigidBody{groundRigidBodyConstructionInfo}};

    dynamicsWorld->addRigidBody(groundRigidBody.get());

    std::unique_ptr<btDefaultMotionState> fallMotionState{
        new btDefaultMotionState{
            btTransform{btQuaternion{0, 0, 0, 1}, btVector3{0, 50, 0}}}};
    btScalar mass{1};
    btVector3 fallInertia{0, 0, 0};

    fallShape->calculateLocalInertia(mass, fallInertia);

    btRigidBody::btRigidBodyConstructionInfo fallRigidBodyConstructionInfo{
        mass, fallMotionState.get(), fallShape.get(), fallInertia};
    std::unique_ptr<btRigidBody> fallRigidBody{
        new btRigidBody{fallRigidBodyConstructionInfo}};

    dynamicsWorld->addRigidBody(fallRigidBody.get());

    glm::mat4 projectionMatrix =
        glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
    glm::mat4 viewMatrix;
    glm::mat4 modelMatrix;
    glm::mat4 mvp;
    glm::vec3 lightPosition{3, 3, 3};
    glm::vec3 lightColor{1, 1, 1};
    float lightPower{50.0f};

    btTransform transformation;

    double currentTime, lastTime = glfwGetTime();
    double fps{0.0};
    std::size_t nFrames{0l};

    do {
        dynamicsWorld->stepSimulation(1 / 60.0f, 10);
        fallRigidBody->getMotionState()->getWorldTransform(transformation);

        ProcessInputs(window, player.get());
        modelMatrix = glm::translate(
            glm::mat4(1.0f), glm::vec3{transformation.getOrigin().getX(),
                                       transformation.getOrigin().getY(),
                                       transformation.getOrigin().getZ()});
        viewMatrix = glm::lookAt(player->GetPosition(),
                                 player->GetPosition() + player->GetDirection(),
                                 player->GetUp());
        mvp = projectionMatrix * viewMatrix * modelMatrix;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUniformMatrix4fv(mvpMatrixId, 1, GL_FALSE, &mvp[0][0]);
        glUniformMatrix4fv(modelMatrixId, 1, GL_FALSE, &modelMatrix[0][0]);
        glUniformMatrix4fv(viewMatrixId, 1, GL_FALSE, &viewMatrix[0][0]);
        glUniform3f(lightPositionId, lightPosition.x, lightPosition.y,
                    lightPosition.z);
        glUniform3f(lightColorId, lightColor.x, lightColor.y, lightColor.z);
        glUniform1f(lightPowerId, lightPower);

        model->Draw();

        currentTime = glfwGetTime();
        ++nFrames;
        if (currentTime - lastTime > 1.0) {
            fps = 1000.0 / nFrames;
            nFrames = 0;
            lastTime += 1.0;
        }
        ShowStatus(transformation.getOrigin().getY(), fps);

        glfwSwapBuffers(window);
        glfwPollEvents();
    } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
             glfwGetKey(window, GLFW_KEY_Q) != GLFW_PRESS &&
             glfwWindowShouldClose(window) == 0);
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
GLFWwindow *SetupOpenGL()
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

    auto window = glfwCreateWindow(1024, 768, "Bullet + OpenGL", nullptr, nullptr);

    if (window == nullptr) {
        std::cerr << "Failed to open GLFW window." << std::endl;
        glfwTerminate();
        exit(1);
    }
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW." << std::endl;
        glfwTerminate();
        exit(1);
    }
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glGenVertexArrays(1, &g_vertexArrayId);
    glBindVertexArray(g_vertexArrayId);

    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);

    return window;
}

static void CleanupOpenGL()
{
    glDeleteVertexArrays(1, &g_vertexArrayId);
    glfwTerminate();
}

static std::unique_ptr<Program>
LoadShaders(const std::string &vertexShaderFile,
            const std::string &fragmentShaderFile)
{
    auto shaders = std::vector<std::shared_ptr<Shader>>{};

    shaders.push_back(
        std::make_shared<Shader>(VERTEX_SHADER, vertexShaderFile));
    shaders.push_back(
        std::make_shared<Shader>(FRAGMENT_SHADER, fragmentShaderFile));

    auto shaderProgram = std::unique_ptr<Program>{new Program{shaders}};

    if (!shaderProgram->Link()) {
        return std::unique_ptr<Program>{};
    }
    return shaderProgram;
}

static void ProcessInputs(GLFWwindow *window, Player *player)
{
    static double lastTime = glfwGetTime();
    double now = glfwGetTime();
    float horizontalAngle;
    float verticalAngle;
    float deltaTime = float(now - lastTime);
    double xPos, yPos;

    lastTime = now;
    glfwGetCursorPos(window, &xPos, &yPos);

    // TODO: glfwGetWindowSize
    horizontalAngle = static_cast<float>(xPos) * MOUSE_SPEED * -1.0f;
    verticalAngle = static_cast<float>(yPos) * MOUSE_SPEED * -1.0f;

    glm::vec3 dir{cos(verticalAngle) * sin(horizontalAngle), sin(verticalAngle),
                  cos(verticalAngle) * cos(horizontalAngle)};

    std::clog << "Direction: (" << dir.x << ", " << dir.y << ", " << dir.z
              << ")" << std::endl;
    player->SetDirection(dir);
    player->SetRight(glm::vec3{sin(horizontalAngle - M_PI / 2), 0,
                               cos(horizontalAngle - M_PI / 2)});
    player->SetUp(glm::cross(player->GetRight(), player->GetDirection()));

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        player->SetPosition(player->GetPosition() +
                            player->GetDirection() * deltaTime * SPEED);
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        player->SetPosition(player->GetPosition() -
                            player->GetDirection() * deltaTime * SPEED);
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        player->SetPosition(player->GetPosition() +
                            player->GetRight() * deltaTime * SPEED);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        player->SetPosition(player->GetPosition() -
                            player->GetRight() * deltaTime * SPEED);
    }
}

static void ShowStatus(double boxHeight, double fps)
{
    // static bool firstRun = true;

    // if (firstRun) {
    //     firstRun = false;
    // } else {
    //     std::cout << "\x1b[2A"; // two lines up
    //     std::cout << "\x1b[J";  // erase line
    // }
    std::cout << "Box height: " << boxHeight << std::endl << fps << " ms/Frame"
              << std::endl;
}
