#include "Gfx/LoadDDS.hxx"
#include "Gfx/LoadObj.hxx"
#include "Gfx/Material.hxx"
#include "Gfx/Mesh.hxx"
#include "Gfx/Model.hxx"
#include "Gfx/Program.hxx"
#include "Gfx/Shader.hxx"
#include "Gfx/Window.hxx"
#include "Physics/PhysicsWorld.hxx"

#include <iostream>
#include <fstream>

static std::unique_ptr<Program>
LoadShaders(const std::string &vertexShaderFile,
            const std::string &fragmentShaderFile);
static void ShowStatus(double boxHeight, double fps);

int main()
{
    Window window{0.02f, 5.0f};

    window.Init();

    auto mesh =
        std::shared_ptr<Mesh>{LoadMeshFromObjFile("../res/textures/cube.obj")};

    if (!mesh) {
        std::cerr << "Failed to load mesh data" << std::endl;

        return 1;
    }

    auto shaderProgram = std::shared_ptr<Program>{LoadShaders(
        "../res/shaders/vertex.glsl", "../res/shaders/fragment.glsl")};

    if (!shaderProgram) {
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

    PhysicsWorld physicsWorld{btVector3{0, -10, 0}};

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

    auto dynamicsWorld = physicsWorld.GetDynamicsWorld();

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

    do {
        dynamicsWorld->stepSimulation(1 / 60.0f, 10);
        fallRigidBody->getMotionState()->getWorldTransform(transformation);

        viewMatrix = window.BeginLoop();
        modelMatrix = glm::translate(
            glm::mat4(1.0f), glm::vec3{transformation.getOrigin().getX(),
                                       transformation.getOrigin().getY(),
                                       transformation.getOrigin().getZ()});
        mvp = projectionMatrix * viewMatrix * modelMatrix;

        glUniformMatrix4fv(mvpMatrixId, 1, GL_FALSE, &mvp[0][0]);
        glUniformMatrix4fv(modelMatrixId, 1, GL_FALSE, &modelMatrix[0][0]);
        glUniformMatrix4fv(viewMatrixId, 1, GL_FALSE, &viewMatrix[0][0]);
        glUniform3f(lightPositionId, lightPosition.x, lightPosition.y,
                    lightPosition.z);
        glUniform3f(lightColorId, lightColor.x, lightColor.y, lightColor.z);
        glUniform1f(lightPowerId, lightPower);

        model->Draw();

        window.EndLoop();
        ShowStatus(transformation.getOrigin().getY(), window.GetFps());
    } while (window.IsLooping());
    dynamicsWorld->removeRigidBody(fallRigidBody.get());
    dynamicsWorld->removeRigidBody(groundRigidBody.get());

    return 0;
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
