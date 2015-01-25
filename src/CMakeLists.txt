find_package(Bullet REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)

# CMake uses absolute paths, pkg-config doesn't,
# so linking breaks on FreeBSD
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L/usr/local/lib")

# While we're at it, glm is a header-only library and may need following line
# add_definitions("-I /usr/local/include")

include_directories(${BULLET_INCLUDE_DIRS})
include_directories(${GLEW_INCLUDE_DIRS})
include_directories(${GLFW_INCLUDE_DIRS})

# this really sucks, currently static libs are used on FreeBSD,
# but there may be shared libs on other systems
set(physgame_LIBS
    ${BULLET_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${GLFW_STATIC_LIBRARIES}
)

set(physgame_GFX_SRCS
    Gfx/LoadDDS.cxx
    Gfx/LoadObj.cxx
    Gfx/Material.cxx
    Gfx/Mesh.cxx
    Gfx/Model.cxx
    Gfx/Program.cxx
    Gfx/Shader.cxx
    Gfx/Texture.cxx
    Gfx/Window.cxx)

set(physgame_PHYSICS_SRCS
    Physics/PhysicsWorld.cxx)

set(physgame_SRCS
    physgame.cxx
    ${physgame_GFX_SRCS}
    ${physgame_PHYSICS_SRCS})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

add_executable(physgame ${physgame_SRCS})
target_link_libraries(physgame ${physgame_LIBS})
