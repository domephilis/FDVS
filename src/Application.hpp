#ifndef FDVS_SRC_APPLICATION_HPP_
#define FDVS_SRC_APPLICATION_HPP_

#ifndef GL_HEADERS_
#define GL_HEADERS_

// GLAD First

#include <glad/glad.h>

// GLFW3 Second

#include <GLFW/glfw3.h>

#endif

// C++ Libraries

#include <algorithm>
#include <bits/stdc++.h>
#include <boost/preprocessor/stringize.hpp>
#include <ctime>
#include <iostream>
#include <map>
#include <memory>
#include <string>

// OpenGL Mathematics

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// My Own Libraries
#include "Events.hpp"
#include "Panel.hpp"
#include "ShaderHandler.hpp"

// ImGui Libraries

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// Some Themes Online
#include "Themes.hpp"

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
  GLFWwindow *window;
  std::shared_ptr<Shader> shader;
  ImGuiIO *io;
  std::shared_ptr<Events::Controller> io_ctr_;
  ImGuiContext *main_context;

  // Panels
  std::map<std::string, std::shared_ptr<Windowing::Panel>> panels;

  void CreateWindow();
  void InitializeNewFrame();
  void SetupImGui();
  void RenderMain();
};

#endif
