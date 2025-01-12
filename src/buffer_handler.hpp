#include <cstddef>
#include <glad/glad.h>
#include <iostream>

class BufferBase {
public:
  virtual void Bind() = 0;
  virtual void UnBind() = 0;
  virtual GLuint GetBufferID() = 0;
};

class FBO : public BufferBase {
public:
  FBO(float width, float height); // Width height used to be GLuin

  // FBO Attachments
  void AttachTexture();
  void AttachRBO();

  // Getter Functions
  virtual GLuint GetBufferID();
  GLuint GetAttachedTextureID();
  GLuint GetAttachedRBOID();

  // Bind Unbind Delete
  virtual void Bind();
  virtual void Unbind();
  void ~FBO();

  // Rescale
  void RescaleFramebuffer(float width, float height);

private:
  GLuint FboBufferID = 0;
  GLuint LinkedTextureID = 0;
  GLuint LinkedRBOID = 0;
};

class VBO : public BufferBase {
public:
  VBO(std::vector<float> vertices);
  virtual GLuint GetBufferID();

  // Bind Unbind Delete
  virtual void Bind();
  virtual void Unbind();
  void ~VBO();

private:
  GLuint VboBufferID = 0;
};

class EBO {
public:
private:
};

class VAO {
public:
private:
};

class VAO {
public:
private:
};

struct STATE_MACHINE

    class BufferHandler {
public:
  void Draw();

private:
};
