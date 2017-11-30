cmake_minimum_required(VERSION 3.8)
project(lab5)

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        main.cpp
        vector3D.cpp)

add_executable(lab5 ${SOURCE_FILES})

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})

target_link_libraries(lab5 ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})
