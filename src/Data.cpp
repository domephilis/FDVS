#include <boost/lexical_cast.hpp>
#include <fstream>
#include <iostream>

#include "Data.hpp"

Data::OffMeshData::OffMeshData(std::vector<float> in_vertices,
                               std::vector<unsigned int> in_faces)
    : vertices(in_vertices), faces(in_faces) {
  num_of_vertices = vertices.size() / 3;
  num_of_faces = faces.size() / 3;
  ComputeProperties();
}

Data::OffMeshData::OffMeshData(std::string filename) {
  ReadOffFileAttrib(num_of_vertices, num_of_faces, num_of_edges, filename);
  ReadOffData(vertices, faces, filename);
  ComputeProperties();
}

void Data::OffMeshData::ReadOffFileAttrib(unsigned int &vertices,
                                          unsigned int &facets,
                                          unsigned int &edges,
                                          std::string file_name) {
  std::ifstream in(file_name);
  std::string buffer;
  in >> buffer;
  if (buffer == "OFF") {
    in >> buffer;
    vertices = boost::lexical_cast<unsigned int>(buffer);
    in >> buffer;
    facets = boost::lexical_cast<unsigned int>(buffer);
    in >> buffer;
    edges = boost::lexical_cast<unsigned int>(buffer);
  }
  in.close();
}

void Data::OffMeshData::ReadOffData(std::vector<float> &vertices_arr,
                                    std::vector<unsigned int> &faces_arr,
                                    std::string file_name) {

  unsigned int vertices = 0, facets = 0, edges = 0;
  std::ifstream in(file_name);
  std::string buffer;
  float buffer_float;
  unsigned int buffer_uint = 0;

  in >> buffer;
  if (buffer == "OFF") {
    in >> buffer;
    vertices = boost::lexical_cast<unsigned int>(buffer);
    in >> buffer;
    facets = boost::lexical_cast<unsigned int>(buffer);
    in >> buffer;
    edges = boost::lexical_cast<unsigned int>(buffer);

    // Read Vertices
    unsigned int it = 0;
    unsigned int counter_v = 1;
    while (it <= 3 * vertices) {
      in >> buffer;
      buffer_float = boost::lexical_cast<float>(buffer);
      if ((counter_v % 3) == 0) {
        vertices_arr.push_back(-buffer_float);
      } else
        vertices_arr.push_back(buffer_float);
      it++;
      counter_v++;
    }

    // Read Faces
    unsigned int counter = 1; // I don't know why but one makes it work
    while (it <= (3 * vertices + 4 * facets)) {
      in >> buffer;
      buffer_uint = boost::lexical_cast<unsigned int>(buffer);
      if ((counter % 4) != 0) {
        faces_arr.push_back(buffer_uint);
        if (counter < 50)
          std::cerr << buffer_uint << " ";
      }
      it++;
      counter++;
    }
  }
  in.close();
}

void Data::OffMeshData::ComputeProperties() {
  for (int i = 0; i < num_of_vertices; i++) {
    switch (i % 3) {
    case 0:
      if (max[0] < vertices[i])
        max[0] = vertices[i];
      if (min[0] > vertices[i])
        min[0] = vertices[i];
      break;
    case 1:
      if (max[1] < vertices[i])
        max[1] = vertices[i];
      if (min[1] > vertices[i])
        min[1] = vertices[i];
      break;
    case 2:
      if (max[2] < vertices[i])
        max[2] = vertices[i];
      if (min[2] > vertices[i])
        min[2] = vertices[i];
      break;
    }
  }
  centre = glm::vec3((max[0] - min[0]) / 2, (max[1] - min[1]) / 2,
                     (max[2] - min[2]) / 2);
}
