#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <iostream>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "shader_handler.hpp"

class Renderer {
public:
  std::vector<Subpanel &> Subpanels;

  Renderer(); // Constructor
  unsigned int render_loop();
  ~Renderer();
};

class Subpanel {
public:
  virtual Subpanel();
  virtual unsigned int render_function();
  virtual ~Subpanel();
};

class Config : public Subpanel {
public:
  Config();
  unsigned int render_function();
  ~Config();
};

class Graph : public Subpanel {
public:
  Graph();
  unsigned int render_function();
  ~Graph();
};

class TabularData : public Subpanel {
public:
  TabularData();
  unsigned int render_function();
  ~TabularData();
}
