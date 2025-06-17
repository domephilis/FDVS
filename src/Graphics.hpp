#ifndef FDVS_SRC_GRAPHICS_HPP_
#define FDVS_SRC_GRAPHICS_HPP_

#include <cstddef>
#include <iostream>
#include <memory>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Buffers.hpp"
#include "Data.hpp"
#include "MatrixHandler.hpp"
#include "ShaderHandler.hpp"

#include "imgui.h"

namespace Graphics {

// Graphics Base Class
class Element {
public:
  virtual void drawToBuffer(GLsizei s_x, GLsizei s_y) = 0;

protected:
  void SetDrawTarget(std::shared_ptr<Buffers::FBO> fbo) { fbo_ = fbo; }
  std::shared_ptr<Shader> element_shader_;
  std::shared_ptr<Buffers::FBO> fbo_;
  std::shared_ptr<Buffers::VAO> vao_; // true means with EBO which I'll assume
};

class Box : public Element {
public:
  Box(std::shared_ptr<Shader> element_shader,
      std::shared_ptr<Buffers::FBO> target);
  void drawToBuffer(GLsizei s_x, GLsizei s_y);

private:
  std::shared_ptr<Data::OffMeshData> data_;

  // Define the Vertices and Faces
  unsigned int num_of_vertices = 4, num_of_faces = 2, num_of_edges = 0;

  // Draw cube
  std::vector<float> vertices{

      -1.0f, -1.0f, 0.0f, // bottom left
      1.0f, 1.0f, 0.0f,   // top right
      -1.0f, 1.0f, 0.0f,  // top left
      1.0f, -1.0f, 0.0f,  // bottom right
                          // Same thing in the back

      -1.0f, -1.0f, 3.0f, // bottom left
      1.0f, 1.0f, 3.0f,   // top right
      -1.0f, 1.0f, 3.0f,  // top left
      1.0f, -1.0f, 3.0f   // bottom right */
                          /*
                            0.5f,  0.5f,  0.0f, // top right
                            0.5f,  -0.5f, 0.0f, // bottom right
                            -0.5f, -0.5f, 0.0f, // bottom left
                            -0.5f, 0.5f,  0.0f  // top left */
  };
  std::vector<unsigned int> faces{0, 1, 3, 1, 2, 0, 4, 5, 7, 5, 6, 4,
                                  0, 4, 2, 2, 6, 4, 3, 7, 1, 1, 5, 7};
  /*
  std::vector<unsigned int> faces{0, 1, 3, 1, 2, 3};*/
};

class Line : public Element {
public:
  Line(glm::vec3 start, glm::vec3 end, std::shared_ptr<Shader> element_shader,
       std::shared_ptr<Buffers::FBO> target);
  void drawToBuffer(GLsizei s_x, GLsizei s_y);

private:
  glm::vec3 start_;
  glm::vec3 end_;
};

class TriangleMesh : public Element {
public:
  TriangleMesh(std::shared_ptr<Shader> element_shader,
               std::shared_ptr<Data::OffMeshData> data,
               std::shared_ptr<Buffers::FBO> target);
  void drawToBuffer(GLsizei s_x, GLsizei s_y);
  void updateData(std::shared_ptr<Data::OffMeshData> new_data);

private:
  std::shared_ptr<Data::OffMeshData> data_;
};

class ProjectionMatrices {
public:
  ProjectionMatrices() {}
  explicit ProjectionMatrices(glm::mat4 model, glm::mat4 view,
                              glm::mat4 projection)
      : model_(model), view_(view), projection_(projection) {}
  // ! Require that uniforms be named model view and projection
  void pushToShader(const Shader &shader);

private:
  glm::mat4 model_;
  glm::mat4 view_;
  glm::mat4 projection_;
};

} // namespace Graphics
#endif
