cmake_minimum_required(VERSION 3.16)
project(RobotArmSimulation LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

add_executable(RobotArmSimulation
    src/main.cpp
    src/Camera.cpp
    src/RobotArm.cpp
    src/SceneObject.cpp
    src/InputState.cpp
    src/MathUtil.cpp
)

target_include_directories(RobotArmSimulation PRIVATE src)

if (TARGET OpenGL::GL)
    set(_OPENGL_LIB OpenGL::GL)
else()
    set(_OPENGL_LIB ${OPENGL_gl_LIBRARY})
endif()

if (TARGET OpenGL::GLU)
    set(_GLU_LIB OpenGL::GLU)
else()
    set(_GLU_LIB ${OPENGL_glu_LIBRARY})
endif()

if (TARGET GLUT::GLUT)
    set(_GLUT_LIB GLUT::GLUT)
else()
    set(_GLUT_LIB ${GLUT_LIBRARIES})
    target_include_directories(RobotArmSimulation PRIVATE ${GLUT_INCLUDE_DIR})
endif()

target_link_libraries(RobotArmSimulation PRIVATE ${_OPENGL_LIB} ${_GLU_LIB} ${_GLUT_LIB})

if (WIN32)
    target_compile_definitions(RobotArmSimulation PRIVATE _USE_MATH_DEFINES NOMINMAX)
endif()
