#ifndef FDVS_SRC_APPLICATION_H_
#define FDVS_SRC_APPLICATION_H_

// GLAD First

#include <glad/glad.h>

// GLFW3 Second

#include <GLFW/glfw3.h>

// C++ Libraries

#include <algorithm>
#include <bits/stdc++.h>
#include <boost/preprocessor/stringize.hpp>
#include <ctime>
#include <iostream>
#include <memory>
#include <string>

// OpenGL Mathematics

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// ImGui Libraries

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// My Own Libraries
#include "Buffers.hpp"
#include "Data.hpp"
#include "Graphics.hpp"
#include "shader_handler.hpp"

struct PROGRAM_CONSTRUCTION_ERROR {
  std::string errString() { return "Program Construction Error"; }
};
struct SHADER_CONSTRUCTION_ERROR : public PROGRAM_CONSTRUCTION_ERROR {
  std::string errString() { return "Shader Construction Error"; }
};
struct GLAD_LOADING_ERROR : public PROGRAM_CONSTRUCTION_ERROR {
  std::string errString() { return "Glad Loading Error"; }
};
struct GLFW_WINDOW_CREATION_FAILURE : public PROGRAM_CONSTRUCTION_ERROR {
  std::string errString() { return "Glfw Window Creation Failure"; }
};
struct WINDOW_NOT_IN_FOCUS {};

void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id,
                                GLenum severity, GLsizei length,
                                const GLchar *message, const void *userParam);

class Application {
public:
  Application();
  bool isOpen();
  void Refresh();
  void Close();

private:
  int app_x_, app_y_;
  GLFWwindow *window;
  std::shared_ptr<Shader> shader;
  ImGuiIO *io;

  // Graph Window
  std::shared_ptr<Buffers::FBO> graph_fbo_;
  std::shared_ptr<Graphics::TriangleMesh> graph;
  Data::OffMeshData data{};

  void CreateWindow();
  void InitializeNewFrame();
  void SetupImGui();
  const ImVec2 GraphWindow(Buffers::FBO &fb);
  void Render();
};

#endif
