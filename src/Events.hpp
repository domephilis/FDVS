#ifndef FDVS_SRC_EVENTS_HPP_
#define FDVS_SRC_EVENTS_HPP_

#ifndef GL_HEADERS_
#define GL_HEADERS_

// GLAD First

#include <glad.h>

// GLFW3 Second

#include <GLFW/glfw3.h>

#endif

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_internal.h"

#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>

namespace Events {

ImGuiKey GLFWKeyToImGuiKey(int key);

class KeyboardSubscriber {
public:
  virtual void Update(int key, int action) = 0;
  virtual ImGuiWindow *GetWindow() { return nullptr; }
  virtual bool WantCaptureKeyboard() { return true; }
  virtual ~KeyboardSubscriber() {}
};

class ScrollwheelSubscriber {
public:
  virtual void Update(double yoffset) = 0;
  virtual ImGuiWindow *GetWindow() { return nullptr; }
  virtual bool WantCaptureScroll() { return true; }
  virtual ~ScrollwheelSubscriber() {}
};

class MouseSubscriber {
public:
  virtual void Update(double xpos, double ypos) = 0;
  virtual bool WantCaptureMouse() { return true; }
  bool IsLeftMouseButtonPressed() { return click_state_; }
  virtual void UpdateMouseButtonState(bool click_state) { click_state_ = click_state; }
  virtual ImGuiWindow *GetWindow() { return nullptr; }
  virtual void ResetCentre() {}
  virtual ~MouseSubscriber() {}

protected:
  bool click_state_ = false;
};

class KeyboardPublisher {
public:
  KeyboardPublisher(GLFWwindow *window) : window_(window) {
    glfwSetKeyCallback(window_, KeyCallback);
  }
  void addSubscriber(std::shared_ptr<KeyboardSubscriber> s) {
    subscribers_.push_back(s);
  }
  void removeSubscriber(std::shared_ptr<KeyboardSubscriber> s) {
    subscribers_.erase(std::remove(subscribers_.begin(), subscribers_.end(), s),
                       subscribers_.end());
  }
  static void KeyCallback(GLFWwindow *window, int key, int scancode, int action,
                          int mods);

  GLFWwindow *window_;
  std::vector<std::shared_ptr<KeyboardSubscriber>> subscribers_;
};

class MousePublisher {
public:
  MousePublisher(GLFWwindow *window) : window_(window) {
    glfwSetCursorPosCallback(window_, CursorPositionCallback);
    // glfwSetInputMode(window_, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);
  }
  void addSubscriber(std::shared_ptr<MouseSubscriber> s) {
    subscribers_.push_back(s);
  }
  void removeSubscriber(std::shared_ptr<MouseSubscriber> s) {
    subscribers_.erase(std::remove(subscribers_.begin(), subscribers_.end(), s),
                       subscribers_.end());
  }
  static void CursorPositionCallback(GLFWwindow *window, double xpos,
                                     double ypos);

  GLFWwindow *window_;
  std::vector<std::shared_ptr<MouseSubscriber>> subscribers_;
};

class ScrollwheelPublisher {
public:
  ScrollwheelPublisher(GLFWwindow *window) : window_(window) {
    glfwSetScrollCallback(window_, ScrollCallback);
    std::cerr << "Set Scroll Callback" << std::endl;
  }
  void addSubscriber(std::shared_ptr<ScrollwheelSubscriber> s) {
    subscribers_.push_back(s);
  }
  void removeSubscriber(std::shared_ptr<ScrollwheelSubscriber> s) {
    subscribers_.erase(std::remove(subscribers_.begin(), subscribers_.end(), s),
                       subscribers_.end());
  }
  static void ScrollCallback(GLFWwindow *window, double xoffset,
                             double yoffset);

  GLFWwindow *window_;
  std::vector<std::shared_ptr<ScrollwheelSubscriber>> subscribers_;
};

class Controller {
public:
  Controller(GLFWwindow *window);
  ~Controller() {}

  GLFWwindow *window_;
  ImGuiIO *imgui_io_;

  std::vector<ImGuiContext *> contexts;
  std::unique_ptr<KeyboardPublisher> k_publisher_;
  std::unique_ptr<MousePublisher> m_publisher_;
  std::unique_ptr<ScrollwheelPublisher> s_publisher_;

  static void MouseButtonCallback(GLFWwindow *window, int button, int action,
                                  int mods);
  static void CharCallback(GLFWwindow *window, unsigned int codepoint);
};

} // namespace Events

#endif // !FDVS_SRC_EVENTS_HPP_
