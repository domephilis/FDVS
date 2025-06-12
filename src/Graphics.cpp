#include "Graphics.hpp"

Graphics::Box::Box(std::shared_ptr<Shader> element_shader,
                   std::shared_ptr<Buffers::FBO> target) {
  element_shader_ = element_shader;
  data_.vertices = vertices;
  data_.faces = faces;
  data_.num_of_vertices = 8;
  data_.num_of_faces = 8;
  vao_ = std::make_shared<Buffers::VAO>(true);
  std::cerr << vao_->GetBufferID() << std::endl;

  SetDrawTarget(target);

  fbo_->Bind();
  vao_->Bind();
  vao_->SetData(data_);

  // Unbind
  vao_->Unbind();
  fbo_->Unbind();
}

void Graphics::Box::drawToBuffer(GLsizei s_x, GLsizei s_y) {
  fbo_->Bind();
  glViewport(0, 0, s_x, s_y);

  element_shader_->use();
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  // Set up projection matrix
  ProjectionMatrices perspective = ProjectionMatrices(
      glm::translate(glm::mat4(1.0f), glm::vec3(-100.0f, 20.0f, 0.0f)),
      glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -10.0f)),
      glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f));
  /*
  ProjectionMatrices perspective =
      ProjectionMatrices(glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f));*/
  perspective.pushToShader(*element_shader_);

  // Draw what is in the buffer
  vao_->Bind();
  vao_->BindEBO();
  int32_t size = 0;
  glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
  std::cerr << size << std::endl;
  glDrawElements(GL_TRIANGLES, data_.num_of_faces * 3, GL_UNSIGNED_INT, 0);

  // Cleanup
  vao_->UnbindEBO();
  vao_->Unbind();
  fbo_->Unbind();
  glUseProgram(0);
}

Graphics::TriangleMesh::TriangleMesh(std::shared_ptr<Shader> element_shader,
                                     Data::OffMeshData data,
                                     std::shared_ptr<Buffers::FBO> target) {

  element_shader_ = element_shader;
  data_ = data;
  vao_ = std::make_shared<Buffers::VAO>(true);

  SetDrawTarget(target);

  fbo_->Bind();
  vao_->Bind();
  vao_->SetData(data);

  // Unbind
  vao_->Unbind();
  fbo_->Unbind();
}

void Graphics::TriangleMesh::updateData(Data::OffMeshData new_data) {
  data_ = new_data;
  vao_->SetData(new_data);
}

void Graphics::TriangleMesh::drawToBuffer(GLsizei s_x, GLsizei s_y) {
  fbo_->Bind();
  glViewport(0, 0, s_x, s_y);

  element_shader_->use();
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  // Set up projection matrix
  ProjectionMatrices perspective = ProjectionMatrices(
      glm::mat4(0.9f),
      glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -10.0f)),
      glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 1000.0f));
  perspective.pushToShader(*element_shader_);

  // Draw what is in the buffer
  vao_->Bind();
  vao_->BindEBO();
  glDrawElements(GL_TRIANGLES, data_.num_of_faces * 3, GL_UNSIGNED_INT, 0);

  // Cleanup
  vao_->UnbindEBO();
  vao_->Unbind();
  fbo_->Unbind();
  glUseProgram(0);
}

void Graphics::ProjectionMatrices::pushToShader(const Shader &shader) {
  int modelID = glGetUniformLocation(shader.ID, "model");
  int viewID = glGetUniformLocation(shader.ID, "view");
  int projectionID = glGetUniformLocation(shader.ID, "projection");

  glUniformMatrix4fv(modelID, 1, GL_FALSE, glm::value_ptr(model_));
  glUniformMatrix4fv(viewID, 1, GL_FALSE, glm::value_ptr(view_));
  glUniformMatrix4fv(projectionID, 1, GL_FALSE, glm::value_ptr(projection_));
}
