cmake_minimum_required(VERSION 3.8)
project(lab2)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        main.cpp)

add_executable(lab2 ${SOURCE_FILES})

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} )

target_link_libraries(lab2 ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} )
