#ifndef FDVS_SRC_DATA_HPP_
#define FDVS_SRC_DATA_HPP_

#include <iostream>
#include <vector>

namespace Data {

// Store Data Retrieved from Off Files
struct OffMeshData {
  unsigned int num_of_vertices = 0;
  unsigned int num_of_faces = 0;
  unsigned int num_of_edges = 0;
  std::vector<float> vertices;
  std::vector<unsigned int> faces;
};

// OFF File Helper Methods

void ReadOffFileAttrib(unsigned int &vertices, unsigned int &facets,
                       unsigned int &edges, std::string file_name);
void ReadOffData(std::vector<float> &vertices_arr,
                 std::vector<unsigned int> &faces_arr, std::string file_name);

} // namespace Data

#endif // !FDVS_SRC_DATA_H_
