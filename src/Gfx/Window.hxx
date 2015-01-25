#pragma once

#include <GL/glew.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include <GLFW/glfw3.h>

struct Viewport
{
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 right;
    glm::vec3 up;
};

struct Window
{
    Window(float mouseSpeed, float movementSpeed);
    Window(const Window &) = delete;
    ~Window();

    glm::mat4 BeginLoop();
    void EndLoop();
    const Viewport &GetViewport() const;
    bool Init();
    bool IsLooping() const;

    double GetFps() const;

  public:
    void ProcessInputs();

    Viewport m_viewport;
    GLFWwindow *m_glfwWindow;
    std::size_t m_numberOfFrames;
    double m_lastLoopTime;
    double m_currentFps;
    GLuint m_vertexArrayId;
    const float m_mouseSpeed;
    const float m_movementSpeed;
    bool m_glfwInitialized;
};
