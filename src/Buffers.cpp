#include "Buffers.hpp"

// Renderbuffer Methods

Buffers::RBO::RBO(GLsizei width, GLsizei height) {
  glGenRenderbuffers(1, &rbo_id_);
  Bind();
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
  Unbind();
}
void Buffers::RBO::Rescale(GLsizei width, GLsizei height) {
  Bind();
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
  Unbind();
}

Buffers::RBO::~RBO() { glDeleteBuffers(1, &rbo_id_); }

// Texture Methods

Buffers::Texture2D::Texture2D(GLsizei width, GLsizei height) {
  glGenTextures(1, &texture_id_);
  Bind();
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  Unbind();
}

void Buffers::Texture2D::Rescale(GLsizei width, GLsizei height) {
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

Buffers::Texture2D::~Texture2D() { glDeleteTextures(1, &texture_id_); }

// FBO Methods
Buffers::FBO::FBO(GLsizei width, GLsizei height) {
  glGenFramebuffers(1, &fbo_buffer_id_);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_buffer_id_);
  this->CheckFboBindingStatus("Inital FBO Creation: ");
  linked_texture_ = std::make_shared<Buffers::Texture2D>(width, height);
  linked_texture_->Bind();
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         linked_texture_->GetBufferID(), 0);
  linked_rbo_ = std::make_shared<RBO>(width, height);
  linked_rbo_->Bind();
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                            GL_RENDERBUFFER, linked_rbo_->GetBufferID());

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cerr << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n";
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  linked_texture_->Unbind();
  linked_rbo_->Unbind();
}

void Buffers::FBO::Rescale(GLsizei width, GLsizei height) {
  Bind();
  linked_texture_->Bind();
  linked_texture_->Rescale(width, height);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         linked_texture_->GetBufferID(), 0);

  linked_rbo_->Bind();
  linked_rbo_->Rescale(width, height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                            GL_RENDERBUFFER, linked_rbo_->GetBufferID());

  linked_texture_->Unbind();
  linked_rbo_->Unbind();
  Unbind();
}
bool Buffers::FBO::CheckFboBindingStatus(std::string msg) {
  bool success = false;
  GLint drawFboId = 0, readFboId = 0;
  glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &drawFboId);
  glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &readFboId);

  if (drawFboId == fbo_buffer_id_) {
    success = true;
    std::cerr << msg + "FBO successfully bound." << std::endl;
  } else {
    std::cerr << msg + "User FBO not successfully bound" << std::endl;
  }
  if (readFboId == fbo_buffer_id_) {
    std::cerr << msg + "FBO successfully bound." << std::endl;
  } else {
    success = false;
    std::cerr << msg + "User FBO not successfully bound" << std::endl;
  }

  return success;
}

Buffers::FBO::~FBO() { glDeleteBuffers(1, &fbo_buffer_id_); }

/****************************************************************
 *                                                              *
 *                 Vertex and Element Buffers                   *
 *                                                              *
 ****************************************************************/

// VBO Methods
Buffers::VBO::VBO() { glGenBuffers(1, &vbo_buffer_id_); }

void Buffers::VBO::SetData(std::vector<float> vertices) {
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
               &vertices.front(), GL_STATIC_DRAW);
}

Buffers::VBO::~VBO() { glDeleteBuffers(1, &vbo_buffer_id_); }

// EBO Methods
Buffers::EBO::EBO() { glGenBuffers(1, &ebo_buffer_id_); }
void Buffers::EBO::SetData(const std::vector<unsigned int> &faces) {
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size() * sizeof(unsigned int),
               &faces.front(), GL_STATIC_DRAW);
}
Buffers::EBO::~EBO() { glDeleteBuffers(1, &ebo_buffer_id_); }

// VAO Methods
Buffers::VAO::VAO(bool EBO_FLAG) {
  EBO_FLAG_ = EBO_FLAG;
  glGenVertexArrays(1, &vao_buffer_id_);
  Bind();
  vbo_ = std::make_shared<VBO>();
  glBindBuffer(GL_ARRAY_BUFFER, vbo_->GetBufferID());
  if (EBO_FLAG_) {
    ebo_ = std::make_shared<EBO>();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_->GetBufferID());
  }
  SetVertexAttribute();
  vbo_->Unbind();
  if (EBO_FLAG_)
    ebo_->Unbind();
  Unbind();
}
void Buffers::VAO::SetData(std::shared_ptr<Data::OffMeshData> data) {
  vbo_->Bind();
  vbo_->SetData(data->vertices);

  ebo_->Bind();
  ebo_->SetData(data->faces);
  ebo_->Unbind();
  vbo_->Unbind();
}
void Buffers::VAO::SetData(std::vector<float> vertices) {
  if (EBO_FLAG_ == false) {
    vbo_->Bind();
    vbo_->SetData(vertices);
    vbo_->Unbind();
  }

  // Check
  int32_t size = 0;
  vbo_->Bind();
  glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
  vbo_->Unbind();
}
void Buffers::VAO::SetVertexAttribute() {
  glVertexAttribPointer(0, size_, GL_FLOAT, GL_FALSE, size_ * sizeof(float),
                        offset_);
}
Buffers::VAO::~VAO() { glDeleteVertexArrays(1, &vao_buffer_id_); }

std::vector<float>
Buffers::Vec3ToFloatArr(std::initializer_list<glm::vec3> vertices) {
  std::vector<float> arr;
  for (glm::vec3 vertex : vertices) {
    for (int i = 0; i < 3; i++)
      arr.push_back(vertex[i]);
  }
  return arr;
}
