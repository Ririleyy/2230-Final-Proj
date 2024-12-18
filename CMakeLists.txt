cmake_minimum_required(VERSION 3.16)

# Sets project name
project(2230-final-project LANGUAGES CXX C)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Sets C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Specifies required Qt components
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS OpenGL)
find_package(Qt6 REQUIRED COMPONENTS OpenGLWidgets)
find_package(Qt6 REQUIRED COMPONENTS Gui)

# Allows you to include files from within those directories, without prefixing their filepaths
include_directories(src)

# Specifies .cpp and .h files to be passed to the compiler
add_executable(${PROJECT_NAME}
    src/main.cpp
    src/glrenderer.cpp
    src/mainwindow.cpp
    src/utils/camera.cpp
    src/settings.cpp
    src/utils/terrain.cpp
    src/utils/terrainQueue.cpp
    src/utils/particle.cpp
  
    src/glrenderer.h
    src/mainwindow.h
    src/shaderloader.h
    src/utils/camera.h
    src/settings.h
    src/utils/terrain.h
    src/utils/terrainQueue.h
    src/utils/particle.h



)
# Specifies other files
qt_add_resources(${PROJECT_NAME} "Resources"
  PREFIX
  "/"
  FILES
  resources/shaders/skydome.vert
  resources/shaders/skydome.frag
  resources/shaders/terrain.vert
  resources/shaders/terrain.frag
  resources/shaders/particle.frag
  resources/shaders/particle.vert
  resources/shaders/water.frag
  resources/shaders/water.vert

  resources/images/indian-travel-destination-beautiful-attractive.jpg
  resources/images/2.jpg
  resources/images/natural-landscape.jpg
  resources/images/water_displacement_0.png
  resources/images/water_displacement_1.jpg
  resources/images/water_displacement_2.jpg
  # resources/images/water_displacement_3.jpg
  # resources/images/water_displacement_4.jpg
  resources/images/front-view-tree-bark.jpg
  resources/images/top-view-corn-flour-texture.jpg

  resources/images/snowNewtop.jpg
  resources/images/snowSand.jpg
  resources/images/grassTop.jpg
  resources/images/rockSand.jpg
  resources/images/seaFloor.jpg
  resources/images/rockMiddle.jpg

  )

# GLEW: this creates its library and allows you to `#include "GL/glew.h"`
add_library(StaticGLEW STATIC glew/src/glew.c)
include_directories(${PROJECT_NAME} PRIVATE glew/include)

# Specifies libraries to be linked (Qt components, glew, etc)
target_link_libraries(${PROJECT_NAME} PRIVATE
  Qt::Core
  Qt::Widgets
  Qt::OpenGL
  Qt::OpenGLWidgets
  Qt::Gui
  StaticGLEW
)

# GLEW: this provides support for Windows (including 64-bit)
if (WIN32)
  add_compile_definitions(GLEW_STATIC)
  target_link_libraries(${PROJECT_NAME} PRIVATE
    opengl32
    glu32
  )
endif()

# Set this flag to silence warnings on Windows
if (MSVC OR MSYS OR MINGW)
  set(CMAKE_CXX_FLAGS "-Wno-volatile")
endif()

# Set this flag to silence warnings on MacOS
if (APPLE)
  set(CMAKE_CXX_FLAGS "-Wno-deprecated-volatile")
endif()
