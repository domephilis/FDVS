#ifndef FDVS_SRC_DATA_HPP_
#define FDVS_SRC_DATA_HPP_

// glm
#include <glm/glm.hpp>

// CPP Libraries
#include <iostream>
#include <string>
#include <vector>

namespace Data {

// Store Data Retrieved from Off Files
struct OffMeshData {
  OffMeshData(std::vector<float> in_vertices,
              std::vector<unsigned int> in_faces);
  OffMeshData(std::string filename);

  // Properties
  unsigned int num_of_vertices = 0;
  unsigned int num_of_faces = 0;
  unsigned int num_of_edges = 0;
  glm::vec3 centre;
  glm::vec3 max;
  glm::vec3 min;

  // Points
  std::vector<float> vertices;
  std::vector<unsigned int> faces;

private:
  void ComputeProperties();
  void ReadOffFileAttrib(unsigned int &vertices, unsigned int &facets,
                         unsigned int &edges, std::string file_name);
  void ReadOffData(std::vector<float> &vertices_arr,
                   std::vector<unsigned int> &faces_arr, std::string file_name);
};

} // namespace Data

#endif // !FDVS_SRC_DATA_H_
