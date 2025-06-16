#ifndef FDVS_SRC_SHADER_HANDLER_HPP_
#define FDVS_SRC_SHADER_HANDLER_HPP_

#ifndef GL_HEADERS_
#define GL_HEADERS_
// GLAD First
#include <glad/glad.h>

// GLFW3 Second
#include <GLFW/glfw3.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

class Shader {
public:
  // Program ID
  unsigned int ID;

  // Constructor
  Shader(const char *vertexPath, const char *fragmentPath);

  // Use/Activate the Shader
  void use();

  // utility uniform functions
  void setMatrix(const std::string &name, glm::mat4 matrix) const;
  void setBool(const std::string &name, bool value) const;
  void setInt(const std::string &name, int value) const;
  void setFloat(const std::string &name, float value) const;
};

#endif
