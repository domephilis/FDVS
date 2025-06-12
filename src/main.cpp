#include "Application.hpp"
#include <iostream>
#include <memory>

int main() {
  std::unique_ptr<Application> program;
  try {
    program = std::make_unique<Application>();
  } catch (GLFW_WINDOW_CREATION_FAILURE gwcf) {
    std::cerr << gwcf.errString() << std::endl;
    return -1;
  } catch (PROGRAM_CONSTRUCTION_ERROR pge) {
    std::cerr << pge.errString() << std::endl;
    return -1;
  } catch (...) {
    return -1;
  }
  while (program->isOpen()) {
    try {
      program->Refresh();
    } catch (WINDOW_NOT_IN_FOCUS wnif) {
      continue;
    }
  }
  program->Close();
  return 0;
}
