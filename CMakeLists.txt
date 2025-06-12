cmake_minimum_required(VERSION 3.15...3.22)

project(
  FDVS
  VERSION 1.0
)

list(APPEND CMAKE_PREFIX_PATH "/usr/local/lib/glfw/")
find_package(glfw3 REQUIRED)

set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/extern/imgui)
set(GLAD_C_DIR ${CMAKE_CURRENT_SOURCE_DIR}/extern/glad)
set(GLAD_H_DIR /usr/include/glad)

set(HEADER_FILES ${IMGUI_DIR}/imgui.h ${IMGUI_DIR}/imgui_impl_glfw.h ${IMGUI_DIR}/imgui_impl_opengl3.h src/shader_handler.hpp src/Buffers.hpp src/Data.hpp src/Application.hpp)

add_library(glad ${GLAD_C_DIR}/glad.c ${GLAD_H_DIR}/glad.h)

add_library(imgui ${IMGUI_DIR}/imgui.cpp ${IMGUI_DIR}/imgui.h)
add_library(imgui_backend ${IMGUI_DIR}/imgui_impl_glfw.cpp ${IMGUI_DIR}/imgui_impl_glfw.h)
add_library(imgui_renderer ${IMGUI_DIR}/imgui_impl_opengl3.cpp ${IMGUI_DIR}/imgui_impl_opengl3.h)
add_library(imgui_demo ${IMGUI_DIR}/imgui_demo.cpp)
add_library(imgui_draw ${IMGUI_DIR}/imgui_draw.cpp)
add_library(imgui_tables ${IMGUI_DIR}/imgui_tables.cpp)
add_library(imgui_widgets ${IMGUI_DIR}/imgui_widgets.cpp)

add_library(shader_handler src/shader_handler.cpp src/shader_handler.hpp)
add_library(Buffers src/Buffers.cpp src/Buffers.hpp)
add_library(Data src/Data.cpp src/Data.hpp)
add_library(Graphics src/Graphics.cpp src/Graphics.hpp)
add_library(Application src/Application.cpp ${HEADER_FILES})

add_executable(program.out src/main.cpp ${HEADER_FILES})

if(CMAKE_CURRENT_SOURCE_DIR)
  target_compile_definitions(Application PRIVATE CMAKE_CURRENT_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR})
endif()

target_include_directories(program.out PRIVATE ${IMGUI_DIR})
target_include_directories(Application PRIVATE ${IMGUI_DIR})
target_link_libraries(program.out PRIVATE Application)
target_link_libraries(Application PRIVATE glad glfw3 imgui imgui_backend imgui_renderer imgui_demo imgui_draw imgui_tables imgui_widgets Buffers Data Graphics shader_handler)
target_link_libraries(shader_handler PRIVATE glad)
target_link_libraries(Buffers PRIVATE glad Data)
target_link_libraries(Graphics PRIVATE glad Buffers Data shader_handler)
