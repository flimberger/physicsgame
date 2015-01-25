#include "Window.hxx"

#include <iostream>

Window::Window(float mouseSpeed, float movementSpeed)
    : m_glfwWindow{nullptr}, m_numberOfFrames{0}, m_lastLoopTime{glfwGetTime()},
      m_currentFps{0}, m_mouseSpeed{mouseSpeed}, m_movementSpeed{movementSpeed},
      m_glfwInitialized{false}
{
}

Window::~Window()
{
    if (m_glfwInitialized) {
        glDeleteVertexArrays(1, &m_vertexArrayId);
        glfwTerminate();
    }
}

glm::mat4 Window::BeginLoop()
{
    ProcessInputs();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    return glm::lookAt(m_viewport.position,
                       m_viewport.position + m_viewport.direction,
                       m_viewport.up);
}

void Window::EndLoop()
{
    glfwSwapBuffers(m_glfwWindow);
    glfwPollEvents();

    double currentTime = glfwGetTime();

    ++m_numberOfFrames;
    if (currentTime - m_lastLoopTime > 1.0) {
        m_currentFps = 1000.0 / m_numberOfFrames;
        m_numberOfFrames = 0;
        m_lastLoopTime += 1.0;
    }
}

const Viewport &Window::GetViewport() const { return m_viewport; }

bool Window::Init()
{
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW." << std::endl;

        return false;
    }
    m_glfwInitialized = true;
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    m_glfwWindow =
        glfwCreateWindow(1024, 768, "Bullet + OpenGL", nullptr, nullptr);
    glfwMakeContextCurrent(m_glfwWindow);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW." << std::endl;
        glfwTerminate();
        exit(1);
    }
    glfwSetInputMode(m_glfwWindow, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetInputMode(m_glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glGenVertexArrays(1, &m_vertexArrayId);
    glBindVertexArray(m_vertexArrayId);
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);

    return true;
}

bool Window::IsLooping() const
{
    return glfwGetKey(m_glfwWindow, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwGetKey(m_glfwWindow, GLFW_KEY_Q) != GLFW_PRESS &&
           glfwWindowShouldClose(m_glfwWindow) == 0;
}

double Window::GetFps() const
{
    return m_currentFps;
}

void Window::ProcessInputs()
{
    static double lastTime = glfwGetTime();
    double now = glfwGetTime();
    float horizontalAngle;
    float verticalAngle;
    float deltaTime = float(now - lastTime);
    double xPos, yPos;

    lastTime = now;
    glfwGetCursorPos(m_glfwWindow, &xPos, &yPos);

    // TODO: glfwGetWindowSize
    horizontalAngle = static_cast<float>(xPos) * m_mouseSpeed * -1.0f;
    verticalAngle = static_cast<float>(yPos) * m_mouseSpeed * -1.0f;

    glm::vec3 dir{cos(verticalAngle) * sin(horizontalAngle), sin(verticalAngle),
                  cos(verticalAngle) * cos(horizontalAngle)};

    std::clog << "Direction: (" << dir.x << ", " << dir.y << ", " << dir.z
              << ")" << std::endl;
    m_viewport.direction = dir;
    m_viewport.right = glm::vec3{sin(horizontalAngle - M_PI / 2), 0,
                                 cos(horizontalAngle - M_PI / 2)};
    m_viewport.up = glm::cross(m_viewport.right, m_viewport.direction);

    if (glfwGetKey(m_glfwWindow, GLFW_KEY_UP) == GLFW_PRESS) {
        m_viewport.position +=
            m_viewport.direction * deltaTime * m_movementSpeed;
    }
    if (glfwGetKey(m_glfwWindow, GLFW_KEY_DOWN) == GLFW_PRESS) {
        m_viewport.position -=
            m_viewport.direction * deltaTime * m_movementSpeed;
    }
    if (glfwGetKey(m_glfwWindow, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        m_viewport.position += m_viewport.right * deltaTime * m_movementSpeed;
    }
    if (glfwGetKey(m_glfwWindow, GLFW_KEY_LEFT) == GLFW_PRESS) {
        m_viewport.position -= m_viewport.right * deltaTime * m_movementSpeed;
    }
}
