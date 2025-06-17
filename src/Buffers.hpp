#ifndef FDVS_SRC_BUFFERS_HPP_
#define FDVS_SRC_BUFFERS_HPP_

#ifndef GL_HEADERS_
#define GL_HEADERS_
// Glad first
#include <glad/glad.h>

// GLFW second
#include <GLFW/glfw3.h>
#endif // !GL_HEADERS_

#include "Data.hpp"
#include <cstddef>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace Buffers {

class RBO {
public:
  RBO(GLsizei width, GLsizei height);
  void Rescale(GLsizei width, GLsizei height);
  void Bind() { glBindRenderbuffer(GL_RENDERBUFFER, rbo_id_); }
  void Unbind() { glBindRenderbuffer(GL_RENDERBUFFER, 0); }
  GLuint GetBufferID() { return rbo_id_; }
  ~RBO();

private:
  GLuint rbo_id_ = 0;
};

// Texture Object
class Texture2D {
public:
  Texture2D(GLsizei width, GLsizei height);
  void Rescale(GLsizei width, GLsizei height);
  void Bind() { glBindTexture(GL_TEXTURE_2D, texture_id_); }
  void Unbind() { glBindTexture(GL_TEXTURE_2D, 0); }
  GLuint GetBufferID() { return texture_id_; }
  ~Texture2D();

private:
  GLuint texture_id_ = 0;
};

// Framebuffer Object
class FBO {
public:
  FBO(GLsizei width, GLsizei height);

  // Getter Functions
  GLuint GetBufferID() { return fbo_buffer_id_; }
  GLuint GetAttachedTextureID() { return linked_texture_->GetBufferID(); }
  GLuint GetAttachedRBOID() { return linked_rbo_->GetBufferID(); }

  // Bind Unbind Delete
  void Bind() { glBindFramebuffer(GL_FRAMEBUFFER, fbo_buffer_id_); }
  void Unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
  void BindAttachments() {
    linked_rbo_->Bind();
    linked_texture_->Bind();
  }
  void UnbindAttachments() {
    linked_rbo_->Unbind();
    linked_texture_->Unbind();
  }
  ~FBO();

  // Rescale
  void Rescale(GLsizei width, GLsizei height);

private:
  GLuint fbo_buffer_id_ = 0;
  std::shared_ptr<Buffers::Texture2D> linked_texture_;
  std::shared_ptr<Buffers::RBO> linked_rbo_;
  bool CheckFboBindingStatus(std::string msg);
};

// Vertex Buffer Object
class VBO {
public:
  VBO();
  GLuint GetBufferID() { return vbo_buffer_id_; }

  // Bind Unbind Delete
  void Bind() { glBindBuffer(GL_ARRAY_BUFFER, vbo_buffer_id_); }
  void Unbind() { glBindBuffer(GL_ARRAY_BUFFER, 0); }
  void SetData(std::vector<float> vertices);
  ~VBO();

private:
  GLuint vbo_buffer_id_ = 0;
  std::vector<float> vertices_;
};

class EBO {
public:
  // Constructor
  EBO();

  // Getter and Setter Functions
  GLuint GetBufferID() { return ebo_buffer_id_; }

  // Bind Unbind Delete
  void Bind() { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_buffer_id_); }
  void Unbind() { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }

  void SetData(const std::vector<unsigned int> &faces);
  ~EBO();

private:
  GLuint ebo_buffer_id_ = 0;
  std::vector<unsigned int> faces;
};

class VAO {
public:
  // Constructor
  VAO(bool EBO_FLAG);
  void SetData(std::shared_ptr<Data::OffMeshData> data);
  void SetData(std::vector<float> vertices);
  void SetVertexAttribute();

  // Getter and Setter Functions
  GLuint GetBufferID() { return vao_buffer_id_; }
  void SetVertexAttributeSize(GLint size) {
    size_ = size;
    SetVertexAttribute();
  }
  void SetVertexAttributeOffset(void *offset) {
    offset_ = offset;
    SetVertexAttribute();
  }

  // Bind Unbind Delete
  void Bind() { glBindVertexArray(GetBufferID()); }
  void Unbind() {
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
  }
  void BindEBO() {
    if (EBO_FLAG_)
      ebo_->Bind();
  }
  void UnbindEBO() {
    if (EBO_FLAG_)
      ebo_->Unbind();
  }
  void BindVBO() { vbo_->Bind(); }
  void UnbindVBO() { vbo_->Unbind(); }
  ~VAO();

private:
  GLuint vao_buffer_id_ = 0;
  bool EBO_FLAG_ = false;
  std::shared_ptr<VBO> vbo_;
  std::shared_ptr<EBO> ebo_;
  void *offset_ = (void *)0;
  GLint size_ = 3;
  GLsizei stride_ = 3 * sizeof(float);
};

std::vector<float> Vec3ToFloatArr(std::initializer_list<glm::vec3> vertices);

} // namespace Buffers
#endif
