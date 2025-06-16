#include "Events.hpp"

Events::Controller::Controller(GLFWwindow *window) {
  window_ = window;
  k_publisher_ = std::make_unique<KeyboardPublisher>(window_);
  m_publisher_ = std::make_unique<MousePublisher>(window_);
}

void Events::KeyboardPublisher::KeyCallback(GLFWwindow *window, int key,
                                            int scancode, int action,
                                            int mods) {
  Events::Controller *ctr =
      static_cast<Events::Controller *>(glfwGetWindowUserPointer(window));
  for (std::shared_ptr<KeyboardSubscriber> s :
       ctr->k_publisher_->subscribers_) {
    if (s->IsUpdateAllowed())
      s->Update(key, action);
  }
}

void Events::MousePublisher::CursorPositionCallback(GLFWwindow *window,
                                                    double xpos, double ypos) {
  Events::Controller *ctr =
      static_cast<Events::Controller *>(glfwGetWindowUserPointer(window));
  for (std::shared_ptr<MouseSubscriber> s : ctr->m_publisher_->subscribers_) {
    s->UpdateClickState(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) ==
                        GLFW_PRESS);
    if (s->IsUpdateAllowed()) {
      s->Update(xpos, ypos);
    }
  }
}
