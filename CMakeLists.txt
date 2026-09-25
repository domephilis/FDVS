cmake_minimum_required(VERSION 3.24.4...4.4)
include(FetchContent)

project(
  FDVS
  VERSION 1.0
)

set(GLFW_BUILD_WAYLAND OFF CACHE INTERNAL "")
set(GLM_BUILD_TESTS OFF CACHE INTERNAL "")
add_compile_definitions(GLM_ENABLE_EXPERIMENTAL)

FetchContent_Declare(
  glfw3
  GIT_REPOSITORY  https://github.com/glfw/glfw
  GIT_TAG         92dcf4ce74f2e2554a98fea09be7c705c17daa5a
  )

FetchContent_Declare(
	glm
	GIT_REPOSITORY	https://github.com/g-truc/glm.git
	GIT_TAG 	    0af55ccecd98d4e5a8d1fad7de25ba429d60e863 #refs/tags/1.0.1
)
  
FetchContent_MakeAvailable(glfw3 glm)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")

set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/extern/imgui)
set(GLAD_H_DIR ${CMAKE_CURRENT_SOURCE_DIR}/extern/glad)
set(GLAD_C_DIR ${CMAKE_CURRENT_SOURCE_DIR}/extern/glad)
set(NLOHMANN_JSON_DIR ${CMAKE_CURRENT_SOURCE_DIR}/extern/nlohmann_json/include)

# nlohmann_json has its own CMakeLists.txt
# We can directly link to the targets therein
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/extern/nlohmann_json)

add_executable(program.out 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

target_sources(program.out PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Themes.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ShaderHandler.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Events.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MatrixHandler.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Buffers.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Data.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Models.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Strategy.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Panel.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Graphics.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Application.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/PricingPolicies.cpp 
  # Header Files
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Themes.hpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ShaderHandler.hpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Events.hpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MatrixHandler.hpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Buffers.hpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Data.hpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Models.hpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Strategy.hpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Panel.hpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Graphics.hpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Application.hpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/PricingPolicies.hpp 
)

if(CMAKE_CURRENT_SOURCE_DIR)
  target_compile_definitions(program.out PRIVATE CMAKE_CURRENT_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR})
endif()

add_library(ExtGraphics
  # Source Files
  ${GLAD_C_DIR}/glad.c 
  ${IMGUI_DIR}/imgui.cpp
  ${IMGUI_DIR}/imgui_impl_glfw.cpp
  ${IMGUI_DIR}/imgui_impl_opengl3.cpp
  ${IMGUI_DIR}/imgui_demo.cpp
  ${IMGUI_DIR}/imgui_draw.cpp
  ${IMGUI_DIR}/imgui_tables.cpp
  ${IMGUI_DIR}/imgui_widgets.cpp

  # Header Files
  ${GLAD_H_DIR}/glad.h
  ${IMGUI_DIR}/imgui.h
  ${IMGUI_DIR}/imgui_impl_glfw.h
  ${IMGUI_DIR}/imgui_impl_opengl3.h
)

target_include_directories(ExtGraphics
  PUBLIC
  ${GLAD_H_DIR}
  ${IMGUI_DIR}
)

target_link_libraries(program.out PRIVATE 
  glm::glm-header-only
  glfw
  ExtGraphics
  nlohmann_json::nlohmann_json)

target_include_directories(program.out
  PRIVATE
  ${NLOHMANN_JSON_DIR}
)
