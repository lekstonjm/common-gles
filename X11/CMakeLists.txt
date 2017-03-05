cmake_minimum_required(VERSION 3.4.1)
project(simple_triangle)

set(CMAKE_CXX_FLAGS  "-Wall -std=c++11")
set(ROOT_PATH "..")
set(COMMON_PATH ${ROOT_PATH}/common)
set(TRIANGLE_PATH ${ROOT_PATH}/triangle)

find_package(X11 REQUIRED)
find_package(glm REQUIRED)
find_library(glm-lib glm)
find_library (egl-lib  EGL)
find_library (gles-lib  GLESv2)
find_library (x11-lib  X11)

include_directories(${COMMON_PATH})
add_library(common-lib ${COMMON_PATH}/Context.cpp)

include_directories(${TRIANGLE_PATH})
add_library(triangle-lib
            ${TRIANGLE_PATH}/Renderer.cpp
            ${TRIANGLE_PATH}/RendererFactory.cpp)
target_link_libraries(triangle-lib ${egl-lib})
target_link_libraries(triangle-lib ${gles-lib})

include_directories(${ROOT_PATH})
add_executable(simple-triangle
                main.cpp
                Bootstrap.cpp)
add_dependencies(simple-triangle triangle-lib)
add_dependencies(simple-triangle common-lib)
target_link_libraries(simple-triangle ${x11-lib})
target_link_libraries(simple-triangle common-lib)
target_link_libraries(simple-triangle triangle-lib)
target_link_libraries(simple-triangle ${egl-lib})
target_link_libraries(simple-triangle ${gles-lib})
