# Sets the minimum version of CMake required to build the native
# library. You should either keep the default value or only pass a
# value of 3.4.0 or lower.

cmake_minimum_required(VERSION 3.4.1)
set(CMAKE_CXX_FLAGS  "-Wall -std=c++11")
set(ROOT_PATH "../..")
set(COMMON_PATH ${ROOT_PATH}/common)
set(TRIANGLE_PATH ${ROOT_PATH}/triangle)
set(NATIVE_PATH "src/main/cpp")

find_library(glm-lib glm)
find_library (egl-lib  EGL)
find_library (gles-lib  GLESv2)
find_library (android-lib  android)

include_directories("/usr/local/include")

include_directories(${ROOT_PATH})

include_directories(${COMMON_PATH})
add_library(common-lib ${COMMON_PATH}/Context.cpp)

include_directories(${TRIANGLE_PATH})
add_library(triangle-lib
            ${TRIANGLE_PATH}/Renderer.cpp
            ${TRIANGLE_PATH}/RendererFactory.cpp)
target_link_libraries(triangle-lib ${egl-lib})
target_link_libraries(triangle-lib ${gles-lib})


add_library( # Sets the name of the library.
             native-lib

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             # Associated headers in the same location as their source
             # file are automatically included.
             ${NATIVE_PATH}/native-lib.cpp
             ${NATIVE_PATH}/Bootstrap.cpp
             )
add_dependencies(native-lib common-lib)
add_dependencies(native-lib triangle-lib)

# Searches for a specified prebuilt library and stores the path as a
# variable. Because system libraries are included in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

find_library (android-lib  android)
find_library (egl-lib    EGL)
find_library (gles-lib   GLESv2)
find_library (glm-lib    glm)
# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in the
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                       native-lib

                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib})

target_link_libraries(native-lib ${android-lib})
target_link_libraries(native-lib ${egl-lib})
target_link_libraries(native-lib ${gles-lib})
target_link_libraries(native-lib common-lib)
target_link_libraries(native-lib triangle-lib)