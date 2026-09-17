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

#list(APPEND CMAKE_PREFIX_PATH "/usr/local/lib/glfw/")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")
#find_package(CGAL REQUIRED)

file(
  GLOB cppfiles
  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/extern/imgui)
set(GLAD_H_DIR ${CMAKE_CURRENT_SOURCE_DIR}/extern/glad)
set(GLAD_C_DIR ${CMAKE_CURRENT_SOURCE_DIR}/extern/glad)
set(NLOHMANN_JSON_DIR ${CMAKE_CURRENT_SOURCE_DIR}/extern/nlohmann_json/include)

set(HEADER_FILES ${IMGUI_DIR}/imgui.h ${IMGUI_DIR}/imgui_impl_glfw.h ${IMGUI_DIR}/imgui_impl_opengl3.h ${CMAKE_CURRENT_SOURCE_DIR}/src/ShaderHandler.hpp ${CMAKE_CURRENT_SOURCE_DIR}/src/Buffers.hpp ${CMAKE_CURRENT_SOURCE_DIR}/src/Data.hpp ${CMAKE_CURRENT_SOURCE_DIR}/src/Application.hpp ${CMAKE_CURRENT_SOURCE_DIR}/src/Events.hpp ${CMAKE_CURRENT_SOURCE_DIR}/src/MatrixHandler.hpp)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/extern/nlohmann_json)

add_library(glad ${GLAD_C_DIR}/glad.c ${GLAD_H_DIR}/glad.h)

add_library(imgui ${IMGUI_DIR}/imgui.cpp ${IMGUI_DIR}/imgui.h)
add_library(imgui_backend ${IMGUI_DIR}/imgui_impl_glfw.cpp ${IMGUI_DIR}/imgui_impl_glfw.h)
add_library(imgui_renderer ${IMGUI_DIR}/imgui_impl_opengl3.cpp ${IMGUI_DIR}/imgui_impl_opengl3.h)
add_library(imgui_demo ${IMGUI_DIR}/imgui_demo.cpp)
add_library(imgui_draw ${IMGUI_DIR}/imgui_draw.cpp)
add_library(imgui_tables ${IMGUI_DIR}/imgui_tables.cpp)
add_library(imgui_widgets ${IMGUI_DIR}/imgui_widgets.cpp)

add_library(Themes ${CMAKE_CURRENT_SOURCE_DIR}/src/Themes.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/Themes.hpp)
add_library(ShaderHandler ${CMAKE_CURRENT_SOURCE_DIR}/src/ShaderHandler.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/ShaderHandler.hpp)
add_library(Events ${CMAKE_CURRENT_SOURCE_DIR}/src/Events.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/Events.hpp)
add_library(MatrixHandler ${CMAKE_CURRENT_SOURCE_DIR}/src/MatrixHandler.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/MatrixHandler.hpp)
add_library(Buffers ${CMAKE_CURRENT_SOURCE_DIR}/src/Buffers.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/Buffers.hpp)
add_library(Data ${CMAKE_CURRENT_SOURCE_DIR}/src/Data.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/Data.hpp)
add_library(Models ${CMAKE_CURRENT_SOURCE_DIR}/src/Models.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/Models.hpp)
add_library(Strategy ${CMAKE_CURRENT_SOURCE_DIR}/src/Strategy.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/Strategy.hpp)
add_library(Panel ${CMAKE_CURRENT_SOURCE_DIR}/src/Panel.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/Panel.hpp)
add_library(Graphics ${CMAKE_CURRENT_SOURCE_DIR}/src/Graphics.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/Graphics.hpp)
add_library(Application ${CMAKE_CURRENT_SOURCE_DIR}/src/Application.cpp ${HEADER_FILES})
add_library(PricingPolicies ${CMAKE_CURRENT_SOURCE_DIR}/src/PricingPolicies.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/PricingPolicies.hpp)

add_executable(program.out ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/Application.hpp)

if(CMAKE_CURRENT_SOURCE_DIR)
  target_compile_definitions(Application PRIVATE CMAKE_CURRENT_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR})
  target_compile_definitions(Panel PRIVATE CMAKE_CURRENT_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR})
endif()

target_include_directories(program.out PRIVATE ${IMGUI_DIR} ${GLAD_H_DIR} ${glm_SOURCE_DIR})
target_include_directories(Themes PRIVATE ${IMGUI_DIR})
target_include_directories(Application PRIVATE ${IMGUI_DIR} ${GLAD_H_DIR})
target_include_directories(Panel PRIVATE ${IMGUI_DIR} ${GLAD_H_DIR})
target_include_directories(Buffers PRIVATE ${GLAD_H_DIR} ${glm_SOURCE_DIR})
target_include_directories(Events PRIVATE ${IMGUI_DIR} ${GLAD_H_DIR})
target_include_directories(MatrixHandler PRIVATE ${IMGUI_DIR} ${GLAD_H_DIR} ${glm_SOURCE_DIR})
target_include_directories(ShaderHandler PRIVATE ${GLAD_H_DIR} ${glm_SOURCE_DIR})
target_include_directories(Data PRIVATE ${glm_SOURCE_DIR})
target_include_directories(Graphics PRIVATE ${IMGUI_DIR} ${GLAD_H_DIR} ${glm_SOURCE_DIR})
target_include_directories(Models PRIVATE ${NLOHMANN_JSON_DIR})
target_include_directories(PricingPolicies PRIVATE ${NLOHMANN_JSON_DIR})
target_include_directories(Panel PRIVATE ${NLOHMANN_JSON_DIR} ${glm_SOURCE_DIR})
target_include_directories(Application PRIVATE ${NLOHMANN_JSON_DIR} ${glm_SOURCE_DIR})
target_include_directories(program.out PRIVATE ${NLOHMANN_JSON_DIR})

target_link_libraries(Models PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(imgui_backend PRIVATE glfw)
target_link_libraries(Strategy PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(Themes PRIVATE imgui imgui_draw imgui_tables imgui_widgets)
target_link_libraries(Events PRIVATE glfw imgui_backend)
target_link_libraries(Data PRIVATE glm::glm-header-only)
target_link_libraries(Buffers PRIVATE glad Data glm::glm-header-only)
target_link_libraries(ShaderHandler PRIVATE glad glm::glm-header-only)
target_link_libraries(MatrixHandler PRIVATE Events ShaderHandler glm::glm-header-only)
target_link_libraries(Graphics PRIVATE glad Buffers Data ShaderHandler MatrixHandler glm::glm-header-only)
target_link_libraries(Panel PRIVATE imgui imgui_backend imgui_renderer imgui_demo imgui_draw imgui_tables imgui_widgets Themes ShaderHandler Buffers Data Events Graphics MatrixHandler Models Strategy PricingPolicies ${CMAKE_DL_LIBS} glm::glm-header-only)
target_link_libraries(Application PRIVATE glad glfw imgui imgui_backend imgui_renderer imgui_demo imgui_draw imgui_tables imgui_widgets Themes Events ShaderHandler Panel glm::glm-header-only)
target_link_libraries(program.out PRIVATE Application glad glm glfw)
